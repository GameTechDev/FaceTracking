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
#include "Common.h"
#include "..\..\IFaceME.h"

/// Bounding sphere of model
struct BoundingSphere
{
	D3DXVECTOR3 mCenter;
	float mRadius;
	BoundingSphere() : mCenter(0.0f, 0.0f, 0.0f), mRadius(0.0f) {}
};


/// Scale/Rotation/Translation transform class
class SRTTransform
{
public:
	D3DXVECTOR3 mScale;
	D3DXQUATERNION mRotation;
	D3DXVECTOR3 mTranslation;

	/// Constructor
	SRTTransform();
	/// Constructor
	SRTTransform(const D3DXMATRIXA16& matrix);
	SRTTransform(const FME::FaceBoneTransform& trans);
	/// Destructor
	~SRTTransform();
	/// Copy constructor
	SRTTransform(const SRTTransform& trans);
	/// Convert matrix to scale, rotation and translation
	void fromMatrix(const D3DXMATRIXA16& matrix);
	/// Convert scale, rotation and translation to matrix
	void toMatrix(D3DXMATRIXA16& matrix) const;
	FME::FaceBoneTransform ToFaceBoneTransform() const;

};


inline
void SRTTransform::fromMatrix(const D3DXMATRIXA16& matrix)
{
	D3DXMatrixDecompose(&mScale, &mRotation, &mTranslation, &matrix);
}


inline
void SRTTransform::toMatrix(D3DXMATRIXA16& matrix) const
{
	D3DXMATRIX transMat, rotMat;
	D3DXMatrixScaling(&matrix, mScale.x, mScale.y, mScale.z);
	D3DXMatrixRotationQuaternion(&rotMat, &mRotation);
	D3DXMatrixTranslation(&transMat, mTranslation.x, mTranslation.y, mTranslation.z);

	D3DXMatrixMultiply(&matrix, &matrix, &rotMat);
	D3DXMatrixMultiply(&matrix, &matrix, &transMat);
}

inline
FME::FaceBoneTransform SRTTransform::ToFaceBoneTransform() const
{
	FME::FaceBoneTransform srt;
	srt.scaleVector[0] = mScale.x;
	srt.scaleVector[1] = mScale.y;
	srt.scaleVector[2] = mScale.z;

	srt.rotationQuaternion[0] = mRotation.x;
	srt.rotationQuaternion[1] = mRotation.y;
	srt.rotationQuaternion[2] = mRotation.z;
	srt.rotationQuaternion[3] = mRotation.w;

	srt.translationVector[0] = mTranslation.x;
	srt.translationVector[1] = mTranslation.y;
	srt.translationVector[2] = mTranslation.z;
	return srt;
}

inline
SRTTransform::SRTTransform(const FME::FaceBoneTransform& trans)
{
	mScale.x = trans.scaleVector[0];
	mScale.y = trans.scaleVector[1];
	mScale.z = trans.scaleVector[2];

	mRotation.x = trans.rotationQuaternion[0];
	mRotation.y = trans.rotationQuaternion[1];
	mRotation.z = trans.rotationQuaternion[2];
	mRotation.w = trans.rotationQuaternion[3];

	mTranslation.x = trans.translationVector[0];
	mTranslation.y = trans.translationVector[1];
	mTranslation.z = trans.translationVector[2];
}

/// clamp template, return value between [low, high]
template <typename T>
T TClamp(const T& value, const T& low, const T& high)
{
	return value < low ? low : (value > high ? high : value);
}
