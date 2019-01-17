/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR loop system
**********************************************************
* AsyncReader.h
* Acync audio reader implementation
*********************************************************/
#pragma once
#include "OSR.h"
#include "LoopList.h"
#include "OSRVST.h"
#include "soxr\src\soxr.h"

inline
void
ProcessAudio(
	float** pInput,
	float** pOutput,
	DWORD SampleRate,
	DWORD BufferSize,
	DWORD Channels,
	IWin32VSTHost* pHost
)
{
	if (!pHost || !pHost->pEffect) { return;}

	float* pCustomOutput[16] = { nullptr };
	float* pCustomInput[16] = { nullptr };

	for (size_t i = 0; i < Channels * 2; i++)
	{
		pCustomOutput[i] = (float*)FastAlloc(BufferSize * sizeof(f32));
		pCustomInput[i] = (float*)FastAlloc(BufferSize * sizeof(f32));

#ifndef WIN32
		memset(pCustomInput[i], 0, BufferSize * sizeof(f32));
		memset(pCustomOutput[i], 0, BufferSize * sizeof(f32));
#endif
	}

	for (size_t i = 0; i < Channels; i++)
	{
		memcpy(pCustomInput[i], pInput[i], BufferSize * sizeof(f32));
	}

	// process function
	pHost->ProcessAudio(pCustomInput, pCustomOutput, BufferSize / Channels);

	for (size_t i = 0; i < Channels; i++)
	{
		memcpy(pOutput[i], pCustomOutput[i], BufferSize * sizeof(f32));
	}

	for (size_t i = 0; i < Channels * 2; i++)
	{
		FREEKERNELHEAP(pCustomOutput[i]);
		FREEKERNELHEAP(pCustomInput[i]);
	}
}

inline
void
ResampleAudio(
	f32* pData,
	size_t DataSize,
	u8 Channels,
	u32 SampleRateIn,
	u32 SampleRateOut,
	f32* pOutData
)
{
	//size_t odone, written, need_input = 1;
	//void* obuf = malloc(DataSize * sizeof(f32));
	//void* ibuf = malloc(DataSize * sizeof(f32));
	//soxr_error_t error = 0;
	//static soxr_t SOxr = soxr_create(SampleRateIn, SampleRateOut, Channels, &error, nullptr, nullptr, nullptr);

	//if (error) { return; }

	//do {
	//	//size_t ilen1 = 0;

	//	//if (need_input) 
	//	//{
	//	//	memcpy(ibuf, pData, DataSize);

	//	//	if (!DataSize) {     /* If the is no (more) input data available, */
	//	//		free(ibuf);     /* set ibuf to NULL, to indicate end-of-input */
	//	//		ibuf = NULL;    /* to the resampler. */
	//	//	}
	//	//}

	//	///* Copy data from the input buffer into the resampler, and resample
	//	// * to produce as much output as is possible to the given output buffer: */
	//	//error = soxr_process(SOxr, ibuf, DataSize, NULL, obuf, DataSize, &odone);


	//	///* If the actual amount of data output is less than that requested, and
	//	// * we have not already reached the end of the input data, then supply some
	//	// * more input next time round the loop: */
	//	//need_input = odone < olen && ibuf;

	//} while (!error && (need_input || written));

}

class DLL_API AsyncReader
{
public:
	AsyncReader()
	{
		SampleNum = 0;
		CurrentSample = nullptr;

		for (size_t i = 0; i < 128; i++)
		{
			SamplesArray[i] = nullptr;
		}

		memset(&loopInfo, 0, sizeof(LOOP_INFO));
	}

	~AsyncReader()
	{

		DWORD dwSampleN = SampleNum + 1;
		if (dwSampleN > 127) { dwSampleN = 127; }
		for (size_t i = 0; i < dwSampleN; i++)
		{
			if (SamplesArray[i])
			{
				delete SamplesArray[i];
				SamplesArray[i] = nullptr;
			}
		}
	}

	OSRSample* FirstInit(LPLOOP_INFO LoopInfo, u32 BufferSize, u32 SampleRate, u8 Channels);
	OSRSample* RenderAudio(DWORD SampleNumber);
	VOID SetNumber(DWORD SampleNumber) { SampleNum = SampleNumber; }

private: 
	DWORD SampleNum;
	OSRSample* SamplesArray[128];
	LOOP_INFO loopInfo;
	OSRSample* CurrentSample;
};
