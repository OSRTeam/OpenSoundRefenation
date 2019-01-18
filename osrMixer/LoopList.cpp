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

void
IWin32SoundList::load(const char* pathUTF8, i32& FileNumber)
{
	if (CurrentCountOfFiles >= MaxCountOfFiles) { return; }

	if (CurrentCountOfFiles == -1)
	{
		CurrentCountOfFiles = 1;
	}
	else
	{
		CurrentCountOfFiles++;
	}

	lpFileInfo[CurrentCountOfFiles - 1].SampleCount = NULL;
	lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo = (LPLOOP_INFO)AdvanceAlloc(sizeof(LOOP_INFO), NULL);

	lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->StructSize = sizeof(LOOP_INFO);
	lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->SampleNumber = NULL;

	FILE_TYPE fType = pFileSystem->IsAudio(pathUTF8);
	IDecoderInterface* pDecoder = nullptr;

	switch (fType)
	{
	case MP3_TYPE:
		pDecoder = new IMFDecoder();
		break;
	case AAC_TYPE:
		if (CurrentCountOfFiles == 1)
		{
			CurrentCountOfFiles = -1;
		}
		else
		{
			CurrentCountOfFiles--;
		}

		FREEKERNELHEAP(lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo);
		return;		// not implemented now
	default:
		pDecoder = new ISndFileDecoder();
		break;
	}

	pDecoder->DecodeFile(
		pathUTF8,
		*(void**)&(lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->pSample),
		*(size_t*)(&lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->SampleSize),
		*(WAVE_EX*)(&lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->waveFormat),
		fType
	); 
	
	if (!lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->pSample)
	{
		if (CurrentCountOfFiles == 1)
		{
			CurrentCountOfFiles = -1;
		}
		else
		{
			CurrentCountOfFiles--;
		}

		FREEKERNELHEAP(lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo);
		return;
	}

	lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->SampleDuration = 
		(lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->SampleSize) /
		(lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->waveFormat.nSamplesPerSec + 
			lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->waveFormat.wBitsPerSample + 
			lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->waveFormat.nChannels
			);

	lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->SampleNumber = CurrentCountOfFiles - 1;
	lpFileInfo[CurrentCountOfFiles - 1].FileSize = lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->SampleSize;
	lpFileInfo[CurrentCountOfFiles - 1].FileDuration = lpFileInfo[CurrentCountOfFiles - 1].pSampleInfo->SampleDuration;
	lpFileInfo[CurrentCountOfFiles - 1].fType = fType;

	FileNumber = CurrentCountOfFiles - 1;
	_RELEASE(pDecoder);
}

void 
IWin32SoundList::unload(i32 FileNumber)
{

}
