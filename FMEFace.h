/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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