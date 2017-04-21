/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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