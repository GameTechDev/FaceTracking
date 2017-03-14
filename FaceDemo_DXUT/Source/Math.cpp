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
#include "Math.h"

SRTTransform::SRTTransform()
	: mScale(1.0f, 1.0f, 1.0f)
	, mRotation(0.0f, 0.0f, 0.0f, 0.0f)
	, mTranslation(0.0f, 0.0f, 0.0f)
{

}


SRTTransform::SRTTransform(const D3DXMATRIXA16& matrix)
{
	fromMatrix(matrix);
}


SRTTransform::SRTTransform(const SRTTransform& rhs)
{
	mScale = rhs.mScale;
	mRotation = rhs.mRotation;
	mTranslation = rhs.mTranslation;
}


SRTTransform::~SRTTransform()
{

}

