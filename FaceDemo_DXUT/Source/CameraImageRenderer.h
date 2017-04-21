/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "..\..\IFaceME.h"

using namespace FME;

/// Helper class to draw camera images
class CameraImageRenderer
{
public:
	/// Constructor
	CameraImageRenderer();
	/// Destructor
	~CameraImageRenderer();

	/// do the initialization
	bool Initialize(IFaceMotionEmulator* manager);

	/// destroy
	void Destroy();
	/// Update
	void Update();
	/// Draw the images
	void Draw(float progress);
	/// Get width and height
	void GetCameraImageSize(int& width, int& height) const;
	/// Should be called when device lost
	void HandleLostDevice();
	/// Should be called when device reset
	void ResetDevice();
	/// Get if video texture should be drawn
	bool GetDrawVideoTexture() const;
	/// Set if video texture should be drawn
	void SetDrawVideoTexture(bool draw);
	/// Get if debug layer should be drawn
	bool GetDrawDebugLayer() const;
	/// Set if debug layer should be drawn
	void SetDrawDebugLayer(bool draw);

private:
	/// Delete old image texture and create again
	bool RecreateVideoTexture(int width, int height);

	/// Delete old solid texture and create again
	bool RecreateSolidTexture();

	ID3DXSprite* mSprite;
	IDirect3DTexture9* mVideoTexture;
	IDirect3DTexture9* mSolidTexture;
	IFaceMotionEmulator* mFaceManager;
	int mCameraImageWidth;
	int mCameraImageHeight;
	bool mInit;
	bool mLandmarkArrayReady;
	LandmarkArray mLandmarkArray;
	bool mDrawVideoTexture;
	bool mDrawDebugLayer;
};


inline
bool CameraImageRenderer::GetDrawVideoTexture() const 
{ 
	return mDrawVideoTexture;
}


inline
void CameraImageRenderer::SetDrawVideoTexture(bool draw) 
{ 
	mDrawVideoTexture = draw;
}


inline
bool CameraImageRenderer::GetDrawDebugLayer() const
{ 
	return mDrawDebugLayer;
}


inline
void CameraImageRenderer::SetDrawDebugLayer(bool draw)
{ 
	mDrawDebugLayer = draw;
}

