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
#include "FMECommon.h"

using namespace FME;

float FME::CalWeight(float delta, float threshold)
{
	float weight = 0;
	if ((delta > 0) && (threshold > 0))
	{
		weight = Clampf(delta / threshold, 0.f, 1.f);
	}
	return weight;
}

float FME::CalWeight(float delta, float negR, float posR)
{
		float weight = 0;
		if((delta < 0) && (negR<0))
		{
			weight = Clampf(-delta/negR, -1.f, 0.f);
		}
		else if( (delta > 0) && (posR > 0 ))
		{
			weight = Clampf( delta/posR, 0.f, 1.f);
		}
		return weight;
}

inline int FME::Clamp(int value, int low, int high)
{
	//return std::min<int>(std::max<int>(value,low), high);
	return min(max(value, low), high);
};

inline float FME::Clampf(float value, float low, float high)
{
	//return std::min<float>(std::max<float>(value,low), high);
	return min(max(value, low), high);
};


void FME::LogStatus(int level, std::wstring info)
{
	if(!g_logFun)g_logFun(level,info);
};

std::wstring* FME::g_deviceName;
std::wstring* FME::g_fileName;
bool	 FME::g_isRecord;
CallbackWriteLog	FME::g_logFun;
