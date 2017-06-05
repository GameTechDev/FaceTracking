/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or imlied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "FrameLogger.h"

FrameLogger::FrameLogger()
{
	m_frameNum=0;
	m_logger = GetFMELogger();
}

FrameLogger::~FrameLogger()
{
}

void FrameLogger::BeginFrame()
{
	if(!m_logger) return;

	if(m_frameNum == 0){	// print table header;
		//face angle
		//sprintf(m_s, "FrameNum\t Raw.x\t Raw.y\t Raw.z\t IntraOpt.x\t IntraOpt.y\t IntraOpt.z\t InterOpt.x\t InterOpt.y\t InterOpt.z\t Blend.x\t Blend.y\t Blend.z\t");
		//m_logger->LogFrameToFile(m_s);

		////MOUTH_OPEN
		//sprintf(m_s, "FrameNum\t Raw\t SG1stDeriv\t SG2ndDeriv\t");
		//m_logger->LogFrameToFile(m_s);

		//EYELID
		sprintf(m_s, "FrameNum\t RawLeft\t RawRight\t SmLeft\t SmRight\t DrLeft\t DrRight\t");
		m_logger->LogFrameToFile(m_s);
		
	}

	// print frameNumber;
	sprintf(m_s, "\n%d\t",m_frameNum++);
	m_logger->LogFrameToFile(m_s);
}

void FrameLogger::EndFrame()
{
	//if(!m_logger) return;

	//// print endl;
	//sprintf(m_s, "\n");
	//m_logger->LogFrameToFile(m_s);
}

void FrameLogger::LogFrameFacialActionWeightMap(FacialActionWeightMap& faws)
{
	if(!m_logger) return;

	////MOUTH_OPEN
	////sprintf(m_s, "%.2f\t",faws[MOUTH_OPEN]);
	////m_logger->LogFrameToFile(m_s);
	//sprintf(m_s, "%.2f\t%.2f\t",faws[MOUTH_OPEN],faws[NOSE_STRETCH]);
	//m_logger->LogFrameToFile(m_s);

	//EYELID
	sprintf(m_s, "%.2f\t %.2f\t",faws[EYELID_LEFT_T_CLOSE],faws[EYELID_RIGHT_T_CLOSE]);
	m_logger->LogFrameToFile(m_s);
}


void FrameLogger::LogFrameFaceAngle(FaceAngles& angle)
{
	if(!m_logger) return;

	sprintf(m_s, "%.2f\t%.2f\t%.2f\t",angle.x(), angle.y(), angle.z());
	m_logger->LogFrameToFile(m_s);
}


