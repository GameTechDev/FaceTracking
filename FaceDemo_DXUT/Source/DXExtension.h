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
