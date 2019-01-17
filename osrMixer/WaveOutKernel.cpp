/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR MME
**********************************************************
* WaveOutKernel.cpp
* MME kernel implementation
*********************************************************/
#include "stdafx.h"
#include "WaveOutHeader.h"

static DWORD defaultSampleRateSearchOrder[] =
{ 44100, 48000, 32000, 24000, 22050, 88200, 96000, 192000, 16000, 12000, 11025, 9600, 8000 };

DLL_API HANDLE hMThreadExitEvent = NULL;
DLL_API HANDLE hMThreadLoadSamplesEvent = NULL;
OSRSample* SamplesArray[128] = { nullptr };

VOID
WINAPIV
WaveOutThreadProc(LPVOID pData)
{
	if (!hMThreadExitEvent) hMThreadExitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
	if (!hMThreadLoadSamplesEvent) hMThreadLoadSamplesEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

	MMRESULT mRes = 0;
	WAVOUT_SAMPLE_PROC* pProc = reinterpret_cast<WAVOUT_SAMPLE_PROC*>(pData);
	HANDLE hHandlesArray[] = { hMThreadExitEvent, hMThreadLoadSamplesEvent };
	HWAVEOUT hOut = pProc->pEngine->GetOutputHandle();
	WAVEHDR AudioHeader = pProc->pEngine->GetOutputHeader();
	BOOL isPlaying = TRUE;
	BOOL isFirstSample = TRUE;
	DWORD BufferSize = pProc->pEngine->GetOutputBufferSize();
	DWORD SampleNumber = 0;
	OSRSample* Sample = nullptr;
	HANDLE hO = pProc->pEngine->GetOutputHandle();

	Sample = pProc->pSample;

	if (!Sample && pProc->pLoopInfo)
	{
		Sample = new OSRSample(
			pProc->pLoopInfo->waveFormat.wBitsPerSample,
			pProc->pLoopInfo->waveFormat.nChannels,
			BufferSize,
			pProc->pLoopInfo->waveFormat.nSamplesPerSec
		);

		Sample->LoadSample(
			(void*)pProc->pLoopInfo->pSample,
			BufferSize,
			pProc->pLoopInfo->waveFormat.wBitsPerSample,
			pProc->pLoopInfo->waveFormat.nChannels,
			pProc->pLoopInfo->waveFormat.nSamplesPerSec
		);

		SamplesArray[0] = Sample;
	}
	else if (Sample && !pProc->pLoopInfo)
	{
		// play only this sample. No more
	}
	else if (Sample && pProc->pLoopInfo)
	{
		// do nothing
	}
	else
	{
		THROW1(L"Can't play audio because no loop info");
	}

	while (isPlaying)
	{
		DWORD WaitObject = WaitForMultipleObjects(2, hHandlesArray, FALSE, INFINITY);
		
		switch (WaitObject)
		{
		case WAIT_OBJECT_0:
			isPlaying = FALSE;
			break;
		case WAIT_OBJECT_0 + 1:
		{
			if (isFirstSample)
			{
				Sample->ConvertToPlay((VOID*)AudioHeader.lpData, 16);

				isFirstSample = FALSE;
			}

			//WaitForSingleObject(hO, INFINITE);
			mRes = waveOutWrite(hOut, &AudioHeader, sizeof(WAVEHDR));

			if (SampleNumber > 127)
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
				SampleNumber = 0;
			}

			if (pProc->pEngine->pTaskValue)
			{
				pProc->pEngine->pTaskValue->SetValue(Sample->SamplePosition, pProc->pLoopInfo->SampleSize);
			}

			SamplesArray[SampleNumber + 1] = Sample->OnBufferEnd(pProc->pLoopInfo);
			SampleNumber++;
			Sample = SamplesArray[SampleNumber];

			if (pProc->pEngine->pHost)
			{
				ProcessAudio(
					Sample->pOutputBuffer,
					Sample->pOutputBuffer,
					Sample->SampleRateOutput,
					Sample->BufferSizeOutput,
					Sample->ChannelsOutput,
					(IWin32VSTHost*)pProc->pEngine->pHost
				);
			}
			
			Sample->ConvertToPlay(AudioHeader.lpData, 16);
		} 
		}
	}
}

OSRCODE
MMEngine::DestroyDevice()
{
	if (hInput)
	{
		waveInClose(hInput);
	}

	if (hOutput)
	{
		waveOutClose(hOutput);
	}

	try
	{
		if (InputDevicesInfo)
		{
			for (size_t i = 0; i < (InDevices + 1); i++)
			{
				FREEKERNELHEAP(InputDevicesInfo[i]);
			}
		}

		if (OutputDevicesInfo)
		{
			for (size_t i = 0; i < (OutDevices + 1); i++)
			{
				FREEKERNELHEAP(OutputDevicesInfo[i]);
			}
		}

		FREEKERNELHEAP(InputDevicesInfo);
		FREEKERNELHEAP(OutputDevicesInfo);
	}
	catch (...)
	{
		return FS_OSR_BAD_PTR;
	}

	return OSR_SUCCESS;
}

OSRCODE
MMEngine::CloseDevice()
{
	// close output with input and unload buffers
	{
		if (hInput)
		{
			waveInClose(hInput);
			waveInUnprepareHeader(hInput, &InputHeader, sizeof(WAVEHDR));
		}

		if (hOutput)
		{
			waveOutClose(hOutput);
			waveOutUnprepareHeader(hOutput, &OutputHeader, sizeof(WAVEHDR));
		}
	}
	return OSR_SUCCESS;
}

OSRCODE
MMEngine::GetAudioDevicesInfo()
{
	double DefaultLatency = 0.2;
	double MaxLatency = 0.4;
	DWORD InDevicePrefFlag = 0;
	DWORD OutDevicePrefFlag = 0;
	LPSTR DeviceName = nullptr;
	MMRESULT mResult = 0;
	WAVEINCAPS DefaultWIC = { 0 };
	WAVEOUTCAPS DefaultWOC = { 0 };

	DefaultOutputDeviceId = DWORD(-1);
	DefaultInputDeviceId = DWORD(-1);

	// get info abot default device
	waveInMessage((HWAVEIN)WAVE_MAPPER, 0x2000 + 21, (ULONG_PTR)&DefaultInputDeviceId, (ULONG_PTR)&InDevicePrefFlag);
	waveOutMessage((HWAVEOUT)WAVE_MAPPER, 0x2000 + 21, (ULONG_PTR)&DefaultOutputDeviceId, (ULONG_PTR)&OutDevicePrefFlag);

	// get number of all devices
	InDevices = waveInGetNumDevs();
	OutDevices = waveOutGetNumDevs();

	MaxDevices = InDevices + OutDevices;

	// allocate device info structs
	if (InDevices)
	{
		MaxDevices++;

		if (!InputDevicesInfo)
		{
			InputDevicesInfo = (WAVOUT_DEVICE_INFO**)FastAlloc(sizeof(WAVOUT_DEVICE_INFO*) * (InDevices + 1));
		}
	}

	if (OutDevices)
	{
		MaxDevices++;

		if (!OutputDevicesInfo)
		{
			OutputDevicesInfo = (WAVOUT_DEVICE_INFO**)FastAlloc(sizeof(WAVOUT_DEVICE_INFO*) * (OutDevices + 1));
		}
	}

	for (size_t i = 0; i < (InDevices + 1); i++)
	{
		if (!InputDevicesInfo[i])
		{
			InputDevicesInfo[i] = (WAVOUT_DEVICE_INFO*)FastAlloc(sizeof(WAVOUT_DEVICE_INFO));
		}
	}

	for (size_t i = 0; i < (OutDevices + 1); i++)
	{
		if (!OutputDevicesInfo[i])
		{
			OutputDevicesInfo[i] = (WAVOUT_DEVICE_INFO*)FastAlloc(sizeof(WAVOUT_DEVICE_INFO));
		}
	}


	// get input device info
	for (int i = -1; i < InDevices; i++)
	{
		DWORD CurrentDeviceId = (i == -1) ? WAVE_MAPPER : i;
		WAVEINCAPSW capsA = { 0 };
		WAVEFORMATEX waveFormat = { 0 };

		// get caps for input
		mResult = waveInGetDevCapsW(CurrentDeviceId, &capsA, sizeof(WAVEINCAPSW));

		InputDevicesInfo[i + 1]->DefaultDevicePeriod = DefaultLatency;
		InputDevicesInfo[i + 1]->dwState = 0;
		InputDevicesInfo[i + 1]->MinimumDevicePeriod = DefaultLatency;

		{
			WAVEFORMATEX wf = { 0 };

			// detect true sample rate of device
			for (size_t u : defaultSampleRateSearchOrder)
			{
				MMRESULT res = 0;
				memset(&wf, 0, sizeof(WAVEFORMATEX));
				wf.cbSize = sizeof(WAVEFORMATEX);
				wf.nSamplesPerSec = u;
				wf.nChannels = capsA.wChannels;

				// detect true waveformat of device
				wf.wBitsPerSample = 32;
				wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
				wf.nAvgBytesPerSec = (wf.nSamplesPerSec * wf.nChannels * wf.wBitsPerSample) / 8;
				wf.wFormatTag = 3;

				res = waveInOpen(nullptr, CurrentDeviceId, &wf, 0, 0, WAVE_FORMAT_QUERY);

				if (res == WAVERR_BADFORMAT)
				{
					for (size_t i = 1; i <= 4; i++)
					{
						wf.wBitsPerSample = i * 8;
						wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
						wf.nAvgBytesPerSec = (wf.nSamplesPerSec * wf.nChannels * wf.wBitsPerSample) / 8;
						wf.wFormatTag = (wf.wBitsPerSample == 32) ? 3 : 1;

						res = waveInOpen(nullptr, CurrentDeviceId, &wf, 0, 0, WAVE_FORMAT_QUERY);

						if (res == MMSYSERR_NOERROR) { break; }
					}
				}

				if (res == MMSYSERR_NOERROR) { break; }
			}

			// copy true waveformat to input device info
			memcpy(&InputDevicesInfo[i + 1]->waveFormat, &wf, sizeof(WAVEFORMATEX));
		}

		memcpy(InputDevicesInfo[i + 1]->szDeviceId, capsA.szPname, 64);
	}

	// get output info
	for (int i = -1; i < OutDevices; i++)
	{
		DWORD CurrentDeviceId = (i == -1) ? WAVE_MAPPER : i;
		WAVEOUTCAPSW capsA = { 0 };
		WAVEFORMATEX wAaveFormat = { 0 };

		// get caps for output
		mResult = waveOutGetDevCapsW(CurrentDeviceId, &capsA, sizeof(WAVEOUTCAPSW));

		OutputDevicesInfo[i + 1]->DefaultDevicePeriod = DefaultLatency;
		OutputDevicesInfo[i + 1]->dwState = 0;
		OutputDevicesInfo[i + 1]->MinimumDevicePeriod = DefaultLatency;

		{
			WAVEFORMATEX wf = { 0 };

			// detect true sample rate of device
			for (size_t u : defaultSampleRateSearchOrder)
			{
				MMRESULT res = 0;
				memset(&wf, 0, sizeof(WAVEFORMATEX));
				wf.cbSize = sizeof(WAVEFORMATEX);
				wf.nSamplesPerSec = u;
				wf.nChannels = capsA.wChannels;

				// detect true waveformat of device
				wf.wBitsPerSample = 32;
				wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
				wf.nAvgBytesPerSec = (wf.nSamplesPerSec * wf.nChannels * wf.wBitsPerSample) / 8;
				wf.wFormatTag = 3;

				res = waveOutOpen(nullptr, CurrentDeviceId, &wf, 0, 0, WAVE_FORMAT_QUERY);

				if (res == WAVERR_BADFORMAT)
				{
					for (size_t o = 1; o <= 4; o++)
					{
						wf.wBitsPerSample = o * 8;
						wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
						wf.nAvgBytesPerSec = (wf.nSamplesPerSec * wf.nChannels * wf.wBitsPerSample) / 8;
						wf.wFormatTag = (wf.wBitsPerSample == 32) ? 3 : 1;

						res = waveOutOpen(nullptr, CurrentDeviceId, &wf, 0, 0, WAVE_FORMAT_QUERY);

						if (res == MMSYSERR_NOERROR) { break; }
					}
				}

				if (res == MMSYSERR_NOERROR) { break; }
			}

			// copy true waveformat to input device info
			memcpy(&OutputDevicesInfo[i + 1]->waveFormat, &wf, sizeof(WAVEFORMATEX));
		}

		memcpy(OutputDevicesInfo[i + 1]->szDeviceId, capsA.szPname, 64);
	}

	return OSR_SUCCESS;
}

OSRCODE
MMEngine::InitEngine(
	HWND hwnd
)
{
	pTaskValue = new TaskbarValue(hwnd);
	return GetAudioDevicesInfo();
}

OSRCODE
MMEngine::CreateDefaultDevice(
	LONGLONG TimeInMsec
)
{
	if (!TimeInMsec) { return MXR_OSR_UNSUPPORTED_FMT; }

	// get buffer size by milliseconds. Example: 44100 / 1000 -> 44.1 * 100 milliseconds = 4410 Samples
	float PrepareBufSize = float(OutputDevicesInfo[0]->waveFormat.nSamplesPerSec) / 1000.0f;
	OutputBufferSize = PrepareBufSize * (DWORD)TimeInMsec;
	MMRESULT mmRes = 0;

	// create event for output
	hBufferEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);

	// open default device 
	mmRes = waveOutOpen(&hOutput, WAVE_MAPPER, &OutputDevicesInfo[0]->waveFormat, (ULONG_PTR)hBufferEvent, 0, CALLBACK_EVENT);

	// if 32 float format is not support - try to use lower integer bits
	if (mmRes != MMSYSERR_NOERROR)
	{
		if (mmRes != WAVERR_BADFORMAT) 
		{
			return MXR_OSR_NO_OUT;
		}
		else
		{
			OutputDevicesInfo[0]->waveFormat.wFormatTag = 1;
			OutputDevicesInfo[0]->waveFormat.wBitsPerSample = 16;
			OutputDevicesInfo[0]->waveFormat.nBlockAlign = OutputDevicesInfo[0]->waveFormat.nChannels *
				OutputDevicesInfo[0]->waveFormat.wBitsPerSample / 8;

			OutputDevicesInfo[0]->waveFormat.nAvgBytesPerSec = (OutputDevicesInfo[0]->waveFormat.nSamplesPerSec *
				OutputDevicesInfo[0]->waveFormat.nChannels * OutputDevicesInfo[0]->waveFormat.wBitsPerSample) / 8;

			mmRes = waveOutOpen(&hOutput, WAVE_MAPPER, &OutputDevicesInfo[0]->waveFormat, (ULONG_PTR)hBufferEvent, 0, CALLBACK_EVENT);

			if (mmRes == WAVERR_BADFORMAT)
			{
				OutputDevicesInfo[0]->waveFormat.wBitsPerSample = 24;
				OutputDevicesInfo[0]->waveFormat.nBlockAlign = OutputDevicesInfo[0]->waveFormat.nChannels *
					OutputDevicesInfo[0]->waveFormat.wBitsPerSample / 8;

				OutputDevicesInfo[0]->waveFormat.nAvgBytesPerSec = (OutputDevicesInfo[0]->waveFormat.nSamplesPerSec *
					OutputDevicesInfo[0]->waveFormat.nChannels * OutputDevicesInfo[0]->waveFormat.wBitsPerSample) / 8;

				mmRes = waveOutOpen(&hOutput, WAVE_MAPPER, &OutputDevicesInfo[0]->waveFormat, (ULONG_PTR)hBufferEvent, 0, CALLBACK_EVENT);

				if (mmRes != MMSYSERR_NOERROR)
				{ 
					return MXR_OSR_NO_OUT; 
				}
			}
		}
	}

	// allocate first buffer (second buffer used for input signal)
	if (!pFirstBuffer) { pFirstBuffer = (LPBYTE)FastAlloc(sizeof(f32) * OutputBufferSize); }

	OutputHeader.dwBufferLength = sizeof(f32) * OutputBufferSize;
	OutputHeader.lpData = (LPSTR)pFirstBuffer;
	
	// prepare header for keep a reference to driver memory pages
	waveOutPrepareHeader(hOutput, &OutputHeader, sizeof(WAVEHDR));

	return OSR_SUCCESS;
}

ThreadSystem threadss;

OSRCODE
MMEngine::StartDevice(
	LPVOID pProc
)
{
	MMRESULT mRes = 0;
	WAVOUT_SAMPLE_PROC* pData = reinterpret_cast<WAVOUT_SAMPLE_PROC*>(pProc);

	if (hOutput)
	{
		mRes = waveOutPause(hOutput);
		mRes = waveOutRestart(hOutput);
	}

	// create new thread for MME worker
	static const wchar_t* WasapiString = L"OSR MME worker thread";
	WaveOutThreadId = threadss.CreateUserThread(nullptr, (ThreadFunc*)(WaveOutThreadProc), (LPVOID)pProc, WasapiString);

	// set playing event
	if (hMThreadExitEvent) { ResetEvent(hMThreadExitEvent); }
	if (hMThreadLoadSamplesEvent) { ResetEvent(hMThreadLoadSamplesEvent); }
	SetEvent(hMThreadLoadSamplesEvent);

	return OSR_SUCCESS;
}
