/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>
#include "IFaceME.h"

namespace FME{

class FMEFaceTracker;
class FMEExprParser;
class FMEExprSynthesizer;

class FMEManager: public IFaceMotionEmulator
{
public:
	FMEManager();
	~FMEManager();

	virtual bool QueryDevices(std::vector<std::wstring>& deviceNames);
	virtual bool Init(EMULATOR_CONFIG config);

	virtual void Reset();
	virtual void Release(void);

	virtual void Tick(double fTime, float fElapsedTime);
	virtual bool QueryLandmarks(LandmarkArray& landmarks);
	virtual bool QueryBGRAImage(BGRAImage& image);
	virtual bool QueryActionUnitWeights(ActionUnitWeightMap& weights);
	virtual bool QueryFaceOrientation(FaceOrientation &orientation);
	virtual bool QueryExpressionConficence(float& confidence);

	virtual bool QueryFaceBoneTransforms(IFaceBoneModel* model);
	virtual void RegisterModels(std::vector<IFaceBoneModel*>* models);

protected:
	bool QueryIVCam(std::wstring& deviceName);

private:
	FMEFaceTracker*		m_faceTracker;
	FMEExprParser*	m_exprParser;
	FMEExprSynthesizer*  m_exprSynthesizer;
	bool m_init;
};

}//namespace

