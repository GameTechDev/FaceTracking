/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Common.h"
#include "DXExtension.h"
#include "Math.h"
#include "..\..\IFaceME.h"

/// Model class
class Model: public FME::IFaceBoneModel
{
public:
	bool GetCurrentActionBoneTransforms(FME::ActionUnit inActionName, float inWeight, FME::FaceBoneTransformMap& outCurrentActionBones);
	bool GetNeutralFaceBoneTransforms(FME::FaceBoneTransformMap& outNeutralFaceBones);
	bool SetCombinedFaceBoneTransforms(FME::FaceBoneTransformMap& inCombinedFaceBones);

public:
	/// name of root frame
	static const std::string ROOT_FRAME_NAME;
	/// name of neutral face file (with .X)
	static const std::wstring NEUTRAL_FACE_FILE_NAME;
	/// Get .X file name by ActionUnit
	const std::wstring GetFacialFileName(FME::ActionUnit action);

public:
	/// Constructor
	Model();
	/// Destructor
	~Model();
	/// Initialization
	bool Init(FME::IFaceMotionEmulator* manager);
	/// Destroy
	void Destroy();
	/// Called before Update
	void PreUpdate(double time, float elapsedTime);
	/// Update
	void Update(double time, float elapsedTime);
	/// Called after Update
	void PostUpdate(double time, float elapsedTime);
	/// Initialize shader
	bool RecreateShader();
	/// Draw the model
	void Draw();
	/// Should be called when device lost
	void HandleLostDevice();
	/// Should be called when device reset
	void HandleResetDevice();
	/// Get bounding sphere
	const BoundingSphere& GetBoundingSphere() const;
	/// Get frame by name, if not found, return 0
	D3DXFRAME_EXT* GetFrame(const std::string& frameName) const;
	/// Get frame transformation from face animation by time
	bool GetFrameAnimTransformByTime(SRTTransform& transform, FME::ActionUnit face, const std::string& frameName, double time) const;
	/// Get frame offset transformation from face animation by time, comparing to neutral pose
	bool GetFrameAnimOffsetTransformByTime(SRTTransform& transform, FME::ActionUnit face, const std::string& frameName, double time) const;
	/// Get frame transformation from face animation by percentage
	bool GetFrameAnimTransformByPercentage(SRTTransform& transform, FME::ActionUnit face, const std::string& frameName, float percentage) const;
	/// Get frame transformation from face animation by percentage, comparing to neutral pose
	bool GetFrameAnimOffsetTransformByPercentage(SRTTransform& transform, FME::ActionUnit face, const std::string& frameName, float percentage) const;
	/// Get length from face animation by facial action type
	double GetAnimationLength(FME::ActionUnit face) const;
	/// Get animation by facial action type, NOTE: do free the pointer after use
	ID3DXKeyframedAnimationSet* GetAnimation(FME::ActionUnit face) const;
	/// Get frame neutral transformation
	bool GetFrameNeutralTransform(SRTTransform& transform, const std::string& frameName) const;
	/// Set local transformation to the frame, if frame not found, do nothing
	void SetFrameLocalTransform(const std::string& frameName, const SRTTransform& trans);
	/// used to iterate the frame hierarchy, recursively, with callback function called on each node
	void visitFrames(bool(*visitMethod)(D3DXFRAME_EXT* frame));
	/// used to iterate the frame hierarchy, recursively, with member callback function called on each node
	template <class T> 
	void visitFrames(T* instance, bool (T::*visitMethod)(D3DXFRAME_EXT* frame));

private:
	/// Material number used in this model
	static const unsigned int MATERIAL_NUMBER;
	/// Material context struct
	struct MaterialParam
	{
		std::string mTechniqueName;
		IDirect3DTexture9* mDiffuseTexture;
		IDirect3DTexture9* mNormalTexture;
		IDirect3DTexture9* mSpecularTexture;
		IDirect3DTexture9* mMaskTexture;
		IDirect3DCubeTexture9 * mEnvironTexture;

		/// Constructor
		MaterialParam() 
			: mDiffuseTexture(0)
			, mNormalTexture(0)
			, mSpecularTexture(0)
			, mMaskTexture(0)
			, mEnvironTexture(0)
		{
		}

		/// Destroy
		void Destroy();

	private:
		MaterialParam(MaterialParam& rhs) {}
	};

	/// used to iterate the frame hierarchy, recursively
	void visitFrame(D3DXFRAME_EXT* frame, bool(*visitMethod)(D3DXFRAME_EXT* frame));
	/// used to iterate the frame hierarchy, recursively
	template <class T>
	void visitFrame(D3DXFRAME_EXT* frame, T* instance, bool (T::*visitMethod)(D3DXFRAME_EXT* frame));
	/// Initialize materials
	bool InitMaterials();
	/// Initialize frame info
	bool InitNeutralFrameInfo(D3DXFRAME_EXT* frame);
	/// Destroy materials
	void DestroyMaterials();
	/// Load .X file
	bool LoadXFile(const std::wstring& filePath, ID3DXAnimationController** animController);
	/// Release model related resources
	void DestroyModel();
	/// Apply animation to current model
	void ApplyAnimation(FME::ActionUnit face, double time);
	/// Draw mesh container
	void DrawMeshContainer(D3DXMESHCONTAINER_EXT* meshContainer, const MaterialParam& mat);
	/// Draw frame
	void DrawFrame(D3DXFRAME_EXT* frame, const MaterialParam& mat);
	/// Update frame matrices from the model
	void UpdateFrameMatrices(D3DXFRAME_EXT* frame, const D3DXMATRIXA16* parentMatrix);
	/// setup bone matrix pointers
	HRESULT SetupBoneMatrixPointers(D3DXFRAME_EXT* frame);
	/// setup bone matrix pointers on mesh
	HRESULT SetupBoneMatrixPointersOnMesh(D3DXMESHCONTAINER_EXT* meshContainer);
	/// search for frame, recursively on frame hierarchy, slow
	D3DXFRAME_EXT* FindFrame(D3DXFRAME_EXT* startFrame, const std::string& frameName);

protected:
	void Root_Face_Root();

private:
	typedef std::map<std::string, D3DXFRAME_EXT*> FrameObjectMap;
	typedef std::map<std::string, SRTTransform> FrameTransformMap;

	bool mInit;
	ID3DXEffect* mEffect;
	ID3DXAnimationController* mAnimController[FME::ACTION_NUM];
	D3DXFRAME_EXT* mRootFrame;
	D3DXFRAME_EXT* mFaceFrame;
	D3DXFRAME_EXT* mEyesFrame;
	D3DXFRAME_EXT* mEyelashFrame;
	D3DXMATRIXA16* mBoneMatrices;
	unsigned int mNumBoneMatricesMax;
	BoundingSphere mModelSphere;
	MaterialParam mMaterials[3];
	FrameObjectMap mFrameObjMap;
	FrameTransformMap mNeutralFrameTransMap;

	FME::IFaceMotionEmulator*	mFMEManager;
	FME::ActionUnitWeightMap	mActionUnitWeights;
	FME::FaceOrientation		mPoseAngles;
};


inline 
const BoundingSphere& Model::GetBoundingSphere() const
{
	return mModelSphere;
}


inline
D3DXFRAME_EXT* Model::GetFrame(const std::string& frameName) const
{
	assert(mInit);

	FrameObjectMap::const_iterator iter = mFrameObjMap.find(frameName);
	if (iter == mFrameObjMap.end())
		return 0;

	return iter->second;
}


inline
void Model::visitFrames(bool (*visitMethod)(D3DXFRAME_EXT* frame))
{
	if (!mRootFrame)
		return;

	visitFrame(mRootFrame, visitMethod);
}


inline
void Model::visitFrame(D3DXFRAME_EXT* frame, bool (*visitMethod)(D3DXFRAME_EXT*))
{
	if (!(*visitMethod)(frame))
		return;

	// all siblings
	if (frame->pFrameSibling)
	{
		visitFrame((D3DXFRAME_EXT*)frame->pFrameSibling, visitMethod);
	}

	// all children
	if (frame->pFrameFirstChild)
	{
		visitFrame((D3DXFRAME_EXT*)frame->pFrameFirstChild, visitMethod);
	}
}


template <class T>
void Model::visitFrames(T* instance, bool (T::*visitMethod)(D3DXFRAME_EXT* frame))
{
	if (!mRootFrame)
		return;

	visitFrame(mRootFrame, instance, visitMethod);
}


template <class T>
void Model::visitFrame(D3DXFRAME_EXT* frame, T* instance, bool (T::*visitMethod)(D3DXFRAME_EXT*))
{
	if (!(instance->*visitMethod)(frame))
		return;

	// all siblings
	if (frame->pFrameSibling)
	{
		visitFrame((D3DXFRAME_EXT*)frame->pFrameSibling, instance, visitMethod);
	}

	// all children
	if (frame->pFrameFirstChild)
	{
		visitFrame((D3DXFRAME_EXT*)frame->pFrameFirstChild, instance, visitMethod);
	}
}