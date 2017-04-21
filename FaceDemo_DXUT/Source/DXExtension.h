/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Common.h"
#include "Math.h"



struct D3DXFRAME_EXT : public D3DXFRAME
{
	D3DXMATRIXA16 mCombinedTransformationMatrix;
	SRTTransform mLocalTransform; // local transformation
};


struct D3DXMESHCONTAINER_EXT : public D3DXMESHCONTAINER
{
	LPD3DXMESH mOrigMesh;
	DWORD mNumAttributeGroups;
	DWORD mNumInfl;
	LPD3DXBUFFER mBoneCombinationBuf;
	D3DXMATRIX** mBoneMatrixPtrs;
	D3DXMATRIX* mBoneOffsetMatrices;
	DWORD mNumPaletteEntries;
};


class AllocateHierarchyExt : public ID3DXAllocateHierarchy
{
public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR name, LPD3DXFRAME *newFrame);
	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR name,
		CONST D3DXMESHDATA* meshData,
		CONST D3DXMATERIAL* materials,
		CONST D3DXEFFECTINSTANCE* effectInstances,
		DWORD numMaterials,
		CONST DWORD* adjacency,
		LPD3DXSKININFO skinInfo,
		LPD3DXMESHCONTAINER* newMeshContainer);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME frame);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER meshContainerBase);

	AllocateHierarchyExt(unsigned int* numBoneMatricesMaxPtr, D3DXMATRIXA16** boneMatricesPtr)
		: mNumBoneMatricesMaxPtr(numBoneMatricesMaxPtr)
		, mBoneMatricesPtr(boneMatricesPtr)
	{
	}

	HRESULT GenerateSkinnedMesh(IDirect3DDevice9* device, D3DXMESHCONTAINER_EXT* meshContainer);

	unsigned int* mNumBoneMatricesMaxPtr;
	D3DXMATRIXA16** mBoneMatricesPtr;
};
