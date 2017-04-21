/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "DXExtension.h"

HRESULT AllocateHierarchyExt::GenerateSkinnedMesh(IDirect3DDevice9* device, D3DXMESHCONTAINER_EXT* meshContainer)
{
	HRESULT hr = S_OK;

	if (!meshContainer->pSkinInfo)
		return hr;

	SAFE_RELEASE(meshContainer->MeshData.pMesh);
	SAFE_RELEASE(meshContainer->mBoneCombinationBuf);

	UINT MaxMatrices = 26;
	meshContainer->mNumPaletteEntries = min(MaxMatrices, meshContainer->pSkinInfo->GetNumBones());

	DWORD Flags = D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED;

	// convert to mesh that contains bone indices and weights
	hr = meshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
		(
			meshContainer->mOrigMesh,
			Flags,
			meshContainer->mNumPaletteEntries,
			meshContainer->pAdjacency,
			0, 0, 0,
			&meshContainer->mNumInfl,
			&meshContainer->mNumAttributeGroups,
			&meshContainer->mBoneCombinationBuf,
			&meshContainer->MeshData.pMesh);

	if (FAILED(hr))
		goto e_Exit;

	SAFE_RELEASE(meshContainer->mOrigMesh);

	const D3DVERTEXELEMENT9 vertexDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,		0 },
		{ 0, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,	0 },
		{ 0, 28, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES,	0 },
		{ 0, 44, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,		0 },
		{ 0, 52, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
		{ 0, 64, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,		0 },

		D3DDECL_END()
	};
	
	LPD3DXMESH mesh = 0;

	// convert to mesh with this vertex declaration
	hr = meshContainer->MeshData.pMesh->CloneMesh(meshContainer->MeshData.pMesh->GetOptions(), vertexDecl, device, &mesh);
	V_RETURN(hr);

	meshContainer->MeshData.pMesh->Release();
	meshContainer->MeshData.pMesh = mesh;
	mesh = 0;
	
	// recreate bones matrices memory if necessary
	if (*mNumBoneMatricesMaxPtr < meshContainer->pSkinInfo->GetNumBones())
	{
		*mNumBoneMatricesMaxPtr = meshContainer->pSkinInfo->GetNumBones();

		if (mBoneMatricesPtr)
			delete[] (*mBoneMatricesPtr);

		*mBoneMatricesPtr = new D3DXMATRIXA16[*mNumBoneMatricesMaxPtr];
		if (!(*mBoneMatricesPtr))
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}
	}

e_Exit:
	return hr;
}


HRESULT AllocateName(LPCSTR name, LPSTR* newName)
{
	UINT cbLength;

	if (name)
	{
		cbLength = (UINT)strlen(name) + 1;
		*newName = new CHAR[cbLength];
		if (!(*newName))
			return E_OUTOFMEMORY;
		memcpy(*newName, name, cbLength * sizeof(CHAR));
	}
	else
	{
		*newName = 0;
	}

	return S_OK;
}


HRESULT AllocateHierarchyExt::CreateFrame(LPCSTR name, LPD3DXFRAME* newFrame)
{
	HRESULT hr = S_OK;
	D3DXFRAME_EXT* frame;

	*newFrame = 0;

	frame = new D3DXFRAME_EXT;
	if (!frame)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	hr = AllocateName(name, &frame->Name);
	if (FAILED(hr))
		goto e_Exit;

	D3DXMatrixIdentity(&frame->TransformationMatrix);
	D3DXMatrixIdentity(&frame->mCombinedTransformationMatrix);

	frame->pMeshContainer = 0;
	frame->pFrameSibling = 0;
	frame->pFrameFirstChild = 0;

	*newFrame = frame;
	frame = 0;

e_Exit:
	delete frame;
	return hr;
}


HRESULT AllocateHierarchyExt::CreateMeshContainer(
	LPCSTR name,
	CONST D3DXMESHDATA *meshData,
	CONST D3DXMATERIAL *materials,
	CONST D3DXEFFECTINSTANCE *effectInstances,
	DWORD numMaterials,
	CONST DWORD *adjacency,
	LPD3DXSKININFO skinInfo,
	LPD3DXMESHCONTAINER *newMeshContainer)
{
	HRESULT hr;
	D3DXMESHCONTAINER_EXT *meshContainer = 0;
	UINT faceNum;
	UINT boneNum;
	LPDIRECT3DDEVICE9 device = 0;

	LPD3DXMESH mesh = 0;

	*newMeshContainer = 0;

	if (meshData->Type != D3DXMESHTYPE_MESH)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	mesh = meshData->pMesh;

	meshContainer = new D3DXMESHCONTAINER_EXT;
	if (!meshContainer)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memset(meshContainer, 0, sizeof(D3DXMESHCONTAINER_EXT));

	hr = AllocateName(name, &meshContainer->Name);
	if (FAILED(hr))
		goto e_Exit;

	mesh->GetDevice(&device);
	faceNum = mesh->GetNumFaces();

	meshContainer->MeshData.pMesh = mesh;
	meshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	mesh->AddRef();

	meshContainer->NumMaterials = max(1, numMaterials);
	meshContainer->pAdjacency = new DWORD[faceNum * 3];
	if (!meshContainer->pAdjacency)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	memcpy(meshContainer->pAdjacency, adjacency, sizeof(DWORD)* faceNum * 3);
	
	if (skinInfo)
	{
		meshContainer->pSkinInfo = skinInfo;
		skinInfo->AddRef();

		meshContainer->mOrigMesh = mesh;
		mesh->AddRef();

		boneNum = skinInfo->GetNumBones();
		meshContainer->mBoneOffsetMatrices = new D3DXMATRIX[boneNum];
		if (!meshContainer->mBoneOffsetMatrices)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		for (unsigned int i = 0; i < boneNum; i++)
		{
			meshContainer->mBoneOffsetMatrices[i] = *(meshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
		}

		hr = GenerateSkinnedMesh(device, meshContainer);
		if (FAILED(hr))
			goto e_Exit;
	}

	*newMeshContainer = meshContainer;
	meshContainer = 0;

e_Exit:
	SAFE_RELEASE(device);

	if (meshContainer)
	{
		DestroyMeshContainer(meshContainer);
	}

	return hr;
}


HRESULT AllocateHierarchyExt::DestroyFrame(LPD3DXFRAME frame)
{
	SAFE_DELETE_ARRAY(frame->Name);
	SAFE_DELETE(frame);
	return S_OK;
}


HRESULT AllocateHierarchyExt::DestroyMeshContainer(LPD3DXMESHCONTAINER meshContainerBase)
{
	D3DXMESHCONTAINER_EXT* meshContainer = (D3DXMESHCONTAINER_EXT*)meshContainerBase;

	SAFE_DELETE_ARRAY(meshContainer->Name);
	SAFE_DELETE_ARRAY(meshContainer->pAdjacency);
	SAFE_DELETE_ARRAY(meshContainer->mBoneOffsetMatrices);

	SAFE_DELETE_ARRAY(meshContainer->mBoneMatrixPtrs);
	SAFE_RELEASE(meshContainer->mBoneCombinationBuf);
	SAFE_RELEASE(meshContainer->MeshData.pMesh);
	SAFE_RELEASE(meshContainer->pSkinInfo);
	SAFE_RELEASE(meshContainer->mOrigMesh);
	SAFE_DELETE(meshContainer);
	return S_OK;
}
