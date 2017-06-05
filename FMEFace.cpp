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

using namespace FME;

FMEFace::FMEFace(RawFaceData *rawFaceData, FMEExprParser *exprParser)
	:m_exprParser(exprParser),
	m_rawFaceData(rawFaceData)
{
}

FMEFace::~FMEFace()
{
}

void FMEFace::MeasureAUStates()
{
	EyebrowStates();
	EyelidStates();
	EyeballStates();
	MouthStates();
}

void FMEFace::MeasureActionUnitWeightMap()
{
	m_neutralAUStates = m_exprParser->GetNeutralAUStates();
	m_neutralFAPUs = m_exprParser->GetNeutralFAPUs();

	if( m_neutralAUStates && m_neutralFAPUs){
		EyebrowWeights();
		EyelidWeights();
		EyeballWeights();
		MouthWeights();
	}
}

void FMEFace::MeasureFaceAngle()
{
	m_rawFaceAngles << m_rawFaceData->poseAngles.pitch,		//x roataion
					 m_rawFaceData->poseAngles.yaw,		//y rotation
					 m_rawFaceData->poseAngles.roll;	//z rotation
}

inline Point3D FMEFace::Landmark(int index)
{
	PXCPoint3DF32 world = (m_rawFaceData->landmarks)[index].world;  
	return Point3D(world.x,world.y,world.z) * 1000; 
}


inline float FMEFace::LandmarkDistance(int i, int j)
{
	return ( Landmark(j) - Landmark(i) ).norm();
}

inline Point2D FMEFace::LandmarkImage(int index)
{
	PXCPointF32 image = (m_rawFaceData->landmarks)[index].image;  
	return Point2D(image.x,image.y);
}

inline float FMEFace::LandmarkImageDistance(int i, int j)
{

	return ( LandmarkImage(j) - LandmarkImage(i) ).norm();
}


void FMEFace::EyebrowStates()
{
	{//EYEBROW_L,		
		m_auStates[EYEBROW_DOWN_L] = m_auStates[EYEBROW_Up_L] = (LandmarkDistance(5,18)+LandmarkDistance(6,25)+LandmarkDistance(7,24))/3;
	}

	{//EYEBROW_R,
		m_auStates[EYEBROW_DOWN_R] = m_auStates[EYEBROW_Up_R] = (LandmarkDistance(0,10)+LandmarkDistance(1,17)+LandmarkDistance(2,16))/3;
	}
}

void FMEFace::EyelidStates()
{
	{//EyeLid_L,
		m_auStates[EYELID_OPEN_L] = m_auStates[EYELID_CLOSE_L] = (LandmarkDistance(20,24)+LandmarkDistance(19,25)+LandmarkDistance(21,23))/3;
	}

	{//EyeLid_R,
		m_auStates[EYELID_OPEN_R] = m_auStates[EYELID_CLOSE_R] = (LandmarkDistance(12,16)+LandmarkDistance(11,17)+LandmarkDistance(13,15))/3;
	}
}

void FMEFace::EyeballStates()
{
	{
		float leftEye = LandmarkDistance(22,77)/LandmarkDistance(18,22);	
		float rightEye = LandmarkDistance(10,76)/LandmarkDistance(10,14);	
		m_auStates[EYEBALL_TURN_R] = m_auStates[EYEBALL_TURN_L] = (leftEye+rightEye)/2;
	}

	{
		float leftEye = LandmarkDistance(24,77)/LandmarkDistance(24,20);	
		float rightEye = LandmarkDistance(16,76)/LandmarkDistance(16,12);	
		m_auStates[EYEBALL_TRUN_D] = m_auStates[EYEBALL_TURN_U] = (leftEye+rightEye)/2;
	}
}

void FMEFace::MouthStates()
{
	{//MOUTH_OPEN,
		m_auStates[MOUTH_OPEN] = (LandmarkDistance(46,52)+LandmarkDistance(47,51)+LandmarkDistance(48,50))/3;
	}

	{//MOUTH_RIGHT_RAISE	
		m_auStates[MOUTH_RIGHT_RAISE] = - (LandmarkDistance(14,33) - LandmarkDistance(33,31)); 
	}

	{//MOUTH_LEFT_RAISE,
		m_auStates[MOUTH_LEFT_RAISE] = - (LandmarkDistance(22,39) - LandmarkDistance(39,31));
	}

	{//MOUTH_LEFT,	
		m_auStates[MOUTH_LEFT] = LandmarkDistance(39,22) - LandmarkDistance(39,31);
	}

	{//MOUTH_RIGHT,
		m_auStates[MOUTH_RIGHT] = LandmarkDistance(33,14) - LandmarkDistance(33,31);
	}

}


void FMEFace::EyebrowWeights()
{
	float ENS0 = (*m_neutralFAPUs)[ENS];
	float ES0 = (*m_neutralFAPUs)[ES];

	{//EYEBROW_Up_L,
		float currentValue = m_auStates[EYEBROW_Up_L];
		float neutralValue = (*m_neutralAUStates)[EYEBROW_Up_L];

		float delta = (currentValue - neutralValue)/ENS0;
		if (delta >= 0)
			m_ActionUnitWeightMap[EYEBROW_Up_L] = CalWeight(delta, 1024 * 0.2);
		else
			m_ActionUnitWeightMap[EYEBROW_Up_L] = 0;

	}

	{//EYEBROW_Up_R,
		float currentValue = m_auStates[EYEBROW_Up_R];
		float neutralValue = (*m_neutralAUStates)[EYEBROW_Up_R];

		float delta = (currentValue - neutralValue)/ENS0;
		if (delta >= 0)
			m_ActionUnitWeightMap[EYEBROW_Up_R] = CalWeight(delta, 1024 * 0.2);
		else
			m_ActionUnitWeightMap[EYEBROW_Up_R] = 0;

	}
	
	{//EYEBROW_DOWN_L,
		float currentValue = m_auStates[EYEBROW_DOWN_L];
		float neutralValue = (*m_neutralAUStates)[EYEBROW_DOWN_L];

		float delta = (currentValue - neutralValue)/ES0;
		if (delta > 0)
			m_ActionUnitWeightMap[EYEBROW_DOWN_L] = 0;
		else
			m_ActionUnitWeightMap[EYEBROW_DOWN_L] = CalWeight(-delta, 1024 * 0.1);
	}

	{//EYEBROW_DOWN_R,
		float currentValue = m_auStates[EYEBROW_DOWN_R];
		float neutralValue = (*m_neutralAUStates)[EYEBROW_DOWN_R];

		float delta = (currentValue - neutralValue)/ES0;
		if (delta > 0)
			m_ActionUnitWeightMap[EYEBROW_DOWN_R] = 0;
		else
			m_ActionUnitWeightMap[EYEBROW_DOWN_R] = CalWeight(-delta, 1024 * 0.1);
	}
}

void FMEFace::EyelidWeights()
{
	float IRISD_L0 = (*m_neutralFAPUs)[IRISD_L];
	float IRISD_R0 = (*m_neutralFAPUs)[IRISD_R];

	{//EYELID_CLOSE_L,
		float currentValue = m_auStates[EYELID_CLOSE_L];
		float neutralValue = (*m_neutralAUStates)[EYELID_CLOSE_L];

		float delta = (currentValue - neutralValue)/IRISD_L0;
		if(delta>0)
			m_ActionUnitWeightMap[EYELID_CLOSE_L] = 0;
		else
			m_ActionUnitWeightMap[EYELID_CLOSE_L] = CalWeight(-delta, 1024 * 0.4);

	}

	{//EYELID_CLOSE_R,
		float currentValue = m_auStates[EYELID_CLOSE_R];
		float neutralValue = (*m_neutralAUStates)[EYELID_CLOSE_R];

		float delta = (currentValue - neutralValue)/IRISD_R0; 
		if (delta>0)
			m_ActionUnitWeightMap[EYELID_CLOSE_R] = 0;
		else
			m_ActionUnitWeightMap[EYELID_CLOSE_R] = CalWeight(-delta, 1024 * 0.4);
	}

	{//EYELID_OPEN_L,
		float currentValue = m_auStates[EYELID_OPEN_L];
		float neutralValue = (*m_neutralAUStates)[EYELID_OPEN_L];

		float delta = (currentValue - neutralValue) / IRISD_L0;
		if (delta>0)
			m_ActionUnitWeightMap[EYELID_OPEN_L] = CalWeight(delta, 1024 * 0.6);
		else
			m_ActionUnitWeightMap[EYELID_OPEN_L] = 0;
	}

	{//EYELID_OPEN_R,
		float currentValue = m_auStates[EYELID_OPEN_R];
		float neutralValue = (*m_neutralAUStates)[EYELID_OPEN_R];

		float delta = (currentValue - neutralValue) / IRISD_L0;
		if (delta>0)
			m_ActionUnitWeightMap[EYELID_OPEN_R] = CalWeight(delta, 1024 * 0.6);
		else
			m_ActionUnitWeightMap[EYELID_OPEN_R] = 0;
	}
}

void FMEFace::EyeballWeights()
{
	{//EYEBALL_TURN_L,
		float currentValue = m_auStates[EYEBALL_TURN_L];
		float neutralValue = (*m_neutralAUStates)[EYEBALL_TURN_L];

		float delta = (currentValue - neutralValue);
		if (delta>0)
			m_ActionUnitWeightMap[EYEBALL_TURN_L] = CalWeight(delta, 0.5);
		else
			m_ActionUnitWeightMap[EYEBALL_TURN_L] = 0;
	}

	{//EYEBALL_TURN_R,
		float currentValue = m_auStates[EYEBALL_TURN_R];
		float neutralValue = (*m_neutralAUStates)[EYEBALL_TURN_R];

		float delta = (currentValue - neutralValue);
		if (delta>0)
			m_ActionUnitWeightMap[EYEBALL_TURN_R] = 0;
		else
			m_ActionUnitWeightMap[EYEBALL_TURN_R] = CalWeight(-delta, 0.5);
	}

	{//EYEBALL_TURN_U,
		float currentValue = m_auStates[EYEBALL_TURN_U];
		float neutralValue = (*m_neutralAUStates)[EYEBALL_TURN_U];

		float delta = (currentValue - neutralValue);
		if (delta>0)
			m_ActionUnitWeightMap[EYEBALL_TURN_R] = CalWeight(delta, 0.5);
		else
			m_ActionUnitWeightMap[EYEBALL_TURN_R] = 0;
	}

	{//EYEBALL_TRUN_D,
		float currentValue = m_auStates[EYEBALL_TRUN_D];
		float neutralValue = (*m_neutralAUStates)[EYEBALL_TRUN_D];

		float delta = (currentValue - neutralValue);
		if (delta>0)
			m_ActionUnitWeightMap[EYEBALL_TRUN_D] = 0;
		else
			m_ActionUnitWeightMap[EYEBALL_TRUN_D] = CalWeight(-delta, 0.5);
	}

}

void FMEFace::MouthWeights()
{
	float MNS0 = (*m_neutralFAPUs)[MNS];
	float MW0 = (*m_neutralFAPUs)[MW];

	{//MOUTH_OPEN,
		float currentValue = m_auStates[MOUTH_OPEN];
		float neutralValue = (*m_neutralAUStates)[MOUTH_OPEN];

		float delta = currentValue/MNS0; 
		if (delta > 0)
			m_ActionUnitWeightMap[MOUTH_OPEN] = CalWeight(delta, 1024 * 1.2);
		else
			m_ActionUnitWeightMap[MOUTH_OPEN] = 0;
	}

	{//MOUTH_LEFT_RAISE,	
		float currentValue = m_auStates[MOUTH_LEFT_RAISE];
		float neutralValue = (*m_neutralAUStates)[MOUTH_LEFT_RAISE];

		float delta = (currentValue - neutralValue)/MNS0;
		if (delta > 0)
			m_ActionUnitWeightMap[MOUTH_LEFT_RAISE] = CalWeight(delta, 1024 * 0.4);
		else
			m_ActionUnitWeightMap[MOUTH_LEFT_RAISE] = 0;
	}

	{//MOUTH_RIGHT_RAISE,
		float currentValue = m_auStates[MOUTH_RIGHT_RAISE];
		float neutralValue = (*m_neutralAUStates)[MOUTH_RIGHT_RAISE];

		float delta = (currentValue - neutralValue)/MNS0;
		if (delta > 0)
			m_ActionUnitWeightMap[MOUTH_RIGHT_RAISE] = CalWeight(delta, 1024 * 0.4);
		else
			m_ActionUnitWeightMap[MOUTH_RIGHT_RAISE] = 0;
	}

	{//MOUTH_LEFT,	
		float currentValue = m_auStates[MOUTH_LEFT];
		float neutralValue = (*m_neutralAUStates)[MOUTH_LEFT];

		float delta = (currentValue - neutralValue)/MW0;
		if (delta > 0)
			m_ActionUnitWeightMap[MOUTH_LEFT] = CalWeight(delta, 1024 * 0.2);
		else
			m_ActionUnitWeightMap[MOUTH_LEFT] = 0;
	}

	{//MOUTH_RIGHT,
		float currentValue = m_auStates[MOUTH_RIGHT];
		float neutralValue = (*m_neutralAUStates)[MOUTH_RIGHT];

		float delta = (currentValue - neutralValue)/MW0;
		if (delta > 0)
			m_ActionUnitWeightMap[MOUTH_RIGHT] = CalWeight(delta, 1024 * 0.2);
		else
			m_ActionUnitWeightMap[MOUTH_RIGHT] = 0;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//							RSNeutralFace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FMENeutralFace::FMENeutralFace(RawFaceData *rawFaceData,FMEExprParser *exprParser)
	:FMEFace(rawFaceData, exprParser)
{
}

FMENeutralFace::~FMENeutralFace()
{
}

void FMENeutralFace::MeasureFAPUs()
{
	m_rawFAPUs[IRISD_L] = LandmarkDistance(20,24) / 1024;
	m_rawFAPUs[IRISD_R] = LandmarkDistance(12,16) / 1024; 
	m_rawFAPUs[ES] = LandmarkDistance(77,76) / 1024;
	m_rawFAPUs[ENS] = LandmarkDistance(26,31) / 1024;
	m_rawFAPUs[MNS] = LandmarkDistance(47,31) / 1024;
	m_rawFAPUs[MW] = LandmarkDistance(39,33) / 1024;
}
