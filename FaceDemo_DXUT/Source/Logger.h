/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
