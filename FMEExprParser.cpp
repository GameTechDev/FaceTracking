/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or imlied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "FMEExprParser.h"
#include "FMECommon.h"

using namespace FME;

FMEExprParser::FMEExprParser(FMEFaceTracker* faceTracker):
	m_faceTracker(faceTracker),
	m_calibrated(false),
	m_calibrating(false),
	m_lastFaceAngles(FaceAngles::Zero()),
	m_maxQueueLength(CALIBRATION_QUEUE_LENGTH),
	m_lastFace(NULL),
	m_faceNum(0)
{
}

FMEExprParser::~FMEExprParser()
{
	CleanQueue();
}

void FMEExprParser::StartCalibration(int frameNum)
{
	assert(frameNum>0);
	if(frameNum <= 0) return;
	CleanQueue();
	m_maxQueueLength = frameNum;
	m_faceQueue.resize(CALIBRATION_QUEUE_LENGTH,NULL);
	m_faceNum = 0;
	m_calibrating = true;
	m_calibrated = false;
	m_lastFace = NULL;
	m_neutralFAPUs.setZero();
	m_neutralAUStates.clear();
}

void FMEExprParser::Tick(double fTime, float fElapsedTime)
{
	RawFaceData rawFaceData;
	m_faceTracker->QueryRawFace(rawFaceData);

	if(!m_calibrated) 
	{
		if(!m_calibrating) StartCalibration(CALIBRATION_QUEUE_LENGTH);

		if (!Validate(&rawFaceData)){
			QueueNewFace(NULL);
		}
		else{
			FMENeutralFace *face = new FMENeutralFace(&rawFaceData, this); 
			face->MeasureFAPUs();
			face->MeasureAUStates();
			QueueNewFace(face);
		}

		if(m_faceNum >= CALIBRATION_QUEUE_LENGTH) StopCalibration();
	}
	else
	{	
		if (!Validate(&rawFaceData)){
			QueueNewFace(NULL);
		}
		else{
			FMEFace *face = new FMEFace(&rawFaceData, this);
			face->MeasureAUStates();	
			face->MeasureFaceAngle();
			face->MeasureActionUnitWeightMap(); 
			QueueNewFace(face);
		}

		if(m_lastFace == NULL) return;

		OptimizeActionUnitWeights(); 
		OptimizeFaceAngles(); 

		SmoothActionUnitWeights();	
		SmoothFaceAngles();

		m_lastActionUnitWeights = m_newActionUnitWeights;
		m_lastFaceAngles = m_newFaceAngles;
	}
}


void FMEExprParser::StopCalibration()
{
	ExtractNeutralFAPUs();
	ExtractNeutralAUStates();
	m_calibrated = true;
	m_calibrating = false;
	assert(SMOOTH_QUEUE_LENGTH>=7);
	m_maxQueueLength = SMOOTH_QUEUE_LENGTH;

	CleanQueue();
	m_faceQueue.resize(SMOOTH_QUEUE_LENGTH,NULL);
	m_faceNum = 0;
	InitActionUnitWeightMap(m_lastActionUnitWeights);
	InitActionUnitWeightMap(m_newActionUnitWeights);
	InitActionUnitWeightMap(m_currentActionUnitWeights);
	m_lastFaceAngles.setZero();
	m_currentFaceAngles.setZero();
	m_newFaceAngles.setZero();
	m_expressionConfidence = -1;
}


void FMEExprParser::ExtractNeutralFAPUs()
{
	if(m_faceNum<=0) return;

	FAPUs sum;
	sum.setZero();

	for(FaceQueue::iterator it_face = m_faceQueue.begin(); it_face != m_faceQueue.end(); it_face++ )
	{
		if( *it_face == NULL) continue;
		FMENeutralFace* nf = static_cast<FMENeutralFace *>(*it_face);
		sum += *nf->GetFAPUs();
	}
	
	m_neutralFAPUs = sum/m_faceNum;
}


void FMEExprParser::ExtractNeutralAUStates()
{
	if(m_faceNum<=0) return;

	InitActionUnitWeightMap(m_neutralAUStates);
	for(FaceQueue::reverse_iterator it_face = m_faceQueue.rbegin(); it_face != m_faceQueue.rend(); it_face++ )
	{
		if( *it_face == NULL) continue;

		AUStates* AUStates = (*it_face)->GetAUStates();
		for(AUStates::iterator it_AUStates = AUStates->begin(); it_AUStates != AUStates->end(); it_AUStates++)
		{
			m_neutralAUStates[it_AUStates->first] += it_AUStates->second/m_faceNum;
		}
	}
}

void FMEExprParser::QueueNewFace(FMEFace *face)
{
	m_faceQueue.push_back(face);
	
	if(face != NULL){
		m_faceNum++;
		m_lastFace = face;
	}

	while(m_faceQueue.size() > m_maxQueueLength)
	{
		FMEFace *frontface = m_faceQueue.front();
		m_faceQueue.pop_front();
		if(frontface == m_lastFace) m_lastFace = NULL;
		if(frontface != NULL){ 
			m_faceNum--;
			delete frontface;
		}
	};
}

void FMEExprParser::CleanQueue()
{
	while(!m_faceQueue.empty())
	{
		FMEFace *frontface = m_faceQueue.front();
		m_faceQueue.pop_front();
		if(frontface) delete frontface;
	}
	m_faceNum = 0;
	m_lastFace = NULL;
}


bool FMEExprParser::Validate(const RawFaceData *rawFaceData)
{
	static long frameNum=0;
	frameNum++;

	if(!rawFaceData->hasLandmarks || !rawFaceData->hasPoseAngles) {
		return false;
	}

	for(int i=0; i<rawFaceData->landmarks.size(); i++)
	{
		if ( i<53 || i>69 ) 
		{
			if( (rawFaceData->landmarks)[i].confidenceWorld < 100 ) {
				return false;
			}
		}
	}
	
	return true;
}

AUStates* FMEExprParser::GetNeutralAUStates()
{
	return ( m_calibrated )? &m_neutralAUStates : NULL;
}

FAPUs* FMEExprParser::GetNeutralFAPUs()
{
	return ( m_calibrated )? &m_neutralFAPUs : NULL; 
}


float* FMEExprParser::GetExpressionConfidence()
{
	if(!m_calibrated) return NULL;
	return &m_expressionConfidence;
}

ActionUnitWeightMap*	FMEExprParser::GetActionUnitWeights()
{
	if(!m_calibrated) return NULL;	
	return &m_lastActionUnitWeights;
}


void FMEExprParser::OptimizeFaceAngles()
{
	m_currentFaceAngles = *m_lastFace->GetRawFaceAngles();
	m_newFaceAngles.x() = Clampf(m_currentFaceAngles.x(),-PITCH_THRESHOLD,PITCH_THRESHOLD);
	m_newFaceAngles.y() = Clampf(m_currentFaceAngles.y(),-YAW_THRESHOLD,YAW_THRESHOLD);
	m_newFaceAngles.z() = Clampf(m_currentFaceAngles.z(),-ROLL_THRESHOLD,ROLL_THRESHOLD);	
}


void FMEExprParser::OptimizeActionUnitWeights()
{
	m_currentActionUnitWeights = *m_lastFace->GetActionUnitWeightMap();
	m_newActionUnitWeights = m_currentActionUnitWeights;

	m_newActionUnitWeights[MOUTH_RIGHT_RAISE] = Clampf(m_currentActionUnitWeights[MOUTH_RIGHT_RAISE] - m_currentActionUnitWeights[MOUTH_LEFT], 0, 1);
	m_newActionUnitWeights[MOUTH_LEFT_RAISE] = Clampf(m_currentActionUnitWeights[MOUTH_LEFT_RAISE] - m_currentActionUnitWeights[MOUTH_RIGHT], 0, 1);
}


FaceAngles* FMEExprParser::GetFaceAngles()
{
	if(!m_calibrated) return NULL;  
	return 	&m_lastFaceAngles;
}


void FMEExprParser::InitActionUnitWeightMap(ActionUnitWeightMap& faws)
{
	faws[EYEBROW_Up_L] = 0;
	faws[EYEBROW_Up_R] = 0;
	faws[EYEBROW_DOWN_L] = 0;
	faws[EYEBROW_DOWN_R] = 0;

	faws[EYELID_CLOSE_L] = 0;
	faws[EYELID_CLOSE_R] = 0;
	faws[EYELID_OPEN_L] = 0;
	faws[EYELID_OPEN_R] = 0;

	faws[EYEBALL_TURN_L] = 0;
	faws[EYEBALL_TURN_R] = 0;
	faws[EYEBALL_TURN_U] = 0;
	faws[EYEBALL_TRUN_D] = 0;

	faws[MOUTH_OPEN] =0;
	faws[MOUTH_LEFT_RAISE] =0;
	faws[MOUTH_RIGHT_RAISE] =0;
	faws[MOUTH_LEFT] = 0;
	faws[MOUTH_RIGHT] = 0;
}


void FMEExprParser::SmoothActionUnitWeights()
{ 	
	EvaluteStates();
	for(ActionUnitWeightMap::iterator it_lastFaws = m_lastActionUnitWeights.begin(); it_lastFaws != m_lastActionUnitWeights.end(); it_lastFaws++)
	{
		ActionUnit fa = it_lastFaws->first;
		m_newActionUnitWeights[fa] = m_lastActionUnitWeights[fa]*(1-m_expressionConfidence) + m_newActionUnitWeights[fa]*m_expressionConfidence;
	}
}

void FMEExprParser::SmoothFaceAngles()
{
	if(m_lastFace == NULL) return;
	
	float weight = 0;
	FaceAngles delta;
	delta = m_newFaceAngles - m_lastFaceAngles;
	float smoothCoefficient = 5.f; 
	float w_x = Clampf(abs(delta.x()/smoothCoefficient),0,1); 
	float w_y = Clampf(abs(delta.y()/smoothCoefficient),0,1);
	float w_z = Clampf(abs(delta.z()/smoothCoefficient),0,1);
	weight = ( w_x*w_x + w_y*w_y + w_z*w_z )/3.0;
	m_newFaceAngles = m_lastFaceAngles*(1-weight) + m_newFaceAngles*weight;
}


void FMEExprParser::EvaluteStates()
{
	//LostTracking
	float lostTrackingRate = 1 - (float)m_faceNum/(float)m_faceQueue.size();

	//PoseShifting
	FaceAngles rawAngles = *m_lastFace->GetRawFaceAngles();
	float wx = Clampf(abs(rawAngles.x())/PITCH_CONFIDENCE,0,1);
	float wy = Clampf(abs(rawAngles.y())/YAW_CONFIDENCE,0,1);
	float wz = Clampf(abs(rawAngles.z())/ROLL_CONFIDENCE,0,1);
	float poseShiftingRate = std::max<float>(std::max<float>(wx, wy),wz);

	float conf = std::max<float>(lostTrackingRate,poseShiftingRate);
	conf = pow(conf,2);
	m_expressionConfidence = 1 - conf;
}

