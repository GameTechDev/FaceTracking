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
#include "CameraImageRenderer.h"
#include "Common.h"
#include "Math.h"


// callback function to fulfill the solid texture
VOID WINAPI CALLBACK_COLOR_FILL(D3DXVECTOR4* out, const D3DXVECTOR2* texCoord,
	const D3DXVECTOR2* texelSize, LPVOID data)
{
	*out = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
}


CameraImageRenderer::CameraImageRenderer()
	: mSprite(0)
	, mVideoTexture(0)
	, mSolidTexture(0)
	, mFaceManager(0)
	, mCameraImageWidth(512)
	, mCameraImageHeight(512)
	, mInit(false)
	, mLandmarkArrayReady(false)
	, mDrawVideoTexture(true)
	, mDrawDebugLayer(true)
{

}

CameraImageRenderer::~CameraImageRenderer()
{
	if (mInit)
		Destroy();
}

bool CameraImageRenderer::Initialize(IFaceMotionEmulator* manager)
{
	assert(!mInit);
	if (mInit)
		return true;

	assert(manager);
	if (!manager)
		return false;
	
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);
	if (!device)
		return false;

	HRESULT hr = D3DXCreateSprite(device, &mSprite);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	if (!RecreateVideoTexture(mCameraImageWidth, mCameraImageHeight))
		return false;

	if (!RecreateSolidTexture())
		return false;

	mFaceManager = manager;

	mInit = true;

	return true;
}

bool CameraImageRenderer::RecreateVideoTexture(int width, int height)
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);
	if (!device)
		return false;

	SAFE_RELEASE(mVideoTexture);

	// check if D3DCAPS2_DYNAMICTEXTURES is supported
	const D3DCAPS9* caps = DXUTGetD3D9DeviceCaps();
	if (!(caps->Caps2 & D3DCAPS2_DYNAMICTEXTURES))
	{
		assert(0 && "D3DCAPS2_DYNAMICTEXTURES cap required to run this demo");
		return false;
	}

	// create texture
	HRESULT hr = D3DXCreateTexture(device, width, height, 1, 
		D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mVideoTexture);
	
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	mCameraImageWidth = width;
	mCameraImageHeight = height;

	return true;
}

bool CameraImageRenderer::RecreateSolidTexture()
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);
	if (!device)
		return false;

	SAFE_RELEASE(mSolidTexture);

	HRESULT hr = D3DXCreateTexture(device, LANDMARK_WIDTH, LANDMARK_WIDTH, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mSolidTexture);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	hr = D3DXFillTexture(mSolidTexture, CALLBACK_COLOR_FILL, 0);

	return true;
}

void CameraImageRenderer::Destroy()
{
	assert(mInit);
	if (!mInit)
		return;

	SAFE_RELEASE(mSprite);
	SAFE_RELEASE(mVideoTexture);
	SAFE_RELEASE(mSolidTexture);

	mInit = false;
}

void CameraImageRenderer::Update()
{
	BGRAImage image;

	if (mFaceManager->QueryBGRAImage(image))
	{
		if (mCameraImageWidth != image.width || mCameraImageHeight != image.height)
		{
			if (!RecreateVideoTexture(image.width, image.height))
			{
				assert(0 && "recreate image texture failed");
				return;
			}
		}

		D3DLOCKED_RECT lockedRect = { 0 };

		HRESULT hr = mVideoTexture->LockRect(0, &lockedRect, 0, D3DLOCK_DISCARD);
		assert(SUCCEEDED(hr));

		if (SUCCEEDED(hr))
		{
			unsigned char* dst = (unsigned char*)lockedRect.pBits;
			unsigned char* src = &image.buffer[0];
			unsigned int rowSize = image.width * sizeof(unsigned char) * 4;
			unsigned int srcPitch = image.width * sizeof(unsigned char) * 4;

			// if same pitch, copy the whole image
			if (srcPitch == (unsigned int)lockedRect.Pitch)
			{
				memcpy(dst, src, srcPitch * image.height);
			}
			else // else, row by row copy
			{
				for (size_t i = 0; i < (size_t)image.height; ++i)
				{
					memcpy(dst, src, rowSize);
					dst += lockedRect.Pitch;
					src += rowSize;
				}
			}
		}
		mVideoTexture->UnlockRect(0);
	}

	mLandmarkArrayReady = mFaceManager->QueryLandmarks(mLandmarkArray);
}

void CameraImageRenderer::Draw(float progress)
{
	if (!mInit)
		return;

	if (!mSprite)
		return;

	if (!mDrawVideoTexture && !mDrawDebugLayer)
		return;

	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);
	if (!device)
		return;

	const D3DSURFACE_DESC* backBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	assert(backBufferSurfaceDesc);
	if (!backBufferSurfaceDesc)
		return;

	// here, we assume the ID3DXSprite object will take care all of the batch, it will, to some extent.
	mSprite->Begin(D3DXSPRITE_ALPHABLEND);

	float screenWidth = (float)backBufferSurfaceDesc->Width;
	float targetWidth = screenWidth * CAMERA_OVERLAY_RATIO;
	
	float targetScale = targetWidth / mCameraImageWidth;

	// draw the video image
	D3DXVECTOR2 trans(screenWidth - targetWidth, 0.0f);
	
	if (mDrawVideoTexture)
	{
		D3DXMATRIX matVideo;
		D3DXMatrixAffineTransformation2D(&matVideo, targetScale, 0, 0.0f, &trans);
		mSprite->SetTransform(&matVideo);
		mSprite->Draw(mVideoTexture, 0, 0, 0, D3DCOLOR_RGBA(255, 255, 255, 255));
	}

	if (mDrawDebugLayer)
	{
		// draw the land marks
		if (mLandmarkArrayReady)
		{
			D3DXMATRIX matLM;
			D3DXMatrixAffineTransformation2D(&matLM, 1.0, 0, 0.0f, &trans);
			mSprite->SetTransform(&matLM);

			for (size_t i = 0; i < mLandmarkArray.size(); ++i)
			{
				const Landmark& point = mLandmarkArray[i];
				const float halfSize = LANDMARK_WIDTH / 2.0f;
				D3DXVECTOR3 pos(point.image.x - halfSize, point.image.y - halfSize, 0);
				pos *= targetScale;

				D3DCOLOR color = (point.confidenceImage) ? CONFIDENT_COLOR : UNCONFIDENT_COLOR;
				mSprite->Draw(mSolidTexture, 0, 0, &pos, color);
			}
		}
	}

	// draw the text background
	trans.y += mCameraImageHeight * targetScale;
	D3DXVECTOR2 bgSize(targetWidth / LANDMARK_WIDTH, (screenWidth - targetScale * mCameraImageHeight) / LANDMARK_WIDTH);
	D3DXMATRIX matBg;
	D3DXMatrixTransformation2D(&matBg, 0, 0.0f, &bgSize, 0, 0.0f, &trans);
	mSprite->SetTransform(&matBg);
	mSprite->Draw(mSolidTexture, 0, 0, 0, INTRO_TEXT_BG_COLOR);

	// draw the progress bar
	trans.x = PROGRESS_BAR_MARGIN_X;
	trans.y = backBufferSurfaceDesc->Height - PROGRESS_BAR_HEIGHT - PROGRESS_BAR_MARGIN_Y;

	// draw the background of progress bar
	D3DXVECTOR2 totalSize(PROGRESS_BAR_WIDTH / LANDMARK_WIDTH, PROGRESS_BAR_HEIGHT / LANDMARK_WIDTH);
	D3DXMATRIX totalMat;
	D3DXMatrixTransformation2D(&totalMat, 0, 0.0f, &totalSize, 0, 0.0f, &trans);
	mSprite->SetTransform(&totalMat);
	mSprite->Draw(mSolidTexture, 0, 0, 0, PROGRESS_BAR_BG_COLOR);

	// draw the foreground of progress bar
	float clampProgress = TClamp(progress, 0.0f, 1.0f);
	D3DXVECTOR2 progressSize(PROGRESS_BAR_WIDTH * clampProgress / LANDMARK_WIDTH, PROGRESS_BAR_HEIGHT / LANDMARK_WIDTH);
	D3DXMATRIX progressMat;
	D3DXMatrixTransformation2D(&progressMat, 0, 0.0f, &progressSize, 0, 0.0f, &trans);
	mSprite->SetTransform(&progressMat);
	mSprite->Draw(mSolidTexture, 0, 0, 0, PROGRESS_BAR_FG_COLOR);

	mSprite->End();
}

void CameraImageRenderer::GetCameraImageSize(int& width, int& height) const
{
	width = mCameraImageWidth;
	height = mCameraImageHeight;
}

void CameraImageRenderer::HandleLostDevice()
{
	SAFE_RELEASE(mSprite);
	SAFE_RELEASE(mVideoTexture);
	SAFE_RELEASE(mSolidTexture);
}

void CameraImageRenderer::ResetDevice()
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);
	if (!device)
		return;

	HRESULT hr = D3DXCreateSprite(device, &mSprite);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return;

	if (!RecreateVideoTexture(mCameraImageWidth, mCameraImageHeight))
		return;

	if (!RecreateSolidTexture())
		return;
}

