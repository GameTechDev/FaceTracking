/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or non-disclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2014 Intel Corporation. All Rights Reserved.

*******************************************************************************/
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


