/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: DirectSound Kernel Part
**********************************************************
* DirectSoundMain.h
* DirectSound header
**********************************************************/
#pragma once
#include "stdafx.h"
#include <dsound.h>



class DLL_API DEngine
{
public:
	DEngine();
	~DEngine() { _RELEASE(pDirectSound); _RELEASE(pDirectBuffer); _RELEASE(pSupportBuffer); }

	OSRCODE CreateDirectEngine(HWND hwnd, WAVEFORMATEX* waveFormat, DWORD dwBufSize);
	OSRCODE LoadSoundBuffer(LPVOID pData, WAVEFORMATEX* waveFormat);

	DWORD dwBufferSize;
	LPVOID pBuffer;
	LPDIRECTSOUND pDirectSound;
	LPDIRECTSOUNDBUFFER pDirectBuffer;
	LPDIRECTSOUNDBUFFER pSupportBuffer;
};

class DLL_API DMixer
{
public:
	DMixer() {};
	DMixer(DEngine customEngine) { engine = customEngine; } //-V1002
	~DMixer() {};

	OSRCODE InitMixer(HWND hwnd, WAVEFORMATEX waveFormat, DWORD dwDataSize);
	OSRCODE LoadSoundData(LPVOID lpData, WAVEFORMATEX waveFormat);
	OSRCODE PlaySoundData();
	OSRCODE StopSoundData();
	
private:
	DEngine engine;
};