/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* sndfileOpen.cpp
* libsndfile decoder implementation
*********************************************************/
#include "stdafx.h"

void
ISndFileDecoder::DecodeFile(const char* PathToFileUTF8, void*& pOutFile, size_t& OutSize, WAVE_EX& waveFormat, FILE_TYPE& fileType)
{
	WSTRING_PATH szPath = { 0 };

	if (MultiByteToWideChar(CP_UTF8, 0, PathToFileUTF8, strlen(PathToFileUTF8) + 1, szPath, sizeof(WSTRING_PATH)))
	{
		sndFile = sf_wchar_open(szPath, SFM_READ, &fileInfo);
		if (!sndFile) { return; }

		waveFormat.nChannels = fileInfo.channels;
		waveFormat.nSamplesPerSec = fileInfo.samplerate;
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nAvgBytesPerSec = ((waveFormat.nSamplesPerSec * 32 * waveFormat.nChannels) / 8);
		waveFormat.wBitsPerSample = 32;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample * waveFormat.nChannels) / 8;
		waveFormat.wFormatTag = 3;

		switch (HIWORD(fileInfo.format))
		{
			case SF_FORMAT_WAV:
				fileType = WAV_TYPE;
				break;
			case SF_FORMAT_AIFF:
				fileType = AIFF_TYPE;
				break;
			case SF_FORMAT_AU:
				fileType = UNKNOWN_TYPE;
				break;
			case SF_FORMAT_RAW:
				fileType = RAW_TYPE;
				break;
			case SF_FORMAT_W64:
				fileType = WAV_TYPE;
				break;
			case SF_FORMAT_MAT4:
				fileType = UNKNOWN_TYPE;
				break;
			case SF_FORMAT_MAT5:
				fileType = UNKNOWN_TYPE;
				break;
			case SF_FORMAT_PVF:
				fileType = UNKNOWN_TYPE;
				break;
			case SF_FORMAT_WAVEX:
				fileType = WAV_TYPE;
				break;
			case SF_FORMAT_FLAC:
				fileType = FLAC_TYPE;
				break;
			case SF_FORMAT_OGG:
				fileType = OGG_TYPE;
				break;
			case SF_FORMAT_RF64:
				fileType = WAV_TYPE;
				break;
		default:
			break;
		}

		OutSize = fileInfo.frames * fileInfo.channels * sizeof(f32);
		pOutFile = FastAlloc(OutSize);

		sf_readf_float(sndFile, (f32*)(pOutFile), fileInfo.frames);

		sf_close(sndFile);
		sndFile = nullptr;
	}
}

void 
ISndFileDecoder::EncodeFile(const char* OutPath, void* pFile, size_t FileSize, WAVE_EX waveFormat, FILE_TYPE& fileType)
{
	
}

void
ISndFileDecoder::GetDecoderString(STRING128& OutString)
{
	memcpy(OutString, u8"libsndfile", strlen(u8"libsndfile"));
}

void
ISndFileDecoder::Release()
{
	if (sndFile) { sf_close(sndFile); }
	delete this;
}