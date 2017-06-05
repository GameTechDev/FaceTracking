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

#pragma once
#include "pxcfacedata.h"
#include <vector>
#include <math.h> 
#include "IFaceME.h"

namespace FME{

#define NUM_LANDMARK 78
#define SMOOTH_QUEUE_LENGTH 7
#define CALIBRATION_QUEUE_LENGTH 20

#define PITCH_THRESHOLD	15.f
#define YAW_THRESHOLD	20.f
#define ROLL_THRESHOLD	25.f

#define PITCH_CONFIDENCE 10.f //5.f
#define YAW_CONFIDENCE	 15.f //10.f
#define ROLL_CONFIDENCE	 20.f //15.f

typedef std::vector<PXCFaceData::LandmarkPoint> PXCLandmarkArray;

typedef struct RawFaceData
{
	PXCLandmarkArray	landmarks;
	PXCFaceData::PoseEulerAngles poseAngles;
	bool	hasLandmarks;
	bool	hasPoseAngles;
	RawFaceData():hasLandmarks(false),hasPoseAngles(false){};
} RawFaceData;

inline int Clamp(int value, int low, int high);
inline float Clampf(float value, float low, float high);
float CalWeight(float delta, float threshold);
float CalWeight(float delta, float negR, float posR);
void LogStatus(int level, std::wstring info);

extern "C" std::wstring* g_deviceName;
extern "C" std::wstring* g_fileName;
extern "C" bool		g_isRecord;
extern "C" CallbackWriteLog	g_logFun;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif
}