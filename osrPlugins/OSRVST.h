/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: VST Host
**********************************************************
* OSRVST.h
* VST implementation
*********************************************************/
#pragma once
#include "OSR.h"

typedef struct  
{
	WCHAR PathToPlugin[260];
	bool isInterfaceEnable;
	bool isInstrument;
} VSTPLUGIN_INFO;

class IVSTHost : public IObject
{
public:	
	virtual int  LoadPlugin(LPCWSTR lpPathToPlugin) = 0;
	virtual int  CheckPlugin(LPCWSTR lpPathToPlugin, LPBOOL isFactoryEnable) = 0;
	virtual void UnloadPlugin() = 0; 

	virtual void ResumePlugin() = 0;
	virtual void SuspendPlugin() = 0;
	virtual void DestroyPlugin() = 0;

	virtual void CreatePluginWindow() = 0;
	virtual void DestroyPluginWindow() = 0;
	virtual void OpenPluginWindow() = 0;
	virtual void ClosePluginWindow() = 0;

	virtual int  InitPlugin(DWORD dwSampleRate, DWORD dwBlockSize) = 0;
	virtual void ProcessAudio(float** pAudioInput, float** pAudioOutput, DWORD dwSampleFrames) = 0;
};

#ifdef WIN32
class DLL_API IWin32VSTHost : public IVSTHost
{
public:
	RECT PluginRc;
 	void* PluginWindowHandle;
 	void* pEffect;
 	void* hPlugin;
 	wchar_t lpModulePath[128];
 	void* pUnknown;
 	wchar_t szPluginName[64];
 	wchar_t szWindowName[128];
 	bool IsVSTI;
 	bool IsInterfaceEnabled;

	IWin32VSTHost() : pEffect(nullptr), hPlugin(NULL), pUnknown(nullptr), IsVSTI(false), PluginWindowHandle(NULL), IsInterfaceEnabled(NULL)
	{
		memset(&PluginRc, 0, sizeof(RECT));
		memset(lpModulePath, 0, sizeof(wchar_t) * 128);
		memset(szPluginName, 0, sizeof(wchar_t) * 64);
		memset(szWindowName, 0, sizeof(wchar_t) * 128);
	}

	IWin32VSTHost(
		RECT PluginRc1,
		void* PluginWindowHandle1,
		void* pEffect1,
		void* hPlugin1,
		wchar_t lpModulePath1[128],
		void* pUnknown1,
		wchar_t szPluginName1[64],
		wchar_t szWindowName1[128],
		bool IsVSTI1,
		bool IsInterfaceEnabled1
	)
	{
		memcpy(&PluginRc, &PluginRc1, sizeof(RECT));
		memcpy(szPluginName, szPluginName1, sizeof(wchar_t) * 64);
		memcpy(lpModulePath, lpModulePath1, sizeof(wchar_t) * 128);
		memcpy(szWindowName, szWindowName1, sizeof(wchar_t) * 128);

		PluginWindowHandle = PluginWindowHandle1;
		pEffect = pEffect1;
		hPlugin = hPlugin1;
		pUnknown = pUnknown1;
		IsVSTI = IsVSTI1;
		IsInterfaceEnabled = IsInterfaceEnabled1;
	}

	int  LoadPlugin(LPCWSTR lpPathToPlugin) override;
	int  CheckPlugin(LPCWSTR lpPathToPlugin, LPBOOL isFactoryEnable)  override;
	void UnloadPlugin() override { if (hPlugin) { FreeLibrary((HMODULE)hPlugin); hPlugin = nullptr; } }


	void ResumePlugin() override;
	void SuspendPlugin() override;
	void DestroyPlugin() override;

	void CreatePluginWindow() override;
	void DestroyPluginWindow() override;
	void OpenPluginWindow() override;
	void ClosePluginWindow() override;

	int  InitPlugin(DWORD dwSampleRate, DWORD dwBlockSize) override;
	void ProcessAudio(float** pAudioInput, float** pAudioOutput, DWORD dwSampleFrames) override;

	void Release() override
	{
		if (hPlugin)
		{
			DestroyPluginWindow();
			DestroyPlugin();
			UnloadPlugin();
		}

		pEffect = nullptr;
		hPlugin = NULL;
		PluginWindowHandle = NULL;
		pUnknown = nullptr;

		delete this;
	}

	IObject* CloneObject() override
	{
		return new IWin32VSTHost(PluginRc, PluginWindowHandle, pEffect, hPlugin, lpModulePath, pUnknown, szPluginName, szWindowName, IsVSTI, IsInterfaceEnabled);
	}
};
#else

#endif

typedef struct PluginVst2xIdMembers
{
	DWORD dwId;
	CHAR* szIdString;
} *PluginVst2xId, VST2_PLUGINHOST, *PVST2_PLUGINHOST;


//BOOL CheckPluginList(VSTHost* pHost, LPCWSTR lpPluginFolder, VSTPLUGIN_INFO* pPluginInfo, size_t StructSizes);
