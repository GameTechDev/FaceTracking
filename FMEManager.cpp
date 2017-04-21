/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "FMEFaceTracker.h"
#include "FMEExprParser.h"
#include "FMEExprSynthesizer.h"
#include "FMEManager.h"

using namespace FME;

FMEManager::FMEManager()
	:m_faceTracker(NULL),
	 m_exprParser(NULL),
	 m_exprSynthesizer(NULL),
	 m_init(false)
{
	//创建并组装pipeline，并不初始化
	m_faceTracker = new FMEFaceTracker();
	m_exprParser = new FMEExprParser(m_faceTracker);
	m_exprSynthesizer = new FMEExprSynthesizer(m_exprParser);
}

FMEManager::~FMEManager()
{
	SAFE_DELETE(m_faceTracker);
	SAFE_DELETE(m_exprParser);
	SAFE_DELETE(m_exprSynthesizer);
}

bool FMEManager::Init(EMULATOR_CONFIG config)
{
	m_init=false;
	pxcCHAR* fileName=NULL;
	pxcCHAR* deviceName=NULL;

	if(config.deviceName){
		g_deviceName = new std::wstring(*config.deviceName);
		deviceName = (pxcCHAR*)config.deviceName->c_str();
	}
	else{
		std::wstring ivcam;
		if(!QueryIVCam(ivcam))
			return false;
		g_deviceName = new std::wstring(ivcam);
		deviceName = (pxcCHAR*)g_deviceName->c_str();
	}

	if(config.fileName){
		g_fileName = new std::wstring(*config.fileName);
		fileName = (pxcCHAR*)config.fileName->c_str();
	}

	g_isRecord = config.isRecord;
	g_logFun = config.logFun;

	if( ! m_faceTracker->Initialize(deviceName, fileName, config.isRecord, true, true, false, true, true, false, false) )
		return false;
		
	m_init = true;
	return true;
}

void FMEManager::Reset()
{
	m_init=false;
	SAFE_DELETE(m_exprParser);
	m_exprParser = new FMEExprParser(m_faceTracker);
	m_init=true;

	SAFE_DELETE(m_exprSynthesizer);
	m_exprSynthesizer = new FMEExprSynthesizer(m_exprParser);
}

void FMEManager::Tick(double fTime, float fElapsedTime)
{
	// 该函数每个gameloop都调用一次，使FME pipeline中3个模块都tick一次。
	if(!m_init)return;
	m_faceTracker->Tick();
	m_exprParser->Tick(fTime, fElapsedTime);
	m_exprSynthesizer->Tick();
}

bool FMEManager::QueryIVCam(std::wstring& deviceName)
{
	FMEFaceTracker::Devices rsDevices;
	m_faceTracker->QueryDevices(rsDevices);
	for(auto deviceIter=rsDevices.begin(); deviceIter!=rsDevices.end(); deviceIter++)
	{
		//if(deviceIter->model == PXCCapture::DeviceModel::DEVICE_MODEL_IVCAM )
		if (deviceIter->model == PXCCapture::DeviceModel::DEVICE_MODEL_SR300)
		{
			deviceName.assign(deviceIter->name);
			return true;
		}
	}
	return false;
}

void FMEManager::Release()
{
	SAFE_DELETE(g_deviceName);
	SAFE_DELETE(g_fileName);
	::delete this;
}

bool FMEManager::QueryLandmarks(LandmarkArray& landmarks)
{
	PXCLandmarkArray pxcLandmarks;
	Landmark landmark;
	bool result = m_faceTracker->QueryLandmarks(pxcLandmarks);
	if(result)
	{
		landmarks.clear();
		for(auto landmarkIter=pxcLandmarks.begin(); landmarkIter!=pxcLandmarks.end(); landmarkIter++)
		{
			landmark.image.x = static_cast<float>(landmarkIter->image.x);
			landmark.image.y = static_cast<float>(landmarkIter->image.y);
			landmark.confidenceImage = static_cast<int>(landmarkIter->confidenceImage);
			landmarks.push_back(landmark);
		}
	}
	return result;
}

bool FMEManager::QueryBGRAImage(BGRAImage& image)
{
	PXCImage* colorImage = m_faceTracker->GetColorImage();
	if(!colorImage) return false;
	PXCImage::ImageInfo info = colorImage->QueryInfo();
	image.width = info.width;
	image.height = info.height;
	unsigned int bsize = info.width * info.height *4;
	if(image.buffer.size() < bsize)
		image.buffer.resize(bsize);

	PXCImage::ImageData data;
	if (colorImage->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &data) >= PXC_STATUS_NO_ERROR)
	{
		if(data.pitches[0]/4 == info.width)
		{
			memcpy(&image.buffer[0], data.planes[0], bsize);
		}
		else
		{
			int lineSize = info.width * 4;
			unsigned char * dst = &image.buffer[0];
			unsigned char * src = data.planes[0];
			for(int i=0; i<info.height; i++)
			{
				memcpy( dst, src, lineSize);
				dst += lineSize;
				src += data.pitches[0];
			}
		}
		colorImage->ReleaseAccess(&data);
		return true;
	}

	return false;
}

bool FMEManager::QueryActionUnitWeights(ActionUnitWeightMap& weights)
{
	ActionUnitWeightMap* actionUnitWeights;
	actionUnitWeights = m_exprParser->GetActionUnitWeights();
	if(actionUnitWeights){
		weights = *actionUnitWeights;
		return true;
	}
	else
		return false;
}

bool FMEManager::QueryFaceOrientation(FaceOrientation &orientation)
{
	FaceAngles* faceAngle;
	faceAngle = m_exprParser->GetFaceAngles();
	if(faceAngle){
		orientation.pitch = faceAngle->x();
		orientation.roll = faceAngle->z();
		orientation.yaw = faceAngle->y();
		return true;
	}
	else
		return false;
}

bool FMEManager::QueryExpressionConficence(float& confidence)
{
	float* conf = m_exprParser->GetExpressionConfidence();
	if(conf){
		confidence = *conf;
		return true;
	}
	else
		return false;
}


bool FMEManager::QueryDevices(std::vector<std::wstring>& deviceNames)
{
	return true;
}

bool FMEManager::QueryFaceBoneTransforms(IFaceBoneModel* model)
{
	return m_exprSynthesizer->QueryFaceBoneTransforms(model);
}

void FMEManager::RegisterModels(std::vector<IFaceBoneModel*>* models)
{
	m_exprSynthesizer->RegisterModels(models);
}

/************************************************************************************************************************
					Global Function
*************************************************************************************************************************/
IFaceMotionEmulator* FME::FMECreate()
{
	g_deviceName = NULL;
	g_fileName = NULL;
	g_isRecord = false;
	g_logFun = NULL;
	return new FMEManager();
};
