#include "stdafx.h"
#include "IMWASAPI.h"
#include "WaveOutHeader.h"
#include "DiscordPart.h"

DiscordNetwork* pNetwork;
IVSTHost* pHost = nullptr;
MMEngine mmEngine;
//IMEngine eEngine;
LOOP_INFO lInfo;
HANDLE hStartEvent;
DWORD SamplesCount = 0;
size_t uPlay;
//WASAPI_SAMPLE_PROC SampleProc = { nullptr };
WAVOUT_SAMPLE_PROC MProc = { nullptr };

extern DLL_API bool IsBlur;
extern DLL_API bool IsLoad; 

VOID
WINAPIV
DecodeFileProc(
	LPVOID pProc
)
{
	IsLoad = true;
	DECODE_STRUCT* pStruct = (DECODE_STRUCT*)pProc;
	
	DWORD SampleNumber = 0;
	pStruct->pEngine->loopList.LoadAudioFile(pStruct->lpPath, USE_LIBSNDFILE, 0, &SampleNumber);
	*pStruct->pLoopInfo = *(pStruct->pEngine->loopList.GetLoopInfo()->pSampleInfo);
	SetEvent(hStartEvent);
	IsLoad = false;
} 

void
OSR::Engine::DecodeFile(
	LPCWSTR lpPath,
	LPLOOP_INFO pLoopInfo
)
{
	static ThreadSystem threadS = {};
	static DECODE_STRUCT decodeStruct = { 0 };
	static WSTRING_PATH szPath = { 0 };

	memcpy(szPath, lpPath, sizeof(WSTRING_PATH));

	decodeStruct.pEngine = this;
	decodeStruct.lpPath = szPath;
	decodeStruct.pLoopInfo = pLoopInfo;

	hStartEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	threadS.CreateUserThread(nullptr, DecodeFileProc, &decodeStruct, L"OSR Decoder Worker");
}

#define TEST_VST 1

void
OSR::Mixer::CreateMixer(
	HWND hwnd,
	LPVOID network
)
{
	pNetwork = (DiscordNetwork*)network;
	WSTRING_PATH pathToDll = { 0 };

//	eEngine.InitEngine(hwnd);
//	eEngine.CreateDefaultDevice(600000);
//
//#ifdef TEST_VST
//	pHost = new IWin32VSTHost();
//	pVSTHost = pHost;
//	eEngine.pHost = (IWin32VSTHost*)pVSTHost;
//	if (OSRSUCCEDDED(OpenFileDialog(&pathToDll)))
//	{
//		eEngine.pHost->LoadPlugin(pathToDll);
//		eEngine.pHost->InitPlugin(eEngine.GetOutputInfo()->waveFormat.nSamplesPerSec, eEngine.GetBufferSize() * eEngine.GetOutputInfo()->waveFormat.nChannels);
//	}
//#endif
}

void
OSR::Mixer::LoadSample(
	LPCWSTR lpPath
)
{
	osrEngine.DecodeFile(lpPath, &lInfo);

	WIN32_FIND_DATAW findData = { 0 };
	HANDLE hFind = FindFirstFileW(lpPath, &findData);
	DWORD DataFile = 0;

	// if file doesn't exist
	if (hFind && hFind != INVALID_HANDLE_VALUE)
	{
		int StringSize = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, nullptr, 0, NULL, NULL);
		LPSTR lpNewString = nullptr;

		if (StringSize)
		{
			// allocate new string at kernel heap
			lpNewString = (LPSTR)FastAlloc(++StringSize);

			ASSERT2(WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, lpNewString, StringSize, NULL, NULL), L"Can't convert wchar_t to char");

			memcpy(pNetwork->szName, lpNewString, min(StringSize, 96));
		}

		pNetwork->SetStatus(DiscordNetwork::StatusNumber::OpenedAudio);
		
		FREEKERNELHEAP(lpNewString);

		FindClose(hFind);
	}

	uPlay = 0;
}

void
OSR::Mixer::PlaySample()
{
	//SampleProc.pEngine = &eEngine;
	//SampleProc.pLoopInfo = &lInfo;
	//SampleProc.pSample = nullptr;

	//if (uPlay)
	//{
	//	eEngine.RestartDevice(600000);
	//}

	//WaitForSingleObject(hStartEvent, INFINITE);
	//eEngine.StartDevice((LPVOID)&SampleProc);
	//pNetwork->SetStatus(DiscordNetwork::StatusNumber::PlayingAudio);

	//uPlay++;
}

void
OSR::Mixer::StopSample()
{
	//eEngine.StopDevice();
	//pNetwork->SetStatus(DiscordNetwork::StatusNumber::OpenedAudio);
}

void
OSR::Mixer::SetAudioPosition(f32 Position)
{
	//eEngine.SetAudioPosition(Position);
}

void
OSR::Mixer::OpenPlugin(bool& isOpen)
{
	if (pVSTHost)
	{
		IVSTHost* pLocalHost = (IVSTHost*)pVSTHost;

		if (!isOpen)
		{
			pLocalHost->ClosePluginWindow();
		}
		else
		{
			pLocalHost->OpenPluginWindow();
		}

		isOpen = !isOpen;
	}
}