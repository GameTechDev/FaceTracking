/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>
#include <vector>
#include <map>

namespace FME{

typedef struct FaceOrientation
{		
	float yaw;
	float pitch;
	float roll;
} FaceOrientation;

typedef struct Landmark
{
	int confidenceImage;
	struct{
		float x, y;
	} image;
} Landmark;

typedef std::vector<Landmark> LandmarkArray;

typedef struct BGRAImage
{
	int width;
	int height;
	std::vector<unsigned char> buffer;
} BGRAImage;

typedef enum ActionUnit{
	EYEBROW_DOWN_L=0,
	EYEBROW_DOWN_R,
	EYEBROW_Up_L,
	EYEBROW_Up_R,

	EYELID_CLOSE_L,
	EYELID_CLOSE_R,
	EYELID_OPEN_L,
	EYELID_OPEN_R,

	EYEBALL_TURN_L,
	EYEBALL_TURN_R,
	EYEBALL_TURN_U,
	EYEBALL_TRUN_D,
	
	MOUTH_OPEN,
	MOUTH_LEFT_RAISE,
	MOUTH_RIGHT_RAISE,
	MOUTH_LEFT,
	MOUTH_RIGHT,
	ACTION_NUM
} ActionUnit;

typedef std::map<ActionUnit,float> ActionUnitWeightMap;

typedef struct FaceBoneTransform
{
	float scaleVector[3]; //(x,y,z)
	float rotationQuaternion[4]; //(x,y,z,w)
	float translationVector[3]; //(x,y,z)
} FaceBoneTransform;

typedef std::map<std::string, FaceBoneTransform> FaceBoneTransformMap;

class IFaceBoneModel
{
public:
	virtual bool GetCurrentActionBoneTransforms(ActionUnit inActionName, float inWeight, FaceBoneTransformMap& outCurrentActionBones)=0;
	virtual bool GetNeutralFaceBoneTransforms(FaceBoneTransformMap& outNeutralFaceBones)=0;
	virtual bool SetCombinedFaceBoneTransforms(FaceBoneTransformMap& inCombinedFaceBones)=0;
};

typedef void (*CallbackWriteLog)(int level, std::wstring& info);

typedef struct EMULATOR_CONFIG
{
	std::wstring* deviceName; 
	std::wstring* fileName;
	bool isRecord;
	CallbackWriteLog logFun;
} EMULATOR_CONFIG;

class IFaceMotionEmulator
{
public:
	virtual bool QueryDevices(std::vector<std::wstring>& deviceNames)=0;
	virtual bool Init(EMULATOR_CONFIG config)=0;
	virtual void Reset()=0;
	virtual void Tick(double fTime, float fElapsedTime)=0;
	virtual void Release()=0;

	virtual bool QueryLandmarks(LandmarkArray& landmarks)=0;
	virtual bool QueryBGRAImage(BGRAImage& image)=0;
	virtual bool QueryActionUnitWeights(ActionUnitWeightMap& weights)=0;
	virtual bool QueryFaceOrientation(FaceOrientation &orientation)=0;	
	virtual bool QueryExpressionConficence(float& confidence)=0;

	virtual bool QueryFaceBoneTransforms(IFaceBoneModel* model)=0; //update bone matrix of the model
	virtual void RegisterModels(std::vector<IFaceBoneModel*>* models)=0; 
};

extern "C" IFaceMotionEmulator* FMECreate(); 

}//namespace