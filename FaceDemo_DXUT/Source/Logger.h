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
#include <fstream>
#include "..\..\IFaceME.h"

class Logger
{
public:
	static void Create(wchar_t* fileName=NULL)
	{
		m_context.clear(); 
		if(fileName) fout.open(fileName);
	}

	static void Destroy()
	{
		if(fout.is_open()) fout.close();
	}

	static void WriteLog(int level, std::wstring& info)
	{
		switch(level){
		case 0:
			m_context.append(info); //WriteToString(info);
			break;
		case 1:
			if(fout.is_open())fout<<info.c_str(); //WriteToFile(info);
			break;
		default:
			break;
		}
	}

	static void ClearString()
	{
		m_context.clear();
	}

	static const wchar_t* DumpString()
	{
		return m_context.c_str();
	}

protected:
	static std::wstring m_context;
	static std::ofstream  fout;
};
