/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* OSRClasses.h
* Main classes for engine
*********************************************************/
#pragma once
#include "OSR.h"
#include "OSRSystem.h"
#ifdef WIN32
#include "OSRWin32kernel.h"
#endif

#include "LoopList.h"
#include "MixerBase.h"
#include "AsyncMixer.h"

DWORD DLL_API StartApplication(LPWSTR lpCmdLine);

class IOSREngine : IObject
{
public:

	IOSRFileSystem* pFileSystem;
};

class IOSRDecoder : IObject
{
public:
	IOSRDecoder()
	{
		pList = new IWin32SoundList();
	}

	IOSRDecoder(IObject* pList1)
	{
		pList = (ISoundList*)pList1;
	}

	void DecodeObject(const char* pInFile, i32& OutFile);
	void EncodeObject(i32 inFile, const char* pOutFile);
	void RemoveObject(i32 Object);

	void Release() override
	{
		_RELEASE(pList);
		delete this;
	}

	IObject* CloneObject() override
	{
		IObject* pList1 = pList->CloneObject();
		return new IOSRDecoder(pList1);
	}

	ISoundList* pList;
	i32 NumberOfTrack;
};

class IOSRMixer : IObject
{
public:
	IOSRMixer()
	{
		StartHandle = CreateEventW(nullptr, TRUE, FALSE, nullptr);
		EndHandle = CreateEventW(nullptr, TRUE, FALSE, nullptr);
		pDecoder = new IOSRDecoder();
		pvMixer = new IMixerAsync();
	}

	IOSRMixer(IObject* pDecoder1, IObject* pMixer1)
	{
		pDecoder = (IOSRDecoder*)pDecoder1;
		pvMixer = (IMixerInterface*)pMixer1;
	}

	void SetPosition(f32 Position);
	void OpenPlugin(u32 Track, u32 Effect);
	void ClosePlugin(u32 Track, u32 Effect);
	void AddPlugin(u32 Track, IObject* pPlugin, u32& OutEffect);
	void DeletePlugin(u32 Track, u32 Effect);
	void CreateMixer(f64 Delay);
	void RestartMixer(f64 Delay);
	void DestroyMixer();
	void Play();
	void Stop();

	void Release() override
	{
		_RELEASE(pDecoder);
		_RELEASE(pvMixer);
		delete this;
	}

	IObject* CloneObject() override
	{
		IObject* pDecoder1 = pDecoder->CloneObject();
		IObject* pMixer1 = pvMixer->CloneObject();

		return new IOSRMixer(pDecoder1, pMixer1);
	}

	IOSRDecoder* pDecoder;
	IMixerInterface* pvMixer;
	u32 TrackNum;
	bool isPlay = false;
	OSRHandle StartHandle;
	OSRHandle EndHandle;
	ThreadSystem thread;
};

class IOSRUI : IObject
{
public:
	OSRCODE CreateMainWindow();

	void Release() override
	{
		if (WindowHandle) { DestroyWindow(WindowHandle); }
		delete this;
	}

	IObject* CloneObject() override
	{
		return nullptr;
	}


	HWND WindowHandle;
};
