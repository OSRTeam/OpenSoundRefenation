/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* AVCodecHeader.h
* FFMpeg decoder implementation
*********************************************************/
#pragma once
#include "stdafx.h"

class AVReader
{
public:
	DLL_API VOID OpenFileToBuffer(LPCWSTR lpPath, LPCWSTR* lpTempPath, LPDWORD dwSize, DWORD dwFormat, DWORD* lpSampleRate, LPWORD lpChannels, LPWORD Bits);
};
