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


class SkyBox
{
public:
	/// Constructor
	SkyBox();
	/// Destructor
	~SkyBox();
	/// Initialization
	bool Init();
	/// Destroy
	void Destroy();
	/// Draw the sky box
	void Draw();
	/// Handle device reset
	void HandleResetDevice();
	/// Handle device lost
	void HandleLostDevice();
	/// Recreate shader
	bool RecreateShader();
	/// Get environment texture
	IDirect3DCubeTexture9* GetEnvironTexture() const;

private:
	/// Create environment texture
	bool CreateEnvironTexture();
	/// Recreate vertex buffer
	bool RecreateVB();

private:
	struct SkyBoxVertex
	{
		D3DXVECTOR4 mPos;
		D3DXVECTOR3 mUV0;
	};

	static const D3DVERTEXELEMENT9 SKYBOX_VERTEX_DECL[];

	bool mInit;
	IDirect3DCubeTexture9* mEnvironTexture;
	IDirect3DCubeTexture9* mSHEnvironMap;
    ID3DXEffect* mEffect;
	IDirect3DVertexBuffer9* mVB;
    IDirect3DVertexDeclaration9* mVertexDecl;
};

inline
IDirect3DCubeTexture9* SkyBox::GetEnvironTexture() const
{
	return mEnvironTexture;
}