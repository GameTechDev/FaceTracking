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
#include "pxcfacemodule.h"
#include "pxcfacedata.h"
#include "FMEFaceTracker.h"

using namespace FME;
/************************************************************************************************************************
					FaceTrackingAlertHandler Functions
*************************************************************************************************************************/

void FaceTrackingAlertHandler::OnFiredAlert(const PXCFaceData::AlertData *alertData)
{
	switch(alertData->label)
	{
	case PXCFaceData::AlertData::ALERT_NEW_FACE_DETECTED:
		LogStatus(0,L"ALERT_NEW_FACE_DETECTED");
		break;
	case PXCFaceData::AlertData::ALERT_FACE_OUT_OF_FOV:
		LogStatus(0,L"ALERT_FACE_OUT_OF_FOV");
		break;
	case PXCFaceData::AlertData::ALERT_FACE_BACK_TO_FOV:
		LogStatus(0,L"ALERT_FACE_BACK_TO_FOV");
		break;
	case PXCFaceData::AlertData::ALERT_FACE_OCCLUDED:
		LogStatus(0,L"ALERT_FACE_OCCLUDED");
		break;
	case PXCFaceData::AlertData::ALERT_FACE_NO_LONGER_OCCLUDED:
		LogStatus(0,L"ALERT_FACE_NO_LONGER_OCCLUDED");
		break;
	case PXCFaceData::AlertData::ALERT_FACE_LOST:
		LogStatus(0,L"ALERT_FACE_LOST");
		break;
	default:
		LogStatus(0,L"UNKNOWN_ALERT");
		break;
	}
};

/************************************************************************************************************************
				FMEFaceTracker FaceTrackingAlertHandler
*************************************************************************************************************************/
FMEFaceTracker::FMEFaceTracker():
	m_output(NULL),
	m_colorImage(NULL),
	m_session(NULL),
	m_senseManager(NULL),
	m_config(NULL),
	m_alertHandler(NULL)
{
	m_session = PXCSession_Create();
	if (m_session == NULL) 
	{
		//assert(m_session); 
		LogStatus(0,L"Failed to create an SDK session");
		return;
	}

	m_alertHandler = new FaceTrackingAlertHandler();
};

FMEFaceTracker::~FMEFaceTracker()
{
	Cleanup();
	if(m_session != NULL) m_session->Release();
	if(m_alertHandler != NULL) delete m_alertHandler;
}

bool FMEFaceTracker::Initialize(
	pxcCHAR* deviceName, 
	pxcCHAR* fileName, 
	bool isRecord, 
	bool isMirror,
	bool is3DTracking,
	bool requireBoundingRect, 
	bool requireLandmark, 
	bool requirePose, 
	bool requireExpression,
	bool requireRecognition)
{
	if (m_session == NULL) 
		return false;

	m_senseManager = m_session->CreateSenseManager();
	if (m_senseManager == NULL) 
	{
		LogStatus(0,L"Failed to create an SDK SenseManager");
		return false;
	}

	/* Set Mode & Source */
	PXCCaptureManager* captureManager = m_senseManager->QueryCaptureManager();
	//pxcCHAR* device = NULL;

	pxcStatus status = PXC_STATUS_NO_ERROR;
	if (isRecord && fileName) // record
	{
		status = captureManager->SetFileName(fileName, true);
		captureManager->FilterByDeviceInfo(deviceName, 0, 0);
	} 
	else if ( (!isRecord) && fileName )	// playback
	{
		status = captureManager->SetFileName(fileName, false);
		captureManager->SetRealtime(false);
	} 
	else 
	{
		captureManager->FilterByDeviceInfo(deviceName, 0, 0);
	}

	if (status < PXC_STATUS_NO_ERROR) 
	{
		LogStatus(0,L"Failed to Set Record/Playback File");
		return false;
	}

	/* Set Face Analysis Module */
	m_senseManager->EnableFace();

	/* Initialize */
	LogStatus(0,L"Init Started");
	PXCFaceModule* faceModule = m_senseManager->QueryFace();
	if (faceModule == NULL)
	{
		//assert(faceModule); 
		LogStatus(0,L"Failed to get Face Module");
		return false;
	}
	m_config = faceModule->CreateActiveConfiguration();
	if (m_config == NULL)
	{
		//assert(m_config);
		LogStatus(0,L"Failed to create Configuration");
		return false;
	}

	if(is3DTracking)
		m_config->SetTrackingMode( PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR_PLUS_DEPTH );
	else
		m_config->SetTrackingMode( PXCFaceConfiguration::TrackingModeType::FACE_MODE_COLOR);

	m_config->ApplyChanges();

	// Try default resolution first
	PXCCapture::Device::StreamProfileSet set;
	memset(&set, 0, sizeof(set));
	PXCRangeF32 frameRate = {0, 0};
	set.color.frameRate = frameRate;
	set.color.imageInfo.height = RESOLUTION_Y;
	set.color.imageInfo.width = RESOLUTION_X;
	set.color.imageInfo.format = PXCImage::PIXEL_FORMAT_YUY2;		
	captureManager->FilterByStreamProfiles(&set);

	if (m_senseManager->Init() < PXC_STATUS_NO_ERROR)
	{
		captureManager->FilterByStreamProfiles(NULL);
		if (m_senseManager->Init() < PXC_STATUS_NO_ERROR)
		{
			LogStatus(0,L"Init Failed");
			return false;
		}
	}

	//setup device properties
	PXCCapture::DeviceInfo deviceInfo;
	PXCCapture::DeviceModel model = PXCCapture::DEVICE_MODEL_IVCAM;
	if (captureManager->QueryDevice() != NULL)
	{
		captureManager->QueryDevice()->QueryDeviceInfo(&deviceInfo);
		model = deviceInfo.model;
	}

	//save the original params of capture, they will be restored during the cleanup phrase.
	m_oldThreshold = captureManager->QueryDevice()->QueryDepthConfidenceThreshold();
	m_oldFilterOption =  captureManager->QueryDevice()->QueryIVCAMFilterOption();
	m_oldRangeTradeoff = captureManager->QueryDevice()->QueryIVCAMMotionRangeTradeOff();

	if (model == PXCCapture::DEVICE_MODEL_IVCAM)
	{
		captureManager->QueryDevice()->SetDepthConfidenceThreshold(1);
		captureManager->QueryDevice()->SetIVCAMFilterOption(6);
		captureManager->QueryDevice()->SetIVCAMMotionRangeTradeOff(21);
		if(isMirror)
		{
			captureManager->QueryDevice()->SetMirrorMode(PXCCapture::Device::MIRROR_MODE_HORIZONTAL);
		}
		else
		{

			captureManager->QueryDevice()->SetMirrorMode(PXCCapture::Device::MIRROR_MODE_DISABLED);
		}
	}

	// CheckForDepthStream
	if (m_config->GetTrackingMode() == PXCFaceConfiguration::FACE_MODE_COLOR_PLUS_DEPTH)
	{
		PXCCapture::Device::StreamProfileSet profiles={};
		captureManager->QueryDevice()->QueryStreamProfileSet(&profiles);
		if (!profiles.depth.imageInfo.format)
		{         
			LogStatus(0,L"Depth stream is not supported for device.Using 2D tracking");        
		}
	}

	// configure face module
	m_config->detection.isEnabled = requireBoundingRect;
	m_config->landmarks.isEnabled = requireLandmark;
	m_config->pose.isEnabled = requirePose;

	if (requireExpression)
	{
		m_config->QueryExpressions()->Enable();
		m_config->QueryExpressions()->EnableAllExpressions();
	}
	else
	{
		m_config->QueryExpressions()->DisableAllExpressions();
		m_config->QueryExpressions()->Disable();
	}
	if (requireRecognition)
	{
		m_config->QueryRecognition()->Enable();
	}

	m_config->QueryExpressions()->properties.maxTrackedFaces = 1;	
	if(m_alertHandler != NULL)
	{
		m_config->EnableAllAlerts();
		m_config->SubscribeAlert(m_alertHandler);
	}
	m_config->ApplyChanges();	

	//LogStatus(L"Streaming");
	/* get output buffer of face module */
	m_output = faceModule->CreateOutput();

	return true;
}

void FMEFaceTracker::Cleanup()
{
	if(m_output) m_output->Release();
	if(m_colorImage) m_colorImage->Release();

	if(!m_senseManager)return;
	PXCCaptureManager* captureManager = m_senseManager->QueryCaptureManager();

	PXCCapture::DeviceInfo deviceInfo;
	PXCCapture::DeviceModel model = PXCCapture::DEVICE_MODEL_IVCAM;
	if (captureManager->QueryDevice() != NULL)
	{
		captureManager->QueryDevice()->QueryDeviceInfo(&deviceInfo);
		model = deviceInfo.model;
	}
	if (model == PXCCapture::DEVICE_MODEL_IVCAM)
	{
		captureManager->QueryDevice()->SetDepthConfidenceThreshold(m_oldThreshold);
		captureManager->QueryDevice()->SetIVCAMFilterOption(m_oldFilterOption);
		captureManager->QueryDevice()->SetIVCAMMotionRangeTradeOff(m_oldRangeTradeoff);
	}

	m_config->Release();
	m_senseManager->Close(); 
	m_senseManager->Release();
}


void FMEFaceTracker::Tick()
{
	if( !m_senseManager) return;
	if ( m_senseManager->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
	{ 
		//LOG: ERROR: Failed to AcquireFrame 
		LogStatus(0,L"Failed to AcquireFrame");
		return;
	}

	// update face data. after this step, the m_output can be used after the frame is released.
	m_output->Update();	

	// update image data. 
	PXCCapture::Sample* sample = m_senseManager->QueryFaceSample();	
	if (sample != NULL)
	{
		// save the color image 
		sample->color->AddRef();
		if ( m_colorImage != NULL ) m_colorImage->Release();
		m_colorImage = sample->color;
	}	

	m_senseManager->ReleaseFrame();
}

bool FMEFaceTracker::QueryBoundingRect(PXCRectI32 &outBoundingRect)
{
	PXCFaceData::Face* trackedFace = m_output->QueryFaceByIndex(0); // get the closest face.
	if(trackedFace)
	{
		const PXCFaceData::DetectionData* detectionData = trackedFace->QueryDetection();
		if (detectionData == NULL) return false;
		return detectionData->QueryBoundingRect(&outBoundingRect);
	}
	else
		return false;
}

bool FMEFaceTracker::QueryLandmarks(PXCLandmarkArray &outLandmarkArray)
{
	if(!m_output) return false;
	PXCFaceData::Face* trackedFace = m_output->QueryFaceByIndex(0); // get the closest face.
	if(trackedFace)
	{
		const PXCFaceData::LandmarksData* landmarkData = trackedFace->QueryLandmarks();
		if (landmarkData == NULL) return false;
		pxcI32 numPoints = landmarkData->QueryNumPoints();
		if( outLandmarkArray.size() < numPoints )
			outLandmarkArray.resize(numPoints);
		return landmarkData->QueryPoints(&outLandmarkArray[0]);
	}
	else
		return false;
}

bool FMEFaceTracker::QueryFacePose(PXCPoint3DF32  &headCenter, pxcF64 rotation[9])
{
	if(!m_output) return false;
	const PXCFaceData::PoseData* poseData = m_output->QueryFaceByIndex(0)->QueryPose();
	if (poseData == NULL) return false;
	PXCFaceData::HeadPosition outHead;
	if(poseData->QueryHeadPosition(&outHead) && poseData->QueryRotationMatrix(rotation))
	{
		headCenter = outHead.headCenter;
		return true;
	}
	else 
		return false;
}

void FMEFaceTracker::QueryRawFace(RawFaceData &rawFaceData)
{
	rawFaceData.hasLandmarks = QueryLandmarks(rawFaceData.landmarks);
	rawFaceData.hasPoseAngles = QueryPoseAngles(rawFaceData.poseAngles);
}

bool FMEFaceTracker::QueryPoseAngles(PXCFaceData::PoseEulerAngles &outPoseAngles)
{
	if(!m_output) return false;
	PXCFaceData::Face* face = m_output->QueryFaceByIndex(0);
	if(!face) return false;
	const PXCFaceData::PoseData* poseData = face->QueryPose();
	if (!poseData) return false;
	return poseData->QueryPoseAngles(&outPoseAngles);
}

bool FMEFaceTracker::QueryExpression(PXCFaceData::ExpressionsData::FaceExpression expression, PXCFaceData::ExpressionsData::FaceExpressionResult &expressionResult)
{
	if(!m_output) return false;
	PXCFaceData::Face* trackedFace = m_output->QueryFaceByIndex(0);
	if(!trackedFace)return false;
	PXCFaceData::ExpressionsData* expressionsData = trackedFace->QueryExpressions();
	if (!expressionsData) return false;
	return expressionsData->QueryExpression(expression, &expressionResult);
}

void FMEFaceTracker::QueryDevices(Devices &devices)
{
	PXCSession::ImplDesc desc;
	memset(&desc, 0, sizeof(desc)); 
	desc.group = PXCSession::IMPL_GROUP_SENSOR;
	desc.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;
	
	devices.clear();

	for (int i = 0; ; ++i)
	{
		PXCSession::ImplDesc desc1;
		if (m_session->QueryImpl(&desc, i, &desc1) < PXC_STATUS_NO_ERROR)
			break;

		PXCCapture *capture;
		if (m_session->CreateImpl<PXCCapture>(&desc1, &capture) < PXC_STATUS_NO_ERROR) 
			continue;

		for (int j = 0; ; ++j) {
			PXCCapture::DeviceInfo deviceInfo;
			if (capture->QueryDeviceInfo(j, &deviceInfo) < PXC_STATUS_NO_ERROR) 
				break;
			else
				devices.push_back(deviceInfo);
		}
		capture->Release();
	}
}
