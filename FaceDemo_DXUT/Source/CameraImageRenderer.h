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

