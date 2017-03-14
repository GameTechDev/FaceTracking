/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or non-disclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2014 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include "IFaceME.h"
#include "FMEFace.h"

class FrameLogger
{
public:
	FrameLogger();
	~FrameLogger();
	void BeginFrame();
	void EndFrame();
	void LogFrameFacialActionWeightMap(FacialActionWeightMap& faws);
	void LogFrameFaceAngle(FaceAngles& angle);
protected:
	long		m_frameNum;
	//IFMELogger* m_logger;
	char		m_s[256];
};

//MOUTH_OPEN输出raw data。
//EYELID经过intra，inter，blend三重过滤。
//其他weight进过gauss过滤。