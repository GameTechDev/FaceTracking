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
#include "SkyBox.h"
#include "Sample.h"



const D3DVERTEXELEMENT9 SkyBox::SKYBOX_VERTEX_DECL[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    D3DDECL_END()
};



SkyBox::SkyBox()
	: mEnvironTexture(0)
	, mSHEnvironMap(0)
	, mEffect(0)
	, mVB(0)
	, mVertexDecl(0)
	, mInit(false)
{

}


SkyBox::~SkyBox()
{

}

bool SkyBox::Init()
{
	assert(!mInit);
	if (mInit)
		return true;

	if (!RecreateShader())
		return false;

	if (!RecreateVB())
		return false;

	if (!CreateEnvironTexture())
		return false;

	mInit = true;

	return true;
}


void SkyBox::Destroy()
{
	assert(mInit);
	if (!mInit)
		return;

	SAFE_RELEASE(mEnvironTexture);
	SAFE_RELEASE(mSHEnvironMap);
	SAFE_RELEASE(mEffect);
	SAFE_RELEASE(mVertexDecl);
	SAFE_RELEASE(mVB);

	mInit = false;
}


void SkyBox::Draw()
{
	Sample* app = Sample::GetInstance();
	assert(app);

	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);

	HRESULT hr;

	D3DXMATRIX mInvWorldViewProj;
	D3DXMATRIX worldViewProjMatrix = app->GetWorldMatrix() * app->GetViewMatrix() * app->GetProjMatrix();
	D3DXMatrixInverse(&mInvWorldViewProj, 0, &worldViewProjMatrix);

	V(mEffect->SetMatrix("InvertMVP", &mInvWorldViewProj));
	V(mEffect->SetTechnique("SkyBox"));
	V(mEffect->SetTexture("EnvironTexture", mEnvironTexture));

	device->SetStreamSource(0, mVB, 0, sizeof(SkyBoxVertex));
	device->SetVertexDeclaration(mVertexDecl);

	UINT passNum = 0;
	V(mEffect->Begin(&passNum, 0));
	{
		for (unsigned int i = 0; i < passNum; i++)
		{
			V(mEffect->BeginPass(i));
			{
				device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			}
			V(mEffect->EndPass());
		}
	}
	V(mEffect->End());
}


bool SkyBox::RecreateShader()
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);

	DWORD shaderFlags = D3DXFX_NOT_CLONEABLE;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DXSHADER_DEBUG;
#endif

	ID3DXEffect* effect = 0;
	LPD3DXBUFFER buff = 0;
	if (FAILED(D3DXCreateEffectFromFile(device, L"Data\\Shader\\SkyBox.fx", 0, 0, shaderFlags,
		0, &effect, &buff)))
	{
		OutputDebugStringA("====================== BEGIN SHADER ERROR ======================\n");
		OutputDebugStringA((const char*)buff->GetBufferPointer());
		OutputDebugStringA("====================== END SHADER ERROR ======================\n");
		assert(0 && "Failed to create effect");

		SAFE_RELEASE(buff);

		return false;
	}

	if (buff && buff->GetBufferPointer())
	{
		OutputDebugStringA("====================== BEGIN SHADER WARNING ======================\n");
		OutputDebugStringA((const char*)buff->GetBufferPointer());
		OutputDebugStringA("====================== END SHADER WARNING ======================\n");

		SAFE_RELEASE(buff);
	}

	SAFE_RELEASE(mEffect);
	mEffect = effect;

	OutputDebugStringA("\nShader recreated\n");

	return true;
}


bool SkyBox::RecreateVB()
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);
	const D3DSURFACE_DESC* backBufferSurfaceDesc = DXUTGetD3D9BackBufferSurfaceDesc();
	assert(backBufferSurfaceDesc);

	if (!mVertexDecl)
		_VERY_RET(device->CreateVertexDeclaration(SKYBOX_VERTEX_DECL, &mVertexDecl));

	assert(!mVB);
	_VERY_RET(device->CreateVertexBuffer(4 * sizeof(SkyBoxVertex),
		D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &mVB, 0));

	SkyBoxVertex* vertex = 0;
	_VERY_RET(mVB->Lock(0, 0, (void**)&vertex, 0));
	{
		float highW = -1.0f - (1.0f / (float)backBufferSurfaceDesc->Width);
		float highH = -1.0f - (1.0f / (float)backBufferSurfaceDesc->Height);
		float lowW = 1.0f + (1.0f / (float)backBufferSurfaceDesc->Width);
		float lowH = 1.0f + (1.0f / (float)backBufferSurfaceDesc->Height);

		vertex[0].mPos = D3DXVECTOR4(lowW, lowH, 1.0f, 1.0f);
		vertex[1].mPos = D3DXVECTOR4(lowW, highH, 1.0f, 1.0f);
		vertex[2].mPos = D3DXVECTOR4(highW, lowH, 1.0f, 1.0f);
		vertex[3].mPos = D3DXVECTOR4(highW, highH, 1.0f, 1.0f);
	}

	mVB->Unlock();

	return true;
}


bool SkyBox::CreateEnvironTexture()
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);

	// TODO: we should check device format support and fall back if necessary
	_VERY_RET(D3DXCreateCubeTextureFromFileEx(device, L"Data\\Model\\Environ.dds", 512, 1, 0, D3DFMT_A16B16G16R16F,
		D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, 0, 0, &mEnvironTexture));

	return true;
}


void SkyBox::HandleResetDevice()
{
	HRESULT hr;
	if (mEffect)
		V(mEffect->OnResetDevice());

	V(RecreateVB());
}


void SkyBox::HandleLostDevice()
{
    HRESULT hr;
    if( mEffect )
        V( mEffect->OnLostDevice() );

    SAFE_RELEASE(mVB);
}




