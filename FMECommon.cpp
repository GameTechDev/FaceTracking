/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
