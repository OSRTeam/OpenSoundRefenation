#pragma once
#include "OSR.h"

class IDecoderInterface : public IObject
{
public:
	virtual void DecodeFile(const char* PathToFileUTF8, void*& pOutFile, size_t& OutSize, WAVE_EX& waveFormat, FILE_TYPE& fileType) = 0;
	virtual void EncodeFile(const char* OutPath, void* pFile, size_t FileSize, WAVE_EX waveFormat, FILE_TYPE& fileType) = 0;
	virtual void GetDecoderString(STRING128& OutString) = 0;

	f32 CurrentPercent;
};
