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
#include "FMEFaceTracker.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <deque>
#include <iostream>
#include <map>

namespace FME{

class FMEExprParser;

typedef float	AUWeight;
typedef float	AUState;
typedef std::map<ActionUnit,AUState> AUStates;

typedef Eigen::Vector3f		Point3D;
typedef Eigen::Vector2f		Point2D;
typedef std::vector<Point2D>	Point2Ds;
typedef std::vector<Point3D>	Point3Ds;
typedef Eigen::Vector3f		FaceAngles;
typedef Eigen::Array<float,6,1> FAPUs;

typedef enum FapuName
{
	IRISD_L,
	IRISD_R,
	ES,
	ENS,
	MNS,
	MW,
} FapuName;

class FMEFace
{
public:
	FMEFace(RawFaceData *rawFaceData, FMEExprParser *exprParser);
	~FMEFace();

	void	MeasureAUStates();	
	void	MeasureActionUnitWeightMap();
	void	MeasureFaceAngle();

	AUStates*	GetAUStates(){return &m_auStates;};
	ActionUnitWeightMap*	GetActionUnitWeightMap(){return &m_ActionUnitWeightMap;};
	FaceAngles*	GetRawFaceAngles(){return &m_rawFaceAngles;};

	float	GetAUWeight(ActionUnit fa){return m_ActionUnitWeightMap[fa];}

protected:
	inline Point3D		Landmark(int index);	
	inline float		LandmarkDistance(int i, int j);
	inline Point2D		LandmarkImage(int index);
	inline float		LandmarkImageDistance(int i, int j);

	void	EyebrowStates();
	void	EyelidStates();
	void	EyeballStates();
	void	MouthStates();

	void	EyebrowWeights();
	void	EyelidWeights();
	void	EyeballWeights();
	void	MouthWeights();

protected:
	RawFaceData*	m_rawFaceData;		
	AUStates		m_auStates;	
	ActionUnitWeightMap		m_ActionUnitWeightMap;
	FMEExprParser*	m_exprParser;
	FaceAngles		m_rawFaceAngles;
	FAPUs*			m_neutralFAPUs;
	AUStates*		m_neutralAUStates;
};

class FMENeutralFace: public FMEFace
{
public:
	FMENeutralFace(RawFaceData *rawFaceData,FMEExprParser *exprParser);
	~FMENeutralFace();
	void	MeasureFAPUs();
	FAPUs*	GetFAPUs(){ return &m_rawFAPUs;};

protected:
	FAPUs	m_rawFAPUs;
};

} //namespace