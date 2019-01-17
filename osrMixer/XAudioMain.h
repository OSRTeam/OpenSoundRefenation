/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* XAudioMain.h
* XAudio2 decoder implementation
*********************************************************/
#pragma once
//#include <windows.h>
//#include <xaudio2.h>
//#include <xaudio2fx.h>
//#include "OSRSystem.h"
//
//struct DLL_API StreamingVoiceContext : public IXAudio2VoiceCallback
//{
//	HANDLE hBufferEndEvent;
//
//	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) override { }
//	STDMETHOD_(void, OnVoiceProcessingPassEnd)() override { }
//	STDMETHOD_(void, OnStreamEnd)() override { }
//	STDMETHOD_(void, OnBufferStart)(void*) override { }
//	virtual COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnBufferEnd(void*) override;
//	STDMETHOD_(void, OnLoopEnd)(void*) override { }
//	STDMETHOD_(void, OnVoiceError)(void*, HRESULT) override { }
//
//	StreamingVoiceContext() { }
//
//	virtual ~StreamingVoiceContext()
//	{
//	
//	}
//};
//
////// deprecated
////class DLL_API SoundList : public WMFReader
////{
////public:
////	SoundList() : pSourceVoice(nullptr), waveFormat(nullptr), dwDataSize(NULL), dwIndex(NULL), lpData(nullptr) { }
////	OSRCODE LoadAsync(OSRSample Sample);
////	OSRCODE LoadSoundFile(LPCWSTR lpPath, DWORD dwFlags);
////	OSRCODE WriteWaveFile(LPCWSTR lpFullpath, std::vector<BYTE> pFile, DWORD dwFlags);
////
////	StreamingVoiceContext voiceContext;
////	IXAudio2SourceVoice* pSourceVoice;
////	std::vector<BYTE> audioData;
////	XAUDIO2_BUFFER audioBuffer;
////	WAVEFORMATEX* waveFormat;
////	WAVEFORMATEX wavFile;
////	DWORD dwDataSize;
////	DWORD dwIndex;
////	BYTE* lpData;
////};
//
//class DLL_API XPlay
//{
//public:
//	XPlay() { };
//	XPlay(WAVEFORMATEX waveFormat, LOOP_INFO loop);
//	~XPlay();
//
//	WAVEFORMATEX outFormat;
//	DWORD CurrentSampleCount;
//};
//
//// AuEngine 2.1
//class DLL_API XEngine
//{
//public:	
//	XEngine() {};
//	~XEngine() {};
//	VOID CreateXEngine(XPlay inPlay);
//
//	XPlay play;
//	StreamingVoiceContext voiceContext;
//	IXAudio2SourceVoice* lpSourceVoice;
//	IXAudio2MasteringVoice* pMasteringVoice;
//	IXAudio2* pXAudio;
//};
//
////extern DLL_API HANDLE hPlay;
//
//class DLL_API XMixer
//{
//public:
//	XMixer() { /*hPlay = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);*/ }
//	VOID InitMixer(XEngine engine);
//	VOID PlaySimpleWave();
//	VOID PlayAsync(DWORD SampleNum);
//
//private:
//	// interfaces
//	IXAudio2* pXAudio;
//	IXAudio2SubmixVoice* pSubmixVoice;
//
//	// audio engine and source
//	XEngine audioEngine;
//};
//
//extern DLL_API XMixer XAudioMixer;
//extern DLL_API XEngine XAudioEngine;