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
