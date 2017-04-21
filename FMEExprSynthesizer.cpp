/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "FMEExprSynthesizer.h"

using namespace FME;

static FaceBoneTransform operator+(const FaceBoneTransform& fbt1, const FaceBoneTransform& fbt2)
{
		FaceBoneTransform ret;
		// blend scale
		ret.scaleVector[0] = fbt1.scaleVector[0]*fbt2.scaleVector[0];
		ret.scaleVector[1] = fbt1.scaleVector[1]*fbt2.scaleVector[1];
		ret.scaleVector[2] = fbt1.scaleVector[2]*fbt2.scaleVector[2];

		// blend rotation
		Eigen::Quaternionf r1 = Eigen::Quaternionf(fbt1.rotationQuaternion);	
		Eigen::Quaternionf r2 = Eigen::Quaternionf(fbt2.rotationQuaternion);
		Eigen::Quaternionf r = r1*r2;
		r.normalize();
		ret.rotationQuaternion[0]=r.x();
		ret.rotationQuaternion[1]=r.y();
		ret.rotationQuaternion[2]=r.z();
		ret.rotationQuaternion[3]=r.w();

		// blend translation
		ret.translationVector[0] = fbt1.translationVector[0] + fbt2.translationVector[0];	
		ret.translationVector[1] = fbt1.translationVector[1] + fbt2.translationVector[1];	
		ret.translationVector[2] = fbt1.translationVector[2] + fbt2.translationVector[2];	

		return ret;
};

static FaceBoneTransform operator-(const FaceBoneTransform& fbt1, const FaceBoneTransform& fbt2)
{
		FaceBoneTransform ret;

		// scale
		assert(fbt2.scaleVector[0] != 0.0f && fbt2.scaleVector[1] != 0.0f && fbt2.scaleVector[2] != 0.0f);
		if (fbt2.scaleVector[0] != 0.0f && fbt2.scaleVector[1] != 0.0f && fbt2.scaleVector[2] != 0.0f){
			ret.scaleVector[0] = fbt1.scaleVector[0]/fbt2.scaleVector[0];
			ret.scaleVector[1] = fbt1.scaleVector[1]/fbt2.scaleVector[1];
			ret.scaleVector[2] = fbt1.scaleVector[2]/fbt2.scaleVector[2];
		}

		// rotation
		Eigen::Quaternionf r1 = Eigen::Quaternionf(fbt1.rotationQuaternion);	
		Eigen::Quaternionf r2 = Eigen::Quaternionf(fbt2.rotationQuaternion).inverse();
		Eigen::Quaternionf r = r2*r1;
		r.normalize();
		ret.rotationQuaternion[0]=r.x();
		ret.rotationQuaternion[1]=r.y();
		ret.rotationQuaternion[2]=r.z();
		ret.rotationQuaternion[3]=r.w();

		// translation
		ret.translationVector[0] = fbt1.translationVector[0] - fbt2.translationVector[0];	
		ret.translationVector[1] = fbt1.translationVector[1] - fbt2.translationVector[1];	
		ret.translationVector[2] = fbt1.translationVector[2] - fbt2.translationVector[2];	
		
		return ret;
};

//==============================================================================================================================================================
//
//==============================================================================================================================================================

FMEExprSynthesizer::FMEExprSynthesizer(FMEExprParser* exprParser):
m_exprParser(exprParser)
{
}

FMEExprSynthesizer::~FMEExprSynthesizer()
{
	RegisterModels(NULL);
}


void FMEExprSynthesizer::Tick()
{
	ActionUnitWeightMap* ActionUnitWeights = m_exprParser->GetActionUnitWeights();
	if(!ActionUnitWeights) return;

	for(ModelBoneMap::iterator it = m_modelBoneMap.begin(); it != m_modelBoneMap.end(); it++){ //更新每个face model的BoneMap
		IFaceBoneModel* model = it->first;
		FaceBoneTransformMap& combinedFaceBoneTransforms = *it->second;

		FaceBoneTransformMap neutralFaceBoneTransforms, currentActionBoneTransforms;

		if( !model->GetNeutralFaceBoneTransforms(neutralFaceBoneTransforms) ) continue;
		combinedFaceBoneTransforms = neutralFaceBoneTransforms; 

		for(ActionUnitWeightMap::iterator it_faws = ActionUnitWeights->begin(); it_faws != ActionUnitWeights->end(); it_faws++){ //遍历每个AU
			if (it_faws->second == 0) continue;
			if( model->GetCurrentActionBoneTransforms(it_faws->first,it_faws->second,currentActionBoneTransforms) ){ //获得AU的当前骨骼矩阵。
				for(FaceBoneTransformMap::iterator it_btm = currentActionBoneTransforms.begin(); it_btm != currentActionBoneTransforms.end(); it_btm++){
					FaceBoneTransform offset = it_btm->second - neutralFaceBoneTransforms[it_btm->first];
					combinedFaceBoneTransforms[it_btm->first] = combinedFaceBoneTransforms[it_btm->first] + offset;
				}
			}
		}
	}
}

void FMEExprSynthesizer::RegisterModels(std::vector<IFaceBoneModel*>* models)
{
	for(ModelBoneMap::iterator it = m_modelBoneMap.begin(); it != m_modelBoneMap.end(); it++){
		SAFE_DELETE(it->second);
	}
	m_modelBoneMap.clear();

	if(models != NULL){
		for(int i=0; i<models->size(); i++){
			IFaceBoneModel* model = (*models)[i];
			FaceBoneTransformMap* map = new FaceBoneTransformMap;
			m_modelBoneMap.insert(std::pair<IFaceBoneModel*,FaceBoneTransformMap*>(model, map)); 
		}
	}
}

bool FMEExprSynthesizer::QueryFaceBoneTransforms(IFaceBoneModel* model)
{
	ModelBoneMap::iterator it = m_modelBoneMap.find(model);
	if( it == m_modelBoneMap.end()) return false;
	it->first->SetCombinedFaceBoneTransforms(*it->second);
	return true;
}
