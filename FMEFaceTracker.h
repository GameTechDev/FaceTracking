/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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