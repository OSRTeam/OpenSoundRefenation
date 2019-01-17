/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* MFWDecoder.cpp
* WMF decoder implementation
*********************************************************/
#pragma once
#include <mfapi.h>
#include <mfidl.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include "OSR.h"
#include "KernCPU.h"

class DLL_API WMFReader
{
public:
	WMFReader() 
	{
		MFStartup(MF_VERSION);
		WMFInit(); 
	}

	~WMFReader()
	{
		//MFShutdown();
		_RELEASE(pAttribute);
	}

	VOID WMFInit();
	BOOL IsSupportedByMWF(LPCWSTR lpPath, WAVEFORMATEX** waveFormat);
	VOID LoadFileToMediaBuffer(std::vector<BYTE>& lpData, WAVEFORMATEX** waveFormat);
	VOID WriteFileFromMediaBufferEx(IMFSourceReader* pSourceReader, HANDLE hFile, std::vector<BYTE>& pData, BYTE** pSecondData, DWORD dwDataSize);

	IMFAttributes* pAttribute;
	IMFSourceReader* pSourceReader;
	IMFMediaType* pMediaType;
	IMFMediaType* pSecondMediaType;

	UINT64 uDuration;
	UINT64 uSamplesLength;
};
