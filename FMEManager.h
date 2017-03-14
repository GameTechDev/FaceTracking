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

