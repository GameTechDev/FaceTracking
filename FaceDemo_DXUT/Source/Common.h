/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Config.h"
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTcamera.h"
#include "SDKmisc.h"

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

/// Macro, check HRESULT and return false if failed
//#define _VERY_RET(r) \
//	if (FAILED(r)) \
//	{ \
//		DXUTTrace(__FILE__, (DWORD)__LINE__, r, L"D3D Call Validation Failed", true);\
//		return false; \
//	}
#define _VERY_RET(r) \
	if (FAILED(r)) \
	{ \
		return false; \
	}
