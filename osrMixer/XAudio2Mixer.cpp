/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* XAudio2Mixer.cpp
* XAudio2 mixer implementation
*********************************************************/
#include "stdafx.h"
//
//DLL_API XMixer XAudioMixer;
//DLL_API HANDLE hPlay;
//
//OSRSample* SampleArrays[128] = { nullptr };
//LPLOOP_INFO loopInfo[512] = { nullptr };
//DWORD SampleNumber = 0;
//DWORD loopCount = 0;
//
//XPlay::XPlay(WAVEFORMATEX waveFormat, LOOP_INFO loop)
//{
//	loopCount++;
//	outFormat = waveFormat;
//	loopInfo[loopCount] = (LOOP_INFO*)FastAlloc(sizeof(LOOP_INFO));
//	*loopInfo[loopCount] = loop;
//
//	CurrentSampleCount = loopCount;
//}
//
//XPlay::~XPlay()
//{
//
//}
//
//VOID
//StreamingVoiceContext::OnBufferEnd(void*)
//{
//	SetEvent(hBufferEndEvent);
//}
//
//VOID
//XMixer::InitMixer(
//	XEngine engine
//)
//{
//	_DEB(engine.pXAudio->QueryInterface(IID_PPV_ARGS(&pXAudio)));
//	audioEngine = engine;
//}
//
//VOID
//XMixer::PlaySimpleWave()
//{	
//	//if (!audioEngine.sList.audioBuffer.AudioBytes) { return; }
//	//
//	//DWORD currentPosition = 0;
//	//HRESULT hr = audioEngine.sList.pSourceVoice->Start();
//	//DWORD cbValid = min(2048 * 4, audioEngine.sList.audioBuffer.AudioBytes - currentPosition);
//	//XAUDIO2_VOICE_STATE voiceState;
//	//BOOL isRunning = TRUE;
//	//BOOL bRet = FALSE;
//
//	//while (SUCCEEDED(hr) && isRunning)
//	//{
//	//	//if (msg.message == WM_QUIT) { break; }
//	//	audioEngine.sList.pSourceVoice->GetState(&voiceState);
//	//	if (!voiceState.BuffersQueued) { break; }
//
//	//	isRunning = (voiceState.BuffersQueued > 0) != 0;
//
//	//	if (GetAsyncKeyState(VK_SPACE)) { break; }
//	//	Sleep(10);
//	//}
//}
//
//VOID
//XMixer::PlayAsync(DWORD SampleNum)
//{
//	BOOL isRunning = TRUE;
//	OSRSample* Sample = SampleArrays[SampleNum];
//	HRESULT hr = 0;
//
//	FAILEDX2((hr = pXAudio->CreateSourceVoice(&audioEngine.lpSourceVoice, &audioEngine.play.outFormat, 0, 1.0f, &audioEngine.voiceContext)));
//
//	if (!Sample)
//	{ 
//		Sample = new OSRSample(
//			loopInfo[SampleNum]->waveFormat.wBitsPerSample,
//			loopInfo[SampleNum]->waveFormat.nChannels,
//			16384,
//			loopInfo[SampleNum]->waveFormat.nSamplesPerSec
//		);
//
//		 SampleArrays[0] = Sample;
//
//		Sample->LoadSample(
//			(void*)loopInfo[SampleNum]->pSample,
//			16384,
//			loopInfo[SampleNum]->waveFormat.wBitsPerSample,
//			loopInfo[SampleNum]->waveFormat.nChannels,
//			loopInfo[SampleNum]->waveFormat.nSamplesPerSec
//		);
//	}
//
//	bool isPlaying = false;
//
//	while (true)
//	{
//		HRESULT hr = 0;
//		XAUDIO2_VOICE_STATE voiceState;
//		XAUDIO2_BUFFER audioBuffer = { NULL };
//		float lpPlay[16384] = { NULL };
//
//		audioEngine.lpSourceVoice->GetState(&voiceState);
//
//		if (!isPlaying && voiceState.BuffersQueued >= 3)
//		{
//			FAILEDX2(audioEngine.lpSourceVoice->Start());
//			isPlaying = true;
//		}
//
//		if (voiceState.BuffersQueued < 3)
//		{
//			audioBuffer.AudioBytes = Sample->BufferSizeOutput * sizeof(f32);
//			Sample->ConvertToPlay(lpPlay);
//			audioBuffer.pAudioData = (BYTE*)lpPlay;
//			hr = audioEngine.lpSourceVoice->SubmitSourceBuffer(&audioBuffer);
//			SampleArrays[SampleNumber + 1] = SampleArrays[SampleNumber]->OnBufferEnd(loopInfo[loopCount]);
//			SampleNumber++;
//		}
//
//		if (SampleNumber > 127)
//		{
//			for (size_t i = 0; i < 127; i++) { delete SampleArrays[i]; }
//
//			SampleArrays[0] = SampleArrays[127];
//			SampleNumber = 0;
//		}
//
//		Sample = SampleArrays[SampleNumber];
//		if (GetAsyncKeyState(VK_SPACE)) { break; }
//		Sleep(5);
//	}
//
//	FAILEDX2(audioEngine.lpSourceVoice->Stop());
//	SampleNumber = 0;
//}
