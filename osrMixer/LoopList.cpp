/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR loop system
**********************************************************
* LoopList.cpp
* loop system kernel implementation
*********************************************************/
#include "stdafx.h"
#include <vector>
#pragma hdrstop

DLL_API AVReader ffReader;
DLL_API WMFReader mfReader;

LoopList::LoopList() : 
	dwMaxCountOfFiles(NULL),
	dwCurrentCountOfFiles(NULL),
	lpFileInfo(NULL)
{
	dwMaxCountOfFiles = 100;
	lpFileInfo = (LPAUDIO_INFO)AdvanceAlloc(dwMaxCountOfFiles * sizeof(AUDIO_INFO), NULL);
	ASSERT2(lpFileInfo, L"Can't alloc struct");
}

LoopList::LoopList(
	DWORD dwCountOfFiles
) :
	dwCurrentCountOfFiles(NULL),
	lpFileInfo(NULL)
{
	dwMaxCountOfFiles = dwCountOfFiles;
	lpFileInfo = (LPAUDIO_INFO)AdvanceAlloc(dwMaxCountOfFiles * sizeof(AUDIO_INFO), NULL);
	ASSERT2(lpFileInfo, L"Can't alloc struct");
}

LoopList::~LoopList()
{
	FREEPROCESSHEAP(lpFileInfo);
}

VOID
LoopList::LoadAudioFile(
	LPCWSTR lpFilePath,
	DWORD dwDecoderType,
	DWORD dwFormat,				// only for FFmpeg
	LPDWORD pSampleNumber
)
{	
	std::vector<BYTE> audioData;	
	WAVEFORMATEX* waveFormat = nullptr;
	WSTRING_PATH szString = { NULL };
	DWORD dwHeaderSize = NULL;
	OSRCODE sCode = OSR_SUCCESS;

#ifdef WIN32
	IOSRFileSystem* pFileSystem = new IOSRWin32FileSystem();
	IOSRWin32FileSystem* pWin32FileSystem = reinterpret_cast<IOSRWin32FileSystem*>(pFileSystem);
#else
	IOSRFileSystem* pFileSystem = new IOSRPosixFileSystem();
#endif

	if (dwCurrentCountOfFiles == dwMaxCountOfFiles) 
	{
		dwCurrentCountOfFiles = 0;
	}

	lpFileInfo[dwCurrentCountOfFiles].SampleCount = NULL;
	lpFileInfo[dwCurrentCountOfFiles].pSampleInfo = (LPLOOP_INFO)AdvanceAlloc(sizeof(LOOP_INFO), NULL);

	lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->StructSize = sizeof(LOOP_INFO);
	lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleNumber = NULL;

	switch (dwDecoderType)
	{
	case USE_FFMPEG:
	{
		WORD Bits = 0;
		WORD Channels = 0;
		DWORD SampleRate = 0;
		// open file		
		//ffReader.OpenFileToBuffer(lpFilePath, (LPCWSTR*)&szString, (LPDWORD)&lpFileInfo[dwCurrentCountOfFiles].FileSize, 2, &SampleRate, &Channels, &Bits);

		pWin32FileSystem->OpenFileToByteStreamW(
			szString, 
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->pSample,
			(size_t&)lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleSize
		);

		lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.cbSize = sizeof(WAVEFORMATEX);
		lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.nChannels = Channels;
		lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.nSamplesPerSec = SampleRate;

		if (!Bits)
		{
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.wBitsPerSample = 32;
		}
		else
		{
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.wBitsPerSample = Bits;
		}

		if (Bits >= 32)
		{
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.wFormatTag = 3;
		}
		else
		{
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.wFormatTag = 1;
		}

		lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.nBlockAlign = Channels * Bits / 8;
		lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat.nAvgBytesPerSec = ((SampleRate * Bits * Channels) / 8);

		if (waveFormat)
		{
			size_t Ant = (waveFormat->nSamplesPerSec + waveFormat->wBitsPerSample + waveFormat->nChannels);

			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleDuration = (lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleSize) / Ant;
		}
	}
		break;
	case USE_LIBSNDFILE:
	{
		ISndFileDecoder* pDecoder = new ISndFileDecoder();
		STRING_PATH szPath2 = { 0 };
		WAVE_EX wfex = { };
		memset(&wfex, 0, sizeof(WAVE_EX));

		FILE_TYPE ftype = UNKNOWN_TYPE;
		WideCharToMultiByte(CP_UTF8, 0, lpFilePath, 260, szPath2, 260, nullptr, nullptr);
		pDecoder->DecodeFile(szPath2, (void*&)lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->pSample, (size_t&)lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleSize, wfex, ftype);
		Sleep(0);
	}
	case USE_WMF:
		if (mfReader.IsSupportedByMWF(lpFilePath, &waveFormat))
		{
			// load file to vector and copy to local buffer
			mfReader.LoadFileToMediaBuffer(audioData, &waveFormat);
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->pSample = (u8*)AdvanceAlloc(audioData.size() + 1, NULL);

			memcpy((VOID*)lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->pSample, &audioData[0], audioData.size());

			// set data to structs
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleSize = (DWORD)(audioData.size() + 1);
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleDuration = mfReader.uDuration;
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleNumber = dwCurrentCountOfFiles;
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat = *waveFormat;
			lpFileInfo[dwCurrentCountOfFiles].FileSize = (DWORD)(audioData.size());
			lpFileInfo[dwCurrentCountOfFiles].SampleCount = NULL;

			// free COM-pointer
			CoTaskMemFree(waveFormat);
			break;
		}
	case NULL:
	default:
		pWin32FileSystem->OpenFileToByteStreamW(
			szString,
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->pSample,
			(size_t&)lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleSize
		);
	
		sCode = GetWaveFormatExtented(
			(BYTE*)lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->pSample,
			lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleSize,
			&lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->waveFormat
		);

		if (OSRFAILED(sCode))
		{
			FreePointer((VOID*)lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->pSample, NULL, NULL);
			return;
		}

		lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleDuration =
			(lpFileInfo[dwCurrentCountOfFiles].pSampleInfo->SampleSize - dwHeaderSize) /
			(waveFormat->nSamplesPerSec + waveFormat->wBitsPerSample + waveFormat->nChannels);

		break;
	}

	*pSampleNumber = dwCurrentCountOfFiles;
	dwCurrentCountOfFiles++;
}

