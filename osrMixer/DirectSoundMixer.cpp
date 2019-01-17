/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: DirectSound Kernel Part
**********************************************************
* DirectSoundMixer.cpp
* DirectSound mixer implementation
**********************************************************/
#include "stdafx.h"
#pragma hdrstop

OSRCODE
DMixer::InitMixer(
	HWND hwnd,
	WAVEFORMATEX waveFormat,
	DWORD dwDataSize
)
{
	OSRFAIL2(engine.CreateDirectEngine(hwnd, &waveFormat, dwDataSize), L"Can't create DirectSound engine");
	return OSR_SUCCESS;
}

OSRCODE 
DMixer::LoadSoundData(
	LPVOID lpData,
	WAVEFORMATEX waveFormat
)
{
	OSRFAIL2(engine.LoadSoundBuffer(lpData, &waveFormat), L"Can't load data to buffer");

	return OSR_SUCCESS;
}

OSRCODE
DMixer::PlaySoundData()
{
	if (!engine.pSupportBuffer) { return MXR_OSR_BUFFER_CORRUPT; }

	if (FAILED(engine.pSupportBuffer->SetCurrentPosition(NULL))) { return MXR_OSR_BUFFER_CORRUPT; }
	if (FAILED(engine.pSupportBuffer->SetVolume(NULL))) { return MXR_OSR_BUFFER_CORRUPT; }
	if (FAILED(engine.pSupportBuffer->Play(NULL, NULL, NULL))) { return MXR_OSR_NO_OUT; }

	return OSR_SUCCESS;
}

OSRCODE
DMixer::StopSoundData()
{
	if (!engine.pSupportBuffer) { return MXR_OSR_BUFFER_CORRUPT; }

	if (FAILED(engine.pSupportBuffer->Stop())) { return MXR_OSR_NO_OUT; }

	return OSR_SUCCESS;
}
