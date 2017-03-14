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
#include "SkyBox.h"
#include "..\..\IFaceME.h"

#include "CameraImageRenderer.h"

// forward declaration
class Model;

/// Sample class 
class Sample
{
public:
	/// Constructor
	Sample();
	/// Destructor
	~Sample();
	/// return if the singleton instance exists
	static bool HasInstance();
	/// Get the singleton instance
	static Sample* GetInstance();
	/// Get face model object
	Model* GetFaceModel() const;
	/// Get sky box object
	SkyBox* GetSkyBox() const;
	/// Run the sample, can be called after initialization
	int Run();
	/// Initialization
	bool Init();
	/// Destroy
	void Destroy();
	/// Get global world matrix
	const D3DXMATRIXA16& GetWorldMatrix() const;
	/// Get global view matrix
	const D3DXMATRIXA16& GetViewMatrix() const;
	/// Get global projection matrix
	const D3DXMATRIXA16& GetProjMatrix() const;
	/// Get camera position in world space
	const D3DXVECTOR4& GetCameraPos() const;


private:
	/// Draw text
	void DrawText();
	/// Helper function to draw shadowed text
	void DrawTextShadowed(CDXUTTextHelper &txtHelper, const RECT& rect, int shadowOffset, 
		unsigned int flags, const D3DXCOLOR& color, const D3DXCOLOR& shadowColor, const std::wstring& text);

	///<<< Animation module related memebers
	bool CreateFacialAnimModule();
	void DestroyFacialAnimModule();
	bool RecreateFacialAnimModule();
	void UpdateFacialAnimModule(double fTime, float fElapsedTime);
	void DrawFacialAnimModule();
	void ResetFacialAnimModule();
	void HandleLostFacialAnimModule();
	///>>>

	///<<< Members for handle DXUT events
	HRESULT CALLBACK HandleCreateDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc, void* userContext);
	HRESULT CALLBACK HandleResetDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc, void* userContext);
	void CALLBACK HandleFrameRender(IDirect3DDevice9* device, double time, float elapsedTime, void* userContext);
	LRESULT CALLBACK HandleMsgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing, void* userContext);
	void CALLBACK HandleKeyboardProc(UINT c, bool keyDown, bool altDown, void* userContext);
	void CALLBACK HandleMouseProc(bool leftButtonDown, bool rightButtonDown, bool middleButtonDown, bool sideButton1Down, bool sideButton2Down, int mouseWheelDelta, int xPos, int yPos, void* userContext);
	void CALLBACK HandleGUIEvent(UINT evt, int controlID, CDXUTControl* control, void* userContext);
	void CALLBACK HandleLostDevice(void* userContext);
	void CALLBACK HandleDestroyDevice(void* userContext);
	bool CALLBACK HandleIsDeviceAcceptable(D3DCAPS9* caps, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat, bool windowed, void* userContext);
	bool CALLBACK HandleModifyDeviceSettings(DXUTDeviceSettings* deviceSettings, void* userContext);
	void CALLBACK HandleFrameMove(double time, float elapsedTime, void* userContext);
	///>>>

	///<<< DXUT callbacks
	static HRESULT CALLBACK OnCreateDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc, void* userContext);
	static HRESULT CALLBACK OnResetDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc, void* userContext);
	static void CALLBACK OnFrameRender(IDirect3DDevice9* device, double time, float elapsedTime, void* userContext);
	static LRESULT CALLBACK OnMsgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing, void* userContext);
	static void CALLBACK OnKeyboardProc(UINT c, bool keyDown, bool altDown, void* userContext);
	static void CALLBACK OnMouseProc(bool leftButtonDown, bool rightButtonDown, bool middleButtonDown, bool sideButton1Down, bool sideButton2Down, int mouseWheelDelta, int xPos, int yPos, void* userContext);
	static void CALLBACK OnGUIEvent(UINT evt, int controlID, CDXUTControl* control, void* userContext);
	static void CALLBACK OnLostDevice(void* userContext);
	static void CALLBACK OnDestroyDevice(void* userContext);
	static bool CALLBACK OnIsDeviceAcceptable(D3DCAPS9* caps, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat, bool windowed, void* userContext);
	static bool CALLBACK OnModifyDeviceSettings(DXUTDeviceSettings* deviceSettings, void* userContext);
	static void CALLBACK OnFrameMove(double time, float elapsedTime, void* userContext);
	///>>>

private:
	static Sample* mInstance;
	bool mInit;
	bool mFAModuleInit;
	bool mDrawSkyBox;
	ID3DXFont* mFont;
	ID3DXSprite* mTextSprite;
	CD3DArcBall mArcBall;
	Model* mFaceModel;
	SkyBox* mSkyBox;
	D3DXMATRIXA16 mWorldMatrix;
	D3DXMATRIXA16 mViewMatrix;
	D3DXMATRIXA16 mProjMatrix;
	D3DXVECTOR4 mLightDir;
	D3DXVECTOR4 mCameraPos;
	D3DVIEWPORT9 mNormalViewPort;
	D3DVIEWPORT9 mOffsetViewPort;

	CameraImageRenderer* mCameraRenderer;
	IFaceMotionEmulator* mFMEManager;
	float	mConfidence;

	///<<< Thread
	static DWORD WINAPI FMEThread(LPVOID arg); 
	HANDLE mStartSignal;
	HANDLE mCompleteSignal;
	double mTime;
	float  mElapsedTime;
	///>>>

};


inline
Model* Sample::GetFaceModel() const
{
	return mFaceModel;
}


inline
SkyBox* Sample::GetSkyBox() const
{
	return mSkyBox;
}


inline
const D3DXMATRIXA16& Sample::GetWorldMatrix() const
{
	return mWorldMatrix;
}


inline
const D3DXMATRIXA16& Sample::GetViewMatrix() const
{
	return mViewMatrix;
}


inline
const D3DXMATRIXA16& Sample::GetProjMatrix() const
{
	return mProjMatrix;
}


inline
const D3DXVECTOR4& Sample::GetCameraPos() const
{
	return mCameraPos;
}
