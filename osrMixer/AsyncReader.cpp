/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR loop system
**********************************************************
* AsyncReader.cpp
* Acync audio reader implementation
*********************************************************/
#include "stdafx.h"
#include "AsyncReader.h"

OSRSample*
AsyncReader::FirstInit(
	LPLOOP_INFO LoopInfo,
	u32 BufferSize, 
	u32 SampleRate, 
	u8 Channels
)
{
	CurrentSample = new OSRSample(
		LoopInfo->waveFormat.wBitsPerSample,
		LoopInfo->waveFormat.nChannels,
		BufferSize,		// FrameSize * Channels
		LoopInfo->waveFormat.nSamplesPerSec
	);

	CurrentSample->LoadSample(
		(void*)LoopInfo->pSample,
		BufferSize,
		LoopInfo->waveFormat.wBitsPerSample,
		LoopInfo->waveFormat.nChannels,
		LoopInfo->waveFormat.nSamplesPerSec
	);

	SamplesArray[0] = CurrentSample;

	return CurrentSample;
}

OSRSample* 
AsyncReader::RenderAudio(
	DWORD SampleNumber
)
{
	DWORD SampleNum = SampleNumber;

	if (SampleNum > 127)
	{
		for (size_t i = 0; i < 127; i++)
		{
			if (SamplesArray[i])
			{
				delete SamplesArray[i];
				SamplesArray[i] = nullptr;
			}
		}

		SamplesArray[0] = SamplesArray[127];
		SampleNum = 0;
	}

	SamplesArray[SampleNum + 1] = CurrentSample->OnBufferEnd(&loopInfo);
	SampleNum++;
	CurrentSample = SamplesArray[SampleNum];

	return CurrentSample;
}

