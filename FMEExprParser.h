/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "FMECommon.h"
#include "FMEFace.h"

namespace FME{

typedef std::deque<FMEFace*> FaceQueue;

class FMEExprParser
{
public:
	FMEExprParser(FMEFaceTracker* faceTracker);
	~FMEExprParser();
	void	StartCalibration(int frames);
	void	StopCalibration();
	void	Tick(double fTime, float fElapsedTime);

	ActionUnitWeightMap*	GetActionUnitWeights();
	FaceAngles*	GetFaceAngles();
	float*	GetExpressionConfidence();

	AUStates*	GetNeutralAUStates();
	FAPUs*		GetNeutralFAPUs();

	const FaceQueue&	GetFaceQueue(){return m_faceQueue;};

protected:
	bool	Validate(const RawFaceData *rawFaceData);

	void	CleanQueue();
	void	QueueNewFace(FMEFace *face);

	void	ExtractNeutralFAPUs();
	void	ExtractNeutralAUStates();

	void	InitActionUnitWeightMap(ActionUnitWeightMap& faws);

	void	EvaluteStates(); 

	void	OptimizeFaceAngles();
	void	OptimizeActionUnitWeights(); 

	void	SmoothActionUnitWeights(); 
	void	SmoothFaceAngles();

protected:
	FaceQueue	m_faceQueue;
	int			m_faceNum;
	int			m_maxQueueLength;
	FMEFace*	m_lastFace;

	bool		m_calibrated;
	bool		m_calibrating;

	FAPUs			m_neutralFAPUs;	
	AUStates		m_neutralAUStates; 

	ActionUnitWeightMap		m_currentActionUnitWeights; 
	FaceAngles		m_currentFaceAngles;

	ActionUnitWeightMap		m_newActionUnitWeights;	
	FaceAngles		m_newFaceAngles;

	ActionUnitWeightMap		m_lastActionUnitWeights;  
	FaceAngles		m_lastFaceAngles;

	FMEFaceTracker* m_faceTracker;

	float	m_expressionConfidence;
};

} //namespace