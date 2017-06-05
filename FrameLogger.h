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