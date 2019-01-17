/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR MME
**********************************************************
* WaveOutHeader.h
* MME kernel header
*********************************************************/
#pragma once
#include "OSR.h"
#include <mmsystem.h>
#include "OSRVST.h"
#include "AsyncReader.h"

typedef struct mmedevice_info
{
	WSTRING512 szDeviceId;
	DWORD dwState;
	double DefaultDevicePeriod;
	double MinimumDevicePeriod;
	WAVEFORMATEX waveFormat;
	LONG loopBack;

} WAVOUT_DEVICE_INFO, *PWAVOUT_DEVICE_INFO, *LPWAVOUT_DEVICE_INFO;

inline
f64
MReftimeToMillis(
	LONGLONG ref
)
{
	return ((double)ref)*0.0001;
}

inline
LONGLONG
MMillisToReftime(
	f64 ref
)
{
	return (LONGLONG)(ref / 0.0001);
}

class DLL_API MMEngine
{
public:
	MMEngine() : MaxDevices(0), OutDevices(0), InDevices(0), hBufferEvent(NULL), 
		hInput(NULL), hOutput(NULL), WaveOutThreadId(0), pFirstBuffer(nullptr),
		pSecondBuffer(nullptr), InputBufferSize(0), OutputBufferSize(0), pTaskValue(nullptr), pHost(nullptr),
		DefaultInputDeviceId(0), CurrentInputDeviceId(0), DefaultOutputDeviceId(0), CurrentOutputDeviceId(0),
		OutputDevicesInfo(nullptr), InputDevicesInfo(nullptr)
	{
		memset(&InputHeader, 0, sizeof(WAVEHDR));
		memset(&OutputHeader, 0, sizeof(WAVEHDR));

		memset(&outputDevice, 0, sizeof(WAVOUT_DEVICE_INFO));
		memset(&inputDevice, 0, sizeof(WAVOUT_DEVICE_INFO));
	}

	HWAVEIN GetInputHandle() { return hInput; }
	HWAVEOUT GetOutputHandle() { return hOutput; }
	HANDLE GetBufferEvent() { return hBufferEvent; }
	WAVEHDR GetInputHeader() { return InputHeader; }
	WAVEHDR GetOutputHeader() { return OutputHeader; }
	DWORD GetInputBufferSize() { return InputBufferSize; }
	DWORD GetOutputBufferSize() { return OutputBufferSize; }
	DWORD GetInputDefaultDeviceId() { return DefaultInputDeviceId; }
	DWORD GetOutputDefaultDeviceId() { return DefaultOutputDeviceId; }
	LPVOID GetFirstBuffer() { return pFirstBuffer; }
	LPVOID GetSecondBuffer() { return pSecondBuffer; }

	OSRCODE GetAudioDevicesInfo();

	OSRCODE InitEngine(HWND hwnd);

	OSRCODE CreateDefaultDevice(LONGLONG TimeInMsec);
	OSRCODE CreateInputDevice(LONGLONG TimeInMsec, int DeviceNum);
	OSRCODE CreateOutputDevice(LONGLONG TimeInMsec, int DeviceNum);
	OSRCODE CloseDevice();
	OSRCODE DestroyDevice();
	OSRCODE StartDevice(LPVOID pProc);
	OSRCODE StopDevice();

	~MMEngine()
	{
		DestroyDevice();
	}

	TaskbarValue* pTaskValue;
	IVSTHost* pHost;

private:
	int MaxDevices;
	int OutDevices;
	int InDevices;
	HWAVEIN hInput;
	HWAVEOUT hOutput;
	HANDLE hBufferEvent;
	WAVEHDR InputHeader;
	WAVEHDR OutputHeader;
	LPBYTE pFirstBuffer;
	LPBYTE pSecondBuffer;
	DWORD WaveOutThreadId;
	DWORD InputBufferSize;
	DWORD OutputBufferSize;
	DWORD DefaultInputDeviceId;
	DWORD CurrentInputDeviceId;
	DWORD DefaultOutputDeviceId;
	DWORD CurrentOutputDeviceId;
	WAVOUT_DEVICE_INFO inputDevice;
	WAVOUT_DEVICE_INFO outputDevice;
	WAVOUT_DEVICE_INFO** OutputDevicesInfo;
	WAVOUT_DEVICE_INFO** InputDevicesInfo;
};

typedef struct mmesample_proc
{
	LPLOOP_INFO pLoopInfo;
	OSRSample* pSample;
	MMEngine* pEngine;
} WAVOUT_SAMPLE_PROC, *PWAVOUT_SAMPLE_PROC, *LPWAVOUT_SAMPLE_PROC;
