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
#include "Model.h"
#include "Sample.h"


const std::string Model::ROOT_FRAME_NAME = "Root";
const std::wstring Model::NEUTRAL_FACE_FILE_NAME = L"Neutral.X";
const unsigned int Model::MATERIAL_NUMBER = 3;


const std::wstring Model::GetFacialFileName(ActionUnit action)
{
	switch (action)
	{
	case MOUTH_OPEN:		return L"MouthOpen.X";
	case MOUTH_LEFT_RAISE:	return L"MouthSmileL.X";
	case MOUTH_RIGHT_RAISE:	return L"MouthSmileR.X";
	case MOUTH_LEFT:		return L"MouthRight.X"; 
	case MOUTH_RIGHT:		return L"MouthLeft.X";

	case EYELID_CLOSE_L:	return L"EyeLid_Close_L.X";
	case EYELID_CLOSE_R:	return L"EyeLid_Close_R.X";
	case EYELID_OPEN_L:		return L"EyeLid_Open_L.X";
	case EYELID_OPEN_R:		return L"EyeLid_Open_R.X";

	case EYEBROW_Up_L:		return L"EyeBrow_up_L.X";
	case EYEBROW_Up_R:		return L"EyeBrow_up_R.X";
	case EYEBROW_DOWN_L:	return L"EyeBrow_Down_L.X";
	case EYEBROW_DOWN_R:	return L"EyeBrow_Down_R.X";

	case EYEBALL_TURN_L:	return L"EyeBall_Turn_L.X";
	case EYEBALL_TURN_R:	return L"EyeBall_Turn_R.X";
	case EYEBALL_TURN_U:	return L"EyeBall_Turn_U.X";
	case EYEBALL_TRUN_D:	return L"EyeBall_Turn_D.X";

	default:	return L"";
	}
}


Model::Model() 
	: mInit(false)
	, mEffect(0)
	, mRootFrame(0)
	, mFaceFrame(0)
	, mEyesFrame(0)
	, mEyelashFrame(0)
	, mBoneMatrices(0)
	, mNumBoneMatricesMax(0)
	, mFMEManager(0)
{
	for (size_t i = 0; i < (size_t)ACTION_NUM; ++i)
	{
		mAnimController[i] = 0;
	}
}


Model::~Model()
{

}


bool Model::Init(IFaceMotionEmulator* manager)
{
	assert(!mInit);
	if (mInit)
		return true;

	if (!RecreateShader())
		return false;

	const std::wstring fileDir = L"Data\\Model\\";

	// load animation model
	for (size_t i = 0; i < (size_t)ACTION_NUM; ++i)
	{
		std::wstring filePath = fileDir + GetFacialFileName(ActionUnit(i));
		if (!LoadXFile(filePath, &(mAnimController[i])))
			continue;
	}

	// load neutral model
	if (!LoadXFile(fileDir + Model::NEUTRAL_FACE_FILE_NAME, 0))
		return false;

	if (!InitNeutralFrameInfo(mRootFrame))
		return false;

	if (!InitMaterials())
		return false;

	mFMEManager = manager;
	mInit = true;

	return true;
}


bool Model::InitMaterials()
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);

	mMaterials[0].mTechniqueName = "solid";
	mMaterials[1].mTechniqueName = "eyeball";
	mMaterials[2].mTechniqueName = "translucent";

	// Texture objects are in managed pool, so it is safe if device lost happens
	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Face_D.tga", &mMaterials[0].mDiffuseTexture));
	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Face_N.tga", &mMaterials[0].mNormalTexture));
	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Face_S.tga", &mMaterials[0].mSpecularTexture));
	_VERY_RET(D3DXCreateCubeTextureFromFile(device, L"Data\\Model\\Environ.dds", &mMaterials[0].mEnvironTexture));

	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Eyes_D.tga", &mMaterials[1].mDiffuseTexture));
	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Eyes_N.tga", &mMaterials[1].mNormalTexture));
	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Eyes_S.tga", &mMaterials[1].mSpecularTexture));
	//_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Eyes_Mask.tga", &mMaterials[1].mMaskTexture));
	_VERY_RET(D3DXCreateCubeTextureFromFile(device, L"Data\\Model\\Environ.dds", &mMaterials[1].mEnvironTexture));

	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Eyelash_D.tga", &mMaterials[2].mDiffuseTexture));
	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Eyelash_N.tga", &mMaterials[2].mNormalTexture));
	_VERY_RET(D3DXCreateTextureFromFile(device, L"Data\\Model\\Eyelash_S.tga", &mMaterials[2].mSpecularTexture));
	_VERY_RET(D3DXCreateCubeTextureFromFile(device, L"Data\\Model\\Environ.dds", &mMaterials[2].mEnvironTexture));

	return true;
}


bool Model::InitNeutralFrameInfo(D3DXFRAME_EXT* frame)
{
	if (!frame)
		return false;

	// we need to init SRTTransform for the first time
	frame->mLocalTransform.fromMatrix(frame->TransformationMatrix);

	// record frame pointer
	const std::string name = frame->Name;
	mFrameObjMap[name] = frame;

	// record frame transformation
	SRTTransform trans(frame->TransformationMatrix);
	mNeutralFrameTransMap[name] = trans;

	// all children
	if (frame->pFrameFirstChild)
	{
		if (!InitNeutralFrameInfo((D3DXFRAME_EXT*)frame->pFrameFirstChild))
			return false;
	}

	// all siblings
	if (frame->pFrameSibling)
	{
		if (!InitNeutralFrameInfo((D3DXFRAME_EXT*)frame->pFrameSibling))
			return false;
	}

	return true;
}


void Model::Destroy()
{
	assert(mInit);
	if (!mInit)
		return;

	SAFE_RELEASE(mEffect);

	DestroyModel();

	DestroyMaterials();

	for (size_t i = 0; i < (size_t)ACTION_NUM; ++i)
	{
		SAFE_RELEASE(mAnimController[i]);
	}

	SAFE_DELETE_ARRAY(mBoneMatrices);

	mInit = false;
}


void Model::DestroyMaterials()
{
	for (size_t i = 0; i < MATERIAL_NUMBER; ++i)
	{
		mMaterials[i].Destroy();
	}
}


void Model::PreUpdate(double time, float elapsedTime)
{
	if (!mInit)
		return;

	if(!mFMEManager->QueryFaceBoneTransforms(this))	
		return;

	if(mFMEManager->QueryFaceOrientation(mPoseAngles))
		Root_Face_Root();
}


void Model::Update(double time, float elapsedTime)
{
	if (!mInit)
		return;

	Sample* app = Sample::GetInstance();
	if (!app)
		return;

	if (!mRootFrame)
		return;

	UpdateFrameMatrices(mRootFrame, &app->GetWorldMatrix());
}


void Model::PostUpdate(double time, float elapsedTime)
{
	if (!mInit)
		return;

	// we can add logic that happens after update
}


bool Model::RecreateShader()
{
	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);

	DWORD shaderFlags = D3DXFX_NOT_CLONEABLE;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DXSHADER_DEBUG;
#endif
	ID3DXEffect* effect = 0;
	LPD3DXBUFFER buff = 0;
	if (FAILED(D3DXCreateEffectFromFile(device, L"Data\\Shader\\Face.fx", 0, 0, shaderFlags,
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


void Model::Draw()
{
	assert(mInit);
	if (!mInit)
		return;

	Sample* app = Sample::GetInstance();
	assert(app);

	HRESULT hr;
	D3DXMATRIX viewProjMatrix;
	D3DXMatrixMultiply(&viewProjMatrix, &app->GetViewMatrix(), &app->GetProjMatrix());

	V(mEffect->SetMatrix("ViewProjMatix", &viewProjMatrix));
	V(mEffect->SetVector("EyePos", &app->GetCameraPos()));

	// draw face
	DrawFrame(mFaceFrame, mMaterials[0]);

	// draw eyes
	DrawFrame(mEyesFrame, mMaterials[1]);

	// draw eyelash
	DrawFrame(mEyelashFrame, mMaterials[2]);
}


bool Model::LoadXFile(const std::wstring& filePath, ID3DXAnimationController** animController)
{
	// release first
	DestroyModel();

	// Load the mesh
	AllocateHierarchyExt alloc(&mNumBoneMatricesMax, &mBoneMatrices);

	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);

	ID3DXAnimationController* controller = 0;

	_VERY_RET(D3DXLoadMeshHierarchyFromX(filePath.c_str(), D3DXMESH_MANAGED, device,
		&alloc, 0, (LPD3DXFRAME*)&mRootFrame, &controller));
	_VERY_RET(SetupBoneMatrixPointers(mRootFrame));
	_VERY_RET(D3DXFrameCalculateBoundingSphere(mRootFrame, &mModelSphere.mCenter, &mModelSphere.mRadius));

	// search for the meshes we want to render
	mFaceFrame = FindFrame(mRootFrame, "head");
	mEyesFrame = FindFrame(mRootFrame, "eye");
	mEyelashFrame = FindFrame(mRootFrame, "eyelash");

	assert(mFaceFrame && mEyesFrame && mEyelashFrame);
	if (!mFaceFrame || !mEyesFrame|| !mEyelashFrame)
		return false;

	if (animController)
		*animController = controller;

	return true;
}


void Model::DestroyModel()
{
	if (!mRootFrame)
		return;

	AllocateHierarchyExt alloc(&mNumBoneMatricesMax, &mBoneMatrices);
	D3DXFrameDestroy(mRootFrame, &alloc);
}


void Model::UpdateFrameMatrices(D3DXFRAME_EXT* frame, const D3DXMATRIXA16* parentMatrix)
{
	if (parentMatrix)
	{
		D3DXMATRIXA16 localTrans;
		frame->mLocalTransform.toMatrix(localTrans);

		D3DXMatrixMultiply(&frame->mCombinedTransformationMatrix, &localTrans, parentMatrix);
	}
	else
		frame->mLocalTransform.toMatrix(frame->mCombinedTransformationMatrix);

	if (frame->pFrameSibling)
	{
		UpdateFrameMatrices((D3DXFRAME_EXT*)frame->pFrameSibling, parentMatrix);
	}

	if (frame->pFrameFirstChild)
	{
		UpdateFrameMatrices((D3DXFRAME_EXT*)frame->pFrameFirstChild, &frame->mCombinedTransformationMatrix);
	}
}


void Model::HandleLostDevice()
{
	if (mEffect)
		mEffect->OnLostDevice();
}


void Model::HandleResetDevice()
{
	if (mEffect)
		mEffect->OnResetDevice();
}


void Model::SetFrameLocalTransform(const std::string& frameName, const SRTTransform& trans)
{
	assert(mInit);

	D3DXFRAME_EXT* frame = GetFrame(frameName);
	if (!frame)
		return;

	frame->mLocalTransform = trans;
}


void Model::ApplyAnimation(ActionUnit face, double time)
{
	assert(mInit);
	if (!mInit)
		return;

	ID3DXKeyframedAnimationSet* anim = GetAnimation(face);
	if (!anim)
		return;

	// for each frame, set the transform from animation
	for (FrameObjectMap::const_iterator iter = mFrameObjMap.begin();
		iter != mFrameObjMap.end(); ++iter)
	{
		const std::string& frameName = iter->first;
		if (frameName == Model::ROOT_FRAME_NAME)
			continue; // skip root frame

		// get frame's transformation from animation
		SRTTransform trans;
		GetFrameAnimTransformByTime(trans, face, frameName, time);

		// set local transformation to frame
		//SetFrameLocalTransform(frameName, trans);
		D3DXFRAME_EXT* frame = iter->second;
		assert(frame);
		frame->mLocalTransform = trans;
	}

	anim->Release();
}

void Model::DrawMeshContainer(D3DXMESHCONTAINER_EXT* meshContainer, const MaterialParam& mat)
{
	if (!meshContainer->pSkinInfo)
		return;

	HRESULT hr;
	LPD3DXBONECOMBINATION boneComb;

	UINT matrixIndex;
	UINT paletteEntry;
	D3DXMATRIXA16 matTemp;

	IDirect3DDevice9* device = DXUTGetD3D9Device();
	assert(device);

	boneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(meshContainer->mBoneCombinationBuf->GetBufferPointer());
	for (unsigned int i = 0; i < meshContainer->mNumAttributeGroups; i++)
	{
		// first calculate all the world matrices
		for (paletteEntry = 0; paletteEntry < meshContainer->mNumPaletteEntries; ++paletteEntry)
		{
			matrixIndex = boneComb[i].BoneId[paletteEntry];
			if (matrixIndex != UINT_MAX)
			{
				D3DXMatrixMultiply(&mBoneMatrices[paletteEntry], &meshContainer->mBoneOffsetMatrices[matrixIndex], 
					meshContainer->mBoneMatrixPtrs[matrixIndex]);
			}
		}

		mEffect->SetTechnique(mat.mTechniqueName.c_str());

		V(mEffect->SetMatrixArray("BoneMatrices", mBoneMatrices, meshContainer->mNumPaletteEntries));

		if (mat.mDiffuseTexture)
			V(mEffect->SetTexture("DiffuseTexture", mat.mDiffuseTexture));

		if (mat.mNormalTexture)
			V(mEffect->SetTexture("NormalTexture", mat.mNormalTexture));

		if (mat.mSpecularTexture)
			V(mEffect->SetTexture("SpecularTexture", mat.mSpecularTexture));

		if (mat.mMaskTexture)
			V(mEffect->SetTexture("MaskTexture", mat.mMaskTexture));

		if (mat.mEnvironTexture)
			V(mEffect->SetTexture("EnvironTexture", mat.mEnvironTexture));

		// Set CurNumBones to select the correct vertex shader for the number of bones
		V(mEffect->SetInt("BoneNumber", meshContainer->mNumInfl - 1));

		// Start the effect now all parameters have been updated
		unsigned int numPasses;
		V(mEffect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE));

		for (unsigned int j = 0; j < numPasses; j++)
		{
			V(mEffect->BeginPass(j));

			// draw the subset with the current world matrix palette and material state
			V(meshContainer->MeshData.pMesh->DrawSubset(i));

			V(mEffect->EndPass());
		}

		V(mEffect->End());

		V(device->SetVertexShader(0));
	}
}


void Model::DrawFrame(D3DXFRAME_EXT* frame, const MaterialParam& mat)
{
	if (!frame)
		return;

	D3DXMESHCONTAINER_EXT* meshContainer;

	meshContainer = (D3DXMESHCONTAINER_EXT*)frame->pMeshContainer;
	while (meshContainer)
	{
		DrawMeshContainer(meshContainer, mat);
		meshContainer = (D3DXMESHCONTAINER_EXT*)meshContainer->pNextMeshContainer;
	}
}


HRESULT Model::SetupBoneMatrixPointers(D3DXFRAME_EXT* frame)
{
	HRESULT hr;

	if (frame->pMeshContainer)
	{
		hr = SetupBoneMatrixPointersOnMesh((D3DXMESHCONTAINER_EXT*)frame->pMeshContainer);
		if (FAILED(hr))
			return hr;
	}

	if (frame->pFrameSibling)
	{
		hr = SetupBoneMatrixPointers((D3DXFRAME_EXT*)frame->pFrameSibling);
		if (FAILED(hr))
			return hr;
	}

	if (frame->pFrameFirstChild)
	{
		hr = SetupBoneMatrixPointers((D3DXFRAME_EXT*)frame->pFrameFirstChild);
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}


HRESULT Model::SetupBoneMatrixPointersOnMesh(D3DXMESHCONTAINER_EXT* meshContainer)
{

	if (meshContainer->pSkinInfo)
	{
		unsigned int boneNum = meshContainer->pSkinInfo->GetNumBones();

		meshContainer->mBoneMatrixPtrs = new D3DXMATRIX*[boneNum];
		if (!meshContainer->mBoneMatrixPtrs)
			return E_OUTOFMEMORY;

		for (unsigned int i = 0; i < boneNum; i++)
		{
			D3DXFRAME_EXT* frame = (D3DXFRAME_EXT*)D3DXFrameFind(mRootFrame, meshContainer->pSkinInfo->GetBoneName(i));
			if (!frame)
				return E_FAIL;

			meshContainer->mBoneMatrixPtrs[i] = &frame->mCombinedTransformationMatrix;
		}
	}

	return S_OK;
}


D3DXFRAME_EXT* Model::FindFrame(D3DXFRAME_EXT* startFrame, const std::string& frameName)
{
	assert(startFrame);
	if (!startFrame)
		return 0;

	D3DXFRAME_EXT* frame = 0;

	if (frameName == startFrame->Name)
		return startFrame;

	// search all siblings
	if (startFrame->pFrameSibling)
	{
		frame = FindFrame((D3DXFRAME_EXT*)startFrame->pFrameSibling, frameName);
		if (frame)
			return frame;
	}

	// search all children
	if (startFrame->pFrameFirstChild)
	{
		frame = FindFrame((D3DXFRAME_EXT*)startFrame->pFrameFirstChild, frameName);
		if (frame)
			return frame;
	}

	return 0;
}


bool Model::GetFrameAnimTransformByTime(SRTTransform& transform, ActionUnit face, const std::string& frameName, double time) const
{
	assert(mInit);

	ID3DXKeyframedAnimationSet* anim = GetAnimation(face);
	if (!anim)
		return false;

	unsigned int index;
	HRESULT hr = anim->GetAnimationIndexByName(frameName.c_str(), &index);
	if (FAILED(hr))
	{
		anim->Release();
		return false;
	}

	time = anim->GetPeriodicPosition(time);

	// get scale, rotation, translation for this bone from the animation
	anim->GetSRT(time, index, &transform.mScale, &transform.mRotation, &transform.mTranslation);

	anim->Release();

	return true;
}


bool Model::GetFrameAnimTransformByPercentage(SRTTransform& transform, ActionUnit face, const std::string& frameName, float percentage) const
{
	assert(mInit);

	ID3DXKeyframedAnimationSet* anim = GetAnimation(face);
	if (!anim)
		return false;

	unsigned int index;
	HRESULT hr = anim->GetAnimationIndexByName(frameName.c_str(), &index);
	if (FAILED(hr))
	{
		anim->Release();
		return false;
	}

	double totalTime = anim->GetPeriod();
	double time = totalTime * TClamp(double(percentage), 0.0, 1.0);
	//time = anim->GetPeriodicPosition(time);

	// get scale, rotation, translation for this bone from the animation
	anim->GetSRT(time, index, &transform.mScale, &transform.mRotation, &transform.mTranslation);

	anim->Release();

	return true;
}



ID3DXKeyframedAnimationSet* Model::GetAnimation(ActionUnit face) const
{
	assert(mInit);

	assert(face < ActionUnit::ACTION_NUM);
	//assert(mAnimController[face]);
	if (face >= ACTION_NUM || !mAnimController[face])
		return 0;

	ID3DXKeyframedAnimationSet* animSet;
	mAnimController[face]->GetAnimationSet(0, (ID3DXAnimationSet**)&animSet);

	// NOTE: the receiver needs to release it!
	return animSet;
}


double Model::GetAnimationLength(ActionUnit face) const
{
	assert(mInit);

	ID3DXKeyframedAnimationSet* anim = GetAnimation(face);
	if (!anim)
		return 0.0;

	double period =  anim->GetPeriod();

	anim->Release();
	
	return period;
}


bool Model::GetFrameNeutralTransform(SRTTransform& transform, const std::string& frameName) const
{
	assert(mInit);

	FrameTransformMap::const_iterator iter = mNeutralFrameTransMap.find(frameName);
	if (iter == mNeutralFrameTransMap.end())
		return false;

	transform = iter->second;

	return true;
}


void Model::MaterialParam::Destroy()
{
	SAFE_RELEASE(mDiffuseTexture);
	SAFE_RELEASE(mNormalTexture);
	SAFE_RELEASE(mSpecularTexture);
	SAFE_RELEASE(mMaskTexture);
	SAFE_RELEASE(mEnvironTexture);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model::Root_Face_Root()
{
	D3DXQUATERNION extRot;
	D3DXQuaternionRotationYawPitchRoll(&extRot, D3DXToRadian(mPoseAngles.yaw), D3DXToRadian(mPoseAngles.pitch), D3DXToRadian(mPoseAngles.roll));
	D3DXFRAME_EXT* frame = GetFrame("Face_Root");
	D3DXQuaternionMultiply(&frame->mLocalTransform.mRotation, &frame->mLocalTransform.mRotation, &extRot);
}

bool Model::GetCurrentActionBoneTransforms(FME::ActionUnit inActionName, float inWeight, FME::FaceBoneTransformMap& outCurrentActionBones)
{
	if(!mAnimController[inActionName]) 
		return false; 

	for (FrameObjectMap::const_iterator iter = mFrameObjMap.begin(); iter != mFrameObjMap.end(); ++iter)
	{
		const std::string frameName = iter->first;

		if (frameName == Model::ROOT_FRAME_NAME)
			continue; // skip root frame

		SRTTransform trans;
		if (!GetFrameAnimTransformByPercentage(trans, inActionName, frameName, inWeight)) 
			continue;
		outCurrentActionBones[iter->first] = trans.ToFaceBoneTransform();
	}
	return true;
}


bool Model::GetNeutralFaceBoneTransforms(FME::FaceBoneTransformMap& outNeutralFaceBones)
{
	// for each frame
	bool ret = false;
	for (FrameObjectMap::const_iterator iter = mFrameObjMap.begin(); iter != mFrameObjMap.end(); ++iter)
	{
		const std::string frameName = iter->first;

		if (frameName == Model::ROOT_FRAME_NAME)
			continue; // skip root frame
		
		SRTTransform trans;
		if (!GetFrameNeutralTransform(trans, frameName)) continue;
		outNeutralFaceBones[iter->first] = trans.ToFaceBoneTransform();
		ret = true;
	}
	return ret;
}

bool Model::SetCombinedFaceBoneTransforms(FME::FaceBoneTransformMap& inCombinedFaceBones)
{
	// for each frame
	for (FrameObjectMap::const_iterator iter = mFrameObjMap.begin(); iter != mFrameObjMap.end(); ++iter)
	{
		const std::string frameName = iter->first;
		D3DXFRAME_EXT* frame = iter->second;
		assert(frame);

		if (frameName == Model::ROOT_FRAME_NAME)
			continue; // skip root frame

		FaceBoneTransformMap::iterator it = inCombinedFaceBones.find(frameName);
		if( it != inCombinedFaceBones.end() )
			frame->mLocalTransform = SRTTransform(it->second);
	}
	return true;
}