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