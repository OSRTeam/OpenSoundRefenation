/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR loop system
**********************************************************
* LoopList.h
* loop system kernel implementation
*********************************************************/
#pragma once
#include "stdafx.h"
#include "OSRSystem.h"

extern DLL_API AVReader ffReader;
extern DLL_API WMFReader mfReader;

class DLL_API LoopList
{
public:
	LoopList();
	LoopList(DWORD dwCountOfFiles);
	~LoopList();

	DWORD GetFilesCount() { return dwCurrentCountOfFiles; }
	DWORD GetMaxFilesCount() { return dwMaxCountOfFiles; }
	LPAUDIO_INFO GetLoopInfo() { return lpFileInfo; }

	VOID SetMaxFilesCount(DWORD dwFilesCount) { dwMaxCountOfFiles = dwFilesCount; }

	VOID LoadAudioFile(LPCWSTR lpFilePath, DWORD dwDecoderType, DWORD dwFormat, LPDWORD pSampleNumber);
	
private:
	DWORD dwCurrentCountOfFiles;
	DWORD dwMaxCountOfFiles;
	LPAUDIO_INFO lpFileInfo;
};
