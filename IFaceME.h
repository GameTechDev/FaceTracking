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