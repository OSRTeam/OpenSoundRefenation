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
#include "DecoderInterface.h"	

class DLL_API IMFDecoder : public IDecoderInterface
{
public:
	IMFDecoder();

	void DecodeFile(const char* PathToFileUTF8, void*& pOutFile, size_t& OutSize, WAVE_EX& waveFormat, FILE_TYPE& fileType) override;
	void EncodeFile(const char* OutPath, void* pFile, size_t FileSize, WAVE_EX waveFormat, FILE_TYPE& fileType) override;
	void GetDecoderString(STRING128& OutString) override;

	void Release() override;

	IObject* CloneObject() override
	{
		return nullptr;		//#TODO:
	}

	IMFSourceReader* pSourceReader;
	IMFMediaType* pMediaType;
	IMFMediaType* pSecondMediaType;

	UINT64 uDuration;
	UINT64 uSamplesLength;
};
