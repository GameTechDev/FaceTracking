//--------------------------------------------------------------------------------------
// Copyright 2016 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------
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