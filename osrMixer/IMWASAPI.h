/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR WASAPI
**********************************************************
* IMWASAPI.cpp
* WASAPI kernel implementation
*********************************************************/
#pragma once
#include "stdafx.h"
#include <dshow.h>
#include <audiopolicy.h>
#include <AudioClient.h>
#include <wrl\implements.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include "AsyncReader.h"
#include "MixerBase.h"

class ISoundInterfaceWASAPI : public ISoundInterface
{
public:
	ISoundInterfaceWASAPI();

	ISoundInterfaceWASAPI(IMMDevice* pInputDevice1, IMMDevice* pOutputDevice1, IAudioClient* pInputClient1,
		IAudioClient* pOutputClient1, IAudioRenderClient* pRenderClient1, IAudioCaptureClient* pCaptureClient1,
		AUDIO_HOST InputHost1, AUDIO_HOST OutputHost1);

	OSRCODE EnumerateAudioInput(AUDIO_HOST** pHostsList, u32& HostCounts) override;
	OSRCODE EnumerateAudioOutputs(AUDIO_HOST** pHostsList, u32& HostCounts) override;

	OSRCODE GetHostDeviceInInfo(u32 DeviceNumber, AUDIO_HOST& HostInfo) override;
	OSRCODE GetHostDeviceOutInfo(u32 DeviceNumber, AUDIO_HOST& HostInfo) override;
	OSRCODE GetHostDevicesInCount(u32& DeviceCount) override;
	OSRCODE GetHostDevicesOutCount(u32& DeviceCount) override;
	OSRCODE GetVolumeLevel(f32& Volume) override;
	OSRCODE GetDefaultDevice(u32& DeviceNumber) override;

	OSRCODE SetDelayLevel(f64 HostDelay) override;
	OSRCODE SetVolumeLevel(f32 Volume) override;

	OSRCODE CreateRenderSoundDevice(f64 HostDelay, u32 DeviceNumber) override;
	OSRCODE CreateCaptureSoundDevice(f64 HostDelay, u32 DeviceNumber) override;
	
	OSRCODE CreateRenderDefaultSoundDevice(f64 HostDelay) override;
	OSRCODE CreateCaptureDefaultSoundDevice(f64 HostDelay) override;

	OSRCODE RestartRenderSoundDevice(f64 HostDelay, u32 DeviceNumber) override;
	OSRCODE RestartCaptureSoundDevice(f64 HostDelay, u32 DeviceNumber) override;

	OSRCODE RestartRenderDefaultSoundDevice(f64 HostDelay) override;
	OSRCODE RestartCaptureDefaultSoundDevice(f64 HostDelay) override;

	OSRCODE CloseRenderSoundDevice() override;
	OSRCODE CloseCaptureSoundDevice() override;

	OSRCODE PlayHost() override;
	OSRCODE StopHost() override;

	OSRCODE RecvPacket(void* pData, PacketType Type, size_t DataSize) override;
	OSRCODE GetPacket(void*& pData, PacketType Type, size_t& DataSize) override;

	OSRCODE GetLoadBuffer(void*& pData, size_t& BufferSize) override;

	void Release() override;

	IObject* CloneObject() override;

	IMMDevice* pInputDevice;
	IMMDevice* pOutputDevice;

	IAudioClient* pInputClient;
	IAudioClient* pOutputClient;

	IAudioRenderClient* pRenderClient;
	IAudioCaptureClient* pCaptureClient;

	LPVOID pOutputBuffer;
	size_t OutputBufferSize;

	// handles can not be copy to other objects
	HANDLE hInputStart;
	HANDLE hOutputStart;

	HANDLE hInputExit;
	HANDLE hOutputExit;
	
	HANDLE hInputLoadEvent;
	HANDLE hOutputLoadEvent;

	DWORD ThreadId;
	ThreadSystem thread;
};
