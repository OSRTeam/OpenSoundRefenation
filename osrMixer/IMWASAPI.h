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
#include "OSRVST.h"
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

//
//using namespace Microsoft::WRL;
//
//#ifndef GUID_SECT
//#define GUID_SECT
//#endif
//
//#define __DEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const GUID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
//#define __DEFINE_IID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const IID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
//#define __DEFINE_CLSID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const CLSID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
//#define PA_DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//    __DEFINE_CLSID(pa_CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
//#define PA_DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//    __DEFINE_IID(pa_IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
//
//// "1CB9AD4C-DBFA-4c32-B178-C2F568A703B2"
//PA_DEFINE_IID(IAudioClient, 1cb9ad4c, dbfa, 4c32, b1, 78, c2, f5, 68, a7, 03, b2);
//// "726778CD-F60A-4EDA-82DE-E47610CD78AA"
//PA_DEFINE_IID(IAudioClient2, 726778cd, f60a, 4eda, 82, de, e4, 76, 10, cd, 78, aa);
//// "1BE09788-6894-4089-8586-9A2A6C265AC5"
//
//inline 
//f64 
//ReftimeToMillis(
//	REFERENCE_TIME ref
//)
//{
//	return ((double)ref)*0.0001;
//}
//
//inline 
//REFERENCE_TIME
//MillisToReftime(
//	f64 ref
//)
//{
//	return (REFERENCE_TIME)(ref / 0.0001);
//}
//
//inline 
//DWORD 
//GetAudioClientVersion()
//{
//	if (GetWindowsVersion() >= WIN_10_SERVER2016)
//	{
//		return 3;
//	}
//	else
//	{
//		if (GetWindowsVersion() >= WIN_8_SERVER2012)
//		{
//			return 2;
//		}
//	}
//
//	return 1;
//}
//
//const
//inline
//IID*
//GetAudioClientIID()
//{
//	static const IID *cli_iid = nullptr;
//
//	if (!cli_iid)
//	{
//		DWORD cli_version = GetAudioClientVersion();
//
//		if (cli_version <= 1)
//		{
//			cli_iid = &pa_IID_IAudioClient;
//		}
//		else
//		{
//			cli_iid = &pa_IID_IAudioClient2; 
//			cli_version = 2; 
//		}
//	}
//
//	return cli_iid;
//}
//
//typedef void(*WasapiHostProcessorCallback) (void *inputBuffer, long inputFrames,
//	void *outputBuffer, long outputFrames,
//	void *userData);
//
//typedef struct wdevice_info
//{
//	IMMDevice* device;
//	WSTRING512 szDeviceId;
//	DWORD dwState;
//	EDataFlow dataFlow;
//	REFERENCE_TIME DefaultDevicePeriod;
//	REFERENCE_TIME MinimumDevicePeriod;
//	WAVEFORMATEX waveFormat;
//	EndpointFormFactor formFactor;
//	LONG loopBack;
//
//} WASAPI_DEVICE_INFO, *PWASAPI_DEVICE_INFO, *LPWASAPI_DEVICE_INFO;
//
//typedef struct whost_processor
//{
//	WasapiHostProcessorCallback pProcessor;
//	LPVOID pUserData;
//} WASAPI_HOST_PROCESSOR;
//
//class DLL_API IMEngine
//{
//public:
//	IMEngine()
//	{ 
//		pHost = nullptr;
//		WasapiThread = 0;
//		BufferSize = 0;
//		m_MixFormat = nullptr;
//		m_SampleReadyAsyncResult = nullptr;
//		DeviceCount = 0;
//		hOutput = CreateEventW(nullptr, TRUE, FALSE, nullptr);
//		hStart = CreateEventW(nullptr, TRUE, FALSE, nullptr);
//		hExit = CreateEventW(nullptr, TRUE, FALSE, nullptr);
//
//		pRenderStream = nullptr;
//		pAudioClient = nullptr;
//		pAudioRenderClient = nullptr;
//		pOutVol = nullptr;
//		pCaptureClient = nullptr;
//		pCaptureClientParent = nullptr;
//		PCaptureClientStream = nullptr;
//		pInVol = nullptr;
//		enumerator = nullptr;
//		m_SampleReadyAsyncResult = nullptr;
//		pDeviceInfo = nullptr;
//		pTaskValue = nullptr;
//
//		memset(&OutputDeviceInfo, 0, sizeof(WASAPI_DEVICE_INFO));
//		memset(&InputDeviceInfo, 0, sizeof(WASAPI_DEVICE_INFO));
//	}
//
//	~IMEngine()
//	{
//		StopDevice();
//
//		_RELEASE(pAudioRenderClient);
//		_RELEASE(pAudioClient);
//		_RELEASE(pRenderStream);  
//		_RELEASE(pOutVol);
//
//		if (pHost)
//		{
//			pHost->SuspendPlugin();
//			pHost->ClosePluginWindow();
//			pHost->DestroyPluginWindow();
//		}
//
//		if (pTaskValue) { delete pTaskValue; }
//		if (pHost) { delete pHost; }
//	}
//
//	DWORD GetAudioThreadId() { return WasapiThread; }
//	DWORD GetBufferSize() { return BufferSize; }
//	WASAPI_DEVICE_INFO* GetOutputInfo() { return &OutputDeviceInfo; }
//	WASAPI_DEVICE_INFO* GetInputInfo() { return &InputDeviceInfo; }
//
//	OSRCODE InitEngine(HWND hwnd);
//	OSRCODE CreateDefaultDevice(REFERENCE_TIME referTime);
//	OSRCODE StartDevice(LPVOID pProc);
//	OSRCODE StopDevice();
//	OSRCODE SetAudioPosition(f32 Position);
//	OSRCODE GetAudioPosition(f32& Position);
//
//	OSRCODE RestartDevice(REFERENCE_TIME referTime)
//	{
//		StopDevice();
//		
//		_RELEASE(pAudioRenderClient);
//		_RELEASE(pAudioClient);
//		_RELEASE(pRenderStream);
//		_RELEASE(pOutVol);
//
//		return CreateDefaultDevice(referTime);
//	}
//
//	IStream*				pRenderStream;
//	IAudioClient*			pAudioClient;
//	IAudioRenderClient*		pAudioRenderClient;
//	IAudioEndpointVolume*	pOutVol;
//
//	TaskbarValue*			pTaskValue;
//	IWin32VSTHost*				pHost;
//	IAudioCaptureClient*	pCaptureClientParent;
//	IStream*				PCaptureClientStream;
//	IAudioCaptureClient*	pCaptureClient;
//	IAudioEndpointVolume*	pInVol;
//
//private:
//	DWORD				WasapiThread;
//	DWORD				DeviceCount;
//	DWORD				BufferSize;
//	HANDLE				hOutput;
//	HANDLE				hStart;
//	HANDLE				hExit;
//	WAVEFORMATEX*		m_MixFormat;
//
//	
//	IMMDeviceEnumerator*enumerator;	
//	IMFAsyncResult*		m_SampleReadyAsyncResult;
//	WASAPI_DEVICE_INFO** pDeviceInfo;
//	WASAPI_DEVICE_INFO	OutputDeviceInfo;
//	WASAPI_DEVICE_INFO	InputDeviceInfo;
//};
//
//typedef struct wsample_proc
//{
//	LPLOOP_INFO pLoopInfo;
//	OSRSample* pSample;
//	IMEngine* pEngine;
//} WASAPI_SAMPLE_PROC, *PWASAPI_SAMPLE_PROC, *LPWASAPI_SAMPLE_PROC;
//
//class IMMixer
//{
//public:
//
//private:
//	IMEngine* engine;
//};