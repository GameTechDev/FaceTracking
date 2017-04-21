/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
