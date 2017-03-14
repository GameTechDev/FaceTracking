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
#include "Sample.h"
#include "Model.h"
#include "Logger.h"

Sample* Sample::mInstance = 0;


Sample::Sample() 
	: mInit(false) 
	, mFont(0)
	, mTextSprite(0)
	, mFaceModel(0)
	, mDrawSkyBox(true)
	, mSkyBox(0)
	, mCameraRenderer(0)
	, mFMEManager(0)
	, mFAModuleInit(false)
{
	assert(!mInstance);
	mInstance = this;
}


Sample::~Sample()
{
	assert(mInstance);
	mInstance = 0;
}


bool Sample::HasInstance()
{
	return (mInstance != 0);
}


Sample* Sample::GetInstance()
{
	return mInstance;
}


int Sample::Run()
{
	if (!Init())
	{
		MessageBox(0, L"Failed to initialize sample", L"Error", MB_ICONERROR);
		return 1;
	}

	DXUTMainLoop();

	Destroy();

	return DXUTGetExitCode();
}


bool Sample::Init()
{
	assert(!mInit);
	if (mInit)
		return true;

	DXUTSetCallbackD3D9DeviceAcceptable(Sample::OnIsDeviceAcceptable);
	DXUTSetCallbackDeviceChanging(Sample::OnModifyDeviceSettings);
	DXUTSetCallbackD3D9DeviceCreated(Sample::OnCreateDevice);
	DXUTSetCallbackD3D9DeviceReset(Sample::OnResetDevice);
	DXUTSetCallbackD3D9FrameRender(Sample::OnFrameRender);
	DXUTSetCallbackD3D9DeviceLost(Sample::OnLostDevice);
	DXUTSetCallbackD3D9DeviceDestroyed(Sample::OnDestroyDevice);
	DXUTSetCallbackMsgProc(Sample::OnMsgProc);
	DXUTSetCallbackKeyboard(Sample::OnKeyboardProc);
	DXUTSetCallbackMouse(Sample::OnMouseProc);
	DXUTSetCallbackFrameMove(Sample::OnFrameMove);

	_VERY_RET(DXUTCreateWindow(L"RealSense Face Animation Demo"));

	DXUTSetCursorSettings(true, true);
	_VERY_RET(DXUTInit(true, true));
	DXUTSetHotkeyHandling(true, true, true);
	DXUTGetD3D9Enumeration()->SetPossibleVertexProcessingList(false, true, true, false);
	_VERY_RET(DXUTCreateDevice(true, 1280, 720));

	mSkyBox = new SkyBox();
	if (!mSkyBox->Init())
		return false;

	// create facial animation library
	if (!CreateFacialAnimModule())
	{
		//return false;
		::MessageBox(0, L"Failed to initialize camera module, please check the camera device.", L"Error", MB_ICONERROR);

		// camera initialization failed, we still let the program run.
		mFAModuleInit = false;
		//mInit = true;

		//return true;
	}
	else
		mFAModuleInit = true;

	mInit = true;

	return true;
}


void Sample::Destroy()
{
	assert(mInit);
	if (!mInit)
		return;

	SAFE_RELEASE(mFont);

	SAFE_RELEASE(mTextSprite);

	WaitForSingleObject(mCompleteSignal, INFINITE);

	assert(mFaceModel);
	if (mFaceModel)
		mFaceModel->Destroy();
	SAFE_DELETE(mFaceModel);

	assert(mSkyBox);
	if (mSkyBox)
		mSkyBox->Destroy();
	SAFE_DELETE(mSkyBox);

	// destroy facial animation module
	DestroyFacialAnimModule();

	mInit = false;
}


HRESULT Sample::HandleCreateDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc,
	void* userContext)
{
	HRESULT hr;

	// Initialize the font
	V_RETURN(D3DXCreateFont(device, INTRO_TEXT_FONT_SIZE, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Arial", &mFont));

	IDirect3D9* pD3D = DXUTGetD3D9Object();
	D3DCAPS9 Caps;
	device->GetDeviceCaps(&Caps);

	D3DDISPLAYMODE DisplayMode;
	device->GetDisplayMode(0, &DisplayMode);

	if (FAILED(pD3D->CheckDeviceFormat(Caps.AdapterOrdinal, Caps.DeviceType, DisplayMode.Format,
		D3DUSAGE_QUERY_FILTER, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F)))
		return S_FALSE;

	return S_OK;
}


HRESULT Sample::HandleResetDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc, void* userContext)
{
	HRESULT hr;

	if (mFont)
		V_RETURN(mFont->OnResetDevice());

	// Create a sprite to help batch calls when drawing many lines of text
	V_RETURN(D3DXCreateSprite(device, &mTextSprite));

	// Setup render state
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	mNormalViewPort.X = 0;
	mNormalViewPort.Y = 0;
	mNormalViewPort.Width = backBufferSurfaceDesc->Width;
	mNormalViewPort.Height = backBufferSurfaceDesc->Height;
	mNormalViewPort.MinZ = 0.0f;
	mNormalViewPort.MaxZ = 1.0f;

	mOffsetViewPort.X = 0;
	mOffsetViewPort.Y = 0;
	mOffsetViewPort.Width = (DWORD)(backBufferSurfaceDesc->Width * (1.0f - CAMERA_OVERLAY_RATIO));
	mOffsetViewPort.Height = backBufferSurfaceDesc->Height;
	mOffsetViewPort.MinZ = 0.0f;
	mOffsetViewPort.MaxZ = 1.0f;

	// reset facial animation module
	ResetFacialAnimModule();
	
	if (mFaceModel)
		mFaceModel->HandleResetDevice();

	if (mSkyBox)
		mSkyBox->HandleResetDevice();

	return S_OK;
}


void Sample::HandleFrameRender(IDirect3DDevice9* device, double time, float elapsedTime, void* userContext)
{
	// Clear the backbuffer
	device->Clear(0L, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_ARGB(255, 40, 40, 40), 1.0f, 0L);

	//device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// Begin the scene
	if (SUCCEEDED(device->BeginScene()))
	{
		if (mDrawSkyBox && mSkyBox)
		{
			mSkyBox->Draw();
		}

		bool cameraDraw = mFAModuleInit && mCameraRenderer 
			&& (mCameraRenderer->GetDrawVideoTexture() || mCameraRenderer->GetDrawDebugLayer());
		
		if (cameraDraw)
			device->SetViewport(&mOffsetViewPort);
		
		if (mFaceModel)
			mFaceModel->Draw();

		if (cameraDraw)
			device->SetViewport(&mNormalViewPort);

		// draw facial animation module
		DrawFacialAnimModule();

		DrawText();

		// End the scene.
		device->EndScene();
	}
}


LRESULT Sample::HandleMsgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing,
	void* userContext)
{
	mArcBall.HandleMessages(wnd, msg, wParam, lParam);
	return 0;
}


void Sample::HandleKeyboardProc(UINT c, bool keyDown, bool altDown, void* userContext)
{
	if (keyDown)
	{
		switch (c)
		{

		case 'C':
		{
			if (mCameraRenderer)
				mCameraRenderer->SetDrawVideoTexture(!mCameraRenderer->GetDrawVideoTexture());
		}
		break;

		case 'D':
		{
			if (mCameraRenderer)
				mCameraRenderer->SetDrawDebugLayer(!mCameraRenderer->GetDrawDebugLayer());
		}
		break;

		case 'S':
		{
			mDrawSkyBox = !mDrawSkyBox;
		}
		break;

        case 'R':
		{
			WaitForSingleObject(mCompleteSignal, INFINITE);
			mFMEManager->Reset();

			std::vector<IFaceBoneModel*> models;
			models.push_back(mFaceModel);
			mFMEManager->RegisterModels(&models);
			
			SetEvent(mCompleteSignal);
		}
        break;

#if defined(DEBUG) || defined(_DEBUG)
		case VK_F5:
		{
			if (mFaceModel)
				mFaceModel->RecreateShader();

			if (mSkyBox)
				mSkyBox->RecreateShader();
		}
		break;
#endif
		} // end of switch
	}
}


void Sample::HandleMouseProc(bool leftButtonDown, bool rightButtonDown, bool middleButtonDown, 
	bool sideButton1Down, bool sideButton2Down, int mouseWheelDelta, int xPos, int yPos, void* userContext)
{
	// do nothing
}


void Sample::HandleGUIEvent(UINT evt, int controlID, CDXUTControl* control, void* userContext)
{
	// do nothing
}


void Sample::HandleLostDevice(void* userContext)
{
	if (mFont)
		mFont->OnLostDevice();
	
	SAFE_RELEASE(mTextSprite);
	
	// handle facial animation module lost
	HandleLostFacialAnimModule();
	
	if (mFaceModel)
		mFaceModel->HandleLostDevice();

	if (mSkyBox)
		mSkyBox->HandleLostDevice();
}


void CALLBACK Sample::HandleDestroyDevice(void* userContext)
{
	SAFE_RELEASE(mFont);
	SAFE_RELEASE(mTextSprite);
}


void Sample::DrawText()
{
	const D3DSURFACE_DESC* backbufferDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	assert(backbufferDesc);
	if (!backbufferDesc)
		return;

	float left = 0;
	float top = 0;
	float right = (float)backbufferDesc->Width;
	float bottom = (float)backbufferDesc->Height;

	bool hasCameraVideo = (mFAModuleInit && mCameraRenderer 
		&& mCameraRenderer->GetDrawVideoTexture());

	if (hasCameraVideo)
	{
		int imageWidth = 0;
		int imageHeight = 0;
		mCameraRenderer->GetCameraImageSize(imageWidth, imageHeight);

		float drawImageWidth = backbufferDesc->Width * CAMERA_OVERLAY_RATIO;
		float drawImageHeight = drawImageWidth / imageWidth * imageHeight;
		
		left = backbufferDesc->Width - drawImageWidth;
		top = drawImageHeight;
		right = left + drawImageWidth;
	}

	left += INTRO_TEXT_MARGIN_X;
	right -= INTRO_TEXT_MARGIN_X;
	top += INTRO_TEXT_MARGIN_Y;
	bottom -= INTRO_TEXT_MARGIN_Y;

	CDXUTTextHelper txtHelper(mFont, mTextSprite, (int)INTRO_TEXT_LINE_HEIGHT);

	txtHelper.Begin();
	txtHelper.SetInsertionPos((int)left, (int)top);
	RECT rect = { (long)left, (long)top, (long)right, (long)bottom };

	txtHelper.DrawFormattedTextLine( L"FPS: %0.1f", DXUTGetFPS() ); // Show FPS

	DrawTextShadowed(txtHelper, rect, 1, DT_WORDBREAK | DT_CENTER | DT_VCENTER,
		INTRO_TEXT_FORGROUND_COLOR, INTRO_TEXT_SHADOE_COLOR,
		L"Controls：\n\
		Reset:  \"r\/R\" \n\
		Landmarks on/off: \"d\/D\" \n\
		Skybox on/off: \"s\/S\"\n\
		Rotate model: Left click drag \n\
		Zoom: Middle click drag \n\
		Pane: Right click drag \n\
		Quit: ESC");

	txtHelper.SetInsertionPos(5, 5);
	txtHelper.SetForegroundColor(D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.8f));
	//txtHelper.DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled())); 
	//txtHelper.DrawTextLine(DXUTGetDeviceStats());
	txtHelper.DrawTextLine(Logger::DumpString());
	Logger::ClearString();
	txtHelper.End();
}


void Sample::DrawTextShadowed(CDXUTTextHelper& txtHelper, const RECT& rect, 
	int shadowOffset, unsigned int flags,
	const D3DXCOLOR& color, const D3DXCOLOR& shadowColor, const std::wstring& text)
{
	RECT fgRect = rect;
	RECT shadowRect = rect;
	::OffsetRect(&shadowRect, shadowOffset, shadowOffset);
	
	// draw the background shadow
	txtHelper.SetForegroundColor(shadowColor);
	txtHelper.DrawTextLine(shadowRect, flags, text.c_str());
	
	// draw the foreground font
	txtHelper.SetForegroundColor(color);
	txtHelper.DrawTextLine(fgRect, flags, text.c_str());
}

bool CALLBACK Sample::HandleIsDeviceAcceptable(D3DCAPS9* caps, D3DFORMAT adapterFormat,
	D3DFORMAT backBufferFormat, bool windowed, void* userContext)
{
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	if (FAILED(pD3D->CheckDeviceFormat(caps->AdapterOrdinal, caps->DeviceType,
		adapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
		D3DRTYPE_TEXTURE, backBufferFormat)))
		return false;

	// vertex shader version should >= 3.0
	if (caps->VertexShaderVersion < D3DVS_VERSION(3, 0))
		return false;

	// pixel shader version should >= 3.0
	if (caps->PixelShaderVersion < D3DPS_VERSION(3, 0))
		return false;

	// support dynamic textures
	if (!(caps->Caps2 & D3DCAPS2_DYNAMICTEXTURES))
		return false;

	return true;
}


bool CALLBACK Sample::HandleModifyDeviceSettings(DXUTDeviceSettings* deviceSettings, void* userContext)
{
	assert(DXUT_D3D9_DEVICE == deviceSettings->ver);

	HRESULT hr;
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	D3DCAPS9 caps;

	V(pD3D->GetDeviceCaps(deviceSettings->d3d9.AdapterOrdinal,
		deviceSettings->d3d9.DeviceType,
		&caps));

	// turn off the vsync
	deviceSettings->d3d9.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	return true;
}


void Sample::HandleFrameMove(double time, float elapsedTime, void* userContext)
{
	const BoundingSphere& sphere = mFaceModel->GetBoundingSphere();

	D3DXMatrixTranslation(&mWorldMatrix, -sphere.mCenter.x,
		-sphere.mCenter.y,
		-sphere.mCenter.z);
	D3DXMatrixMultiply(&mWorldMatrix, &mWorldMatrix, mArcBall.GetRotationMatrix());
	D3DXMatrixMultiply(&mWorldMatrix, &mWorldMatrix, mArcBall.GetTranslationMatrix());

	D3DXVECTOR3 vEye(0, 0, -2 * sphere.mRadius);
	D3DXVECTOR3 vAt(0, 0, 0);
	D3DXVECTOR3 vUp(0, 1, 0);

	mCameraPos = D3DXVECTOR4(vEye.x, vEye.y, vEye.z, 1.0f);

	D3DXMatrixLookAtLH(&mViewMatrix, &vEye, &vAt, &vUp);

	mLightDir = D3DXVECTOR4(0.0f, 1.0f, -1.0f, 0.0f);
	D3DXVec4Normalize(&mLightDir, &mLightDir);

	const D3DSURFACE_DESC* backBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();

	if (mFAModuleInit && mCameraRenderer
		&& (mCameraRenderer->GetDrawVideoTexture() || mCameraRenderer->GetDrawDebugLayer()))
	{
		float fAspect = (float)backBufferSurfaceDesc->Width * (1.0f - CAMERA_OVERLAY_RATIO) / (float)backBufferSurfaceDesc->Height;
		D3DXMatrixPerspectiveFovLH(&mProjMatrix, D3DX_PI / 4, fAspect,
			sphere.mRadius / 64.0f, sphere.mRadius * 200.0f);
	}
	else
	{
		float fAspect = (float)backBufferSurfaceDesc->Width / (float)backBufferSurfaceDesc->Height;
		D3DXMatrixPerspectiveFovLH(&mProjMatrix, D3DX_PI / 4, fAspect,
			sphere.mRadius / 64.0f, sphere.mRadius * 200.0f);
	}


	// Setup the arcball parameters
	mArcBall.SetWindow(backBufferSurfaceDesc->Width, backBufferSurfaceDesc->Height, 0.85f);
	mArcBall.SetTranslationRadius(sphere.mRadius);

	// update Facial Animation Module
	UpdateFacialAnimModule(time, elapsedTime);

	// Update Model coordinates to world space
	mFaceModel->Update(time, elapsedTime);
}


bool Sample::CreateFacialAnimModule()
{

	Logger::Create();

	assert(!mFMEManager);
	assert(!mCameraRenderer);
	
	mFMEManager = FMECreate();

	FME::EMULATOR_CONFIG config;
	memset(&config,0,sizeof(config));
	config.logFun=Logger::WriteLog;

	if(!mFMEManager->Init(config))
	{
		assert(0 && "Failed to Init FaceMotionEmulator");
		return false;	
	}

	mCameraRenderer = new CameraImageRenderer();
	if (!mCameraRenderer->Initialize(mFMEManager))
	{
		assert(0 && "Failed to init CameraImageRenderer");
		return false;
	}

	mFaceModel = new Model();
	if (!mFaceModel->Init(mFMEManager))
	{
		assert(0 && "Failed to init FaceModel");
		return false;
	}
	
	///<<< Thread
	std::vector<IFaceBoneModel*> models;
	models.push_back(mFaceModel);
	mFMEManager->RegisterModels(&models); 

	mStartSignal = CreateEvent(NULL, FALSE, TRUE, NULL);
	mCompleteSignal = CreateEvent(NULL, FALSE, FALSE, NULL);
	CreateThread(0, 0, FMEThread, this, 0, 0);	
	WaitForSingleObject(mCompleteSignal, INFINITE); 
	SetEvent(mCompleteSignal);	
	///>>>
	return true;

}


void Sample::DestroyFacialAnimModule()
{
	if (!mFAModuleInit)
		return;

	assert(mCameraRenderer);
	assert(mFMEManager);

	if (mCameraRenderer)
	{
		mCameraRenderer->Destroy();
		SAFE_DELETE(mCameraRenderer);
	}

	if (mFMEManager)
	{
		mFMEManager->Release();
	}
	Logger::Destroy();

	///<<< Thread
	CloseHandle(mStartSignal);
	CloseHandle(mCompleteSignal);
	///>>>

}


bool Sample::RecreateFacialAnimModule()
{
	DestroyFacialAnimModule();
	return CreateFacialAnimModule();
}


void Sample::UpdateFacialAnimModule(double fTime, float fElapsedTime)
{
	if (!mFAModuleInit)
		return;

	assert(mCameraRenderer);
	assert(mFMEManager);
	assert(mFaceModel);

	mTime = fTime;
	mElapsedTime = fElapsedTime;
	if(WaitForSingleObject(mCompleteSignal, 0) == WAIT_OBJECT_0 ){ //测试FME处理一帧是否结束，如果结束，就把数据从FME导出到外部，并通知FME开始处理下一帧。

		//todo: retrieve data from FME
		mCameraRenderer->Update(); 

		mFaceModel->PreUpdate(fTime, fElapsedTime); 

		mFMEManager->QueryExpressionConficence(mConfidence);

		SetEvent(mStartSignal);
	}

}


void Sample::DrawFacialAnimModule()
{
	if (!mFAModuleInit)
		return;

	assert(mCameraRenderer);
	if (mCameraRenderer)
	{
		// TODO: This progress value should be set, we use random number for now
		mCameraRenderer->Draw(mConfidence);
	}
}


void Sample::ResetFacialAnimModule()
{
	if (!mFAModuleInit)
		return;

	if (mCameraRenderer)
		mCameraRenderer->ResetDevice();
}


void Sample::HandleLostFacialAnimModule()
{
	if (!mFAModuleInit)
		return;

	if (mCameraRenderer)
		mCameraRenderer->HandleLostDevice();
}


//////////////////////////////////////////////////////////////////////////
// DXUT static callbacks
//////////////////////////////////////////////////////////////////////////

HRESULT CALLBACK Sample::OnCreateDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc, void* userContext)
{
	if (!Sample::HasInstance())
		return S_OK;

	return Sample::GetInstance()->HandleCreateDevice(device, backBufferSurfaceDesc, userContext);
}


HRESULT CALLBACK Sample::OnResetDevice(IDirect3DDevice9* device, const D3DSURFACE_DESC* backBufferSurfaceDesc, void* userContext)
{
	if (!Sample::HasInstance())
		return S_OK;

	return Sample::GetInstance()->HandleResetDevice(device, backBufferSurfaceDesc, userContext);
}


void CALLBACK Sample::OnFrameRender(IDirect3DDevice9* device, double time, float elapsedTime, void* userContext)
{
	if (!Sample::HasInstance())
		return;

	Sample::GetInstance()->HandleFrameRender(device, time, elapsedTime, userContext);
}


LRESULT CALLBACK Sample::OnMsgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* noFurtherProcessing, void* userContext)
{
	if (!Sample::HasInstance())
		return S_OK;

	return Sample::GetInstance()->HandleMsgProc(wnd, msg, wParam, lParam, noFurtherProcessing, userContext);
}


void CALLBACK Sample::OnKeyboardProc(UINT c, bool keyDown, bool altDown, void* userContext)
{
	if (!Sample::HasInstance())
		return;

	Sample::GetInstance()->HandleKeyboardProc(c, keyDown, altDown, userContext);
}


void CALLBACK Sample::OnMouseProc(bool leftButtonDown, bool rightButtonDown, bool middleButtonDown, bool sideButton1Down, bool sideButton2Down, int mouseWheelDelta, int xPos, int yPos, void* userContext)
{
	if (!Sample::HasInstance())
		return;

	Sample::GetInstance()->HandleMouseProc(leftButtonDown, rightButtonDown, middleButtonDown, sideButton1Down, sideButton2Down, mouseWheelDelta, xPos, yPos, userContext);
}


void CALLBACK Sample::OnGUIEvent(UINT evt, int controlID, CDXUTControl* control, void* userContext)
{
	if (!Sample::HasInstance())
		return;

	Sample::GetInstance()->HandleGUIEvent(evt, controlID, control, userContext);
}


void CALLBACK Sample::OnLostDevice(void* userContext)
{
	if (!Sample::HasInstance())
		return;

	Sample::GetInstance()->HandleLostDevice(userContext);
}


void CALLBACK Sample::OnDestroyDevice(void* userContext)
{
	if (!Sample::HasInstance())
		return;

	Sample::GetInstance()->HandleDestroyDevice(userContext);
}


bool CALLBACK Sample::OnIsDeviceAcceptable(D3DCAPS9* caps, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat, bool windowed, void* userContext)
{
	if (!Sample::HasInstance())
		return false;

	return Sample::GetInstance()->HandleIsDeviceAcceptable(caps, adapterFormat, backBufferFormat, windowed, userContext);
}


bool CALLBACK Sample::OnModifyDeviceSettings(DXUTDeviceSettings* deviceSettings, void* userContext)
{
	if (!Sample::HasInstance())
		return false;

	return Sample::GetInstance()->HandleModifyDeviceSettings(deviceSettings, userContext);
}


void CALLBACK Sample::OnFrameMove(double time, float elapsedTime, void* userContext)
{
	if (!Sample::HasInstance())
		return;

	Sample::GetInstance()->HandleFrameMove(time, elapsedTime, userContext);
}


DWORD WINAPI Sample::FMEThread(LPVOID arg)
{
	Sample* sample = (Sample*)arg;
	assert(sample);

	IFaceMotionEmulator* fmeManager = sample->mFMEManager;
	assert(fmeManager);

	while(true){
		WaitForSingleObject(sample->mStartSignal, INFINITE);

		if (fmeManager)
			fmeManager->Tick(sample->mTime, sample->mElapsedTime);

		SetEvent(sample->mCompleteSignal);
	}
}