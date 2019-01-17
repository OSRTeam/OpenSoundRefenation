/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* SoundFile.h
* libsndfile decoder implementation
*********************************************************/
#pragma once
#define ENABLE_SNDFILE_WINDOWS_PROTOTYPES
#include "DecoderInterface.h"
#include "sndfile.hh"
#include "OSR.h"

#define BUFFER_SIZE 4096

class DLL_API ISndFileDecoder : public IDecoderInterface
{
public:
	void DecodeFile(const char* PathToFileUTF8, void*& pOutFile, size_t& OutSize, WAVE_EX& waveFormat, FILE_TYPE& fileType) override;
	void EncodeFile(const char* OutPath, void* pFile, size_t FileSize, WAVE_EX waveFormat, FILE_TYPE& fileType) override;
	void GetDecoderString(STRING128& OutString) override;

	void Release() override
	{

	}

	IObject* CloneObject() override
	{
		return nullptr;
	}
private:
	SF_INFO fileInfo;
	SNDFILE* sndFile;
};
