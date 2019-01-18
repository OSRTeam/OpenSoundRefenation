/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR loop system
**********************************************************
* LoopList.h
* loop system kernel implementation
*********************************************************/
#pragma once
#include "stdafx.h"
#include "OSRSystem.h"

class ISoundList : public IObject
{
public:
	virtual void load(const char* pathUTF8, i32& FileNumber) = 0;
	virtual void unload(i32 FileNumber) = 0;

	i32 CurrentCountOfFiles;
	i32 MaxCountOfFiles;
	LPAUDIO_INFO lpFileInfo;
	IOSRFileSystem* pFileSystem;
};

class DLL_API IWin32SoundList : public ISoundList
{
	IWin32SoundList()
	{
		pFileSystem = new IOSRWin32FileSystem();

		MaxCountOfFiles = 100;
		CurrentCountOfFiles = -1;
		lpFileInfo = (LPAUDIO_INFO)AdvanceAlloc(MaxCountOfFiles * sizeof(AUDIO_INFO), HEAP_MEMORY_ALLOC);
	}

	IWin32SoundList(DWORD dwCurrentCountOfFiles1, DWORD dwMaxCountOfFiles1, LPAUDIO_INFO lpFileInfo1)
	{
		pFileSystem = new IOSRWin32FileSystem();

		MaxCountOfFiles = dwMaxCountOfFiles1;
		CurrentCountOfFiles = dwCurrentCountOfFiles1;
		lpFileInfo = (LPAUDIO_INFO)AdvanceAlloc(MaxCountOfFiles * sizeof(AUDIO_INFO), HEAP_MEMORY_ALLOC);
		memcpy(lpFileInfo, lpFileInfo1, MaxCountOfFiles * sizeof(AUDIO_INFO));
	}

	void load(const char* pathUTF8, i32& FileNumber) override;
	void unload(i32 FileNumber) override;

	void Release() override
	{
		for (size_t i = 0; i < MaxCountOfFiles; i++)
		{
			FREEKERNELHEAP(lpFileInfo[i].pSampleInfo);
		}

		_RELEASE(pFileSystem);
		FREEKERNELHEAP(lpFileInfo);
		delete this;
	}

	IObject* CloneObject() override
	{
		return new IWin32SoundList(CurrentCountOfFiles, MaxCountOfFiles, lpFileInfo);
	}
};
