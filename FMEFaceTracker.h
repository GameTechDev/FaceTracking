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

#include "pxcsensemanager.h"
#include "pxcfaceconfiguration.h"
#include "FMECommon.h"

namespace FME{

#define RESOLUTION_X 640
#define RESOLUTION_Y 480

class FaceTrackingAlertHandler : public PXCFaceConfiguration::AlertHandler
{
public:
	FaceTrackingAlertHandler(){};
	virtual void PXCAPI OnFiredAlert(const PXCFaceData::AlertData *alertData);
};

class FMEFaceTracker
{
public:
	typedef std::vector<PXCCapture::DeviceInfo> Devices;

public:
	FMEFaceTracker();
	bool Initialize(pxcCHAR* deviceName, 
					pxcCHAR* fileName, 
					bool isRecord, 
					bool isMirror,
					bool is3DTracking,
					bool requireBoundingRect, 
					bool requireLandmark, 
					bool requirePose, 
					bool requireExpression,
					bool requireRecognition);
	void Cleanup();
	~FMEFaceTracker();
	void Tick();
	void QueryRawFace(RawFaceData &rawFaceData);

	void QueryDevices(Devices &devices);
	bool QueryBoundingRect(PXCRectI32 &outBoundingRect);

	bool QueryLandmarks(PXCLandmarkArray	&outLandmarkArray);
	bool QueryPoseAngles(PXCFaceData::PoseEulerAngles &outPoseAngles);
	bool QueryFacePose(PXCPoint3DF32  &headCenter, pxcF64 rotation[9]);
	bool QueryExpression(PXCFaceData::ExpressionsData::FaceExpression expression, PXCFaceData::ExpressionsData::FaceExpressionResult &expressionResult);
	PXCImage* GetColorImage(){ return m_colorImage; };

private:
	PXCSession*			m_session;	
	PXCSenseManager*	m_senseManager;
	PXCFaceConfiguration* m_config;
	FaceTrackingAlertHandler *m_alertHandler;

	PXCFaceData*	m_output;		
	PXCImage*		m_colorImage;	

	pxcI16		m_oldThreshold;
	pxcI32		m_oldFilterOption;
	pxcI32		m_oldRangeTradeoff;

};

}//namespace