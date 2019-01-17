/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* SoundList.cpp
* Sound filesystem implementation
*********************************************************/
#include "stdafx.h"
//
//OSRCODE
//SoundList::LoadSoundFile(
//	LPCWSTR lpPath,
//	DWORD dwFlags
//)
//{
//	ZeroMemory(&audioBuffer, sizeof(XAUDIO2_BUFFER));
//	ZeroMemory(&wavFile, sizeof(WAVEFORMATEX));
//	waveFormat = nullptr;
//
//	// check for supporting by Windows Media Fondation
//	if (IsSupportedByMWF(lpPath, &waveFormat))
//	{
//		// load in buffer if this type is supported by WMf
//		LoadFileToMediaBuffer(audioData, &waveFormat);
//		audioBuffer.AudioBytes = (UINT32)audioData.size();
//		audioBuffer.pAudioData = (BYTE*)&audioData[0];
//	}
//	else
//	{
//		// read to external buffer
//		ReadAudioFile(lpPath, (VOID**)&lpData, &dwDataSize);
//		OSRCODE sCode = GetWaveFormatExtented(lpData, dwDataSize, &wavFile);
//		if (OSRFAILED(sCode)) { return sCode; }
//		wavFile.cbSize = 0;
//		audioBuffer.AudioBytes = dwDataSize - 44;
//		audioBuffer.pAudioData = reinterpret_cast<BYTE*>(lpData) + 44;
//		audioBuffer.Flags = XAUDIO2_END_OF_STREAM;
//		if (!waveFormat) { waveFormat = (WAVEFORMATEX*)FastAlloc(sizeof(WAVEFORMATEX)); }
//		*waveFormat = wavFile;
//	}
//	return OSR_SUCCESS;
//}
//
//OSRCODE
//SoundList::WriteWaveFile(
//	LPCWSTR lpFullpath,
//	std::vector<BYTE> pFile,
//	DWORD dwFlags
//)
//{
//	HANDLE hFile = CreateFileW(lpFullpath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (!hFile || hFile == (HANDLE)LONG_PTR(-1)) { THROW3(L"Application can't save this file because file handle is invalid."); return FS_OSR_BAD_HANDLE; }
//
//	{
//		// get current disk
//		WCHAR szCurrentDisk[4] = { lpFullpath[0], ':', '\\', '\0' };
//		ULARGE_INTEGER freeBytesToCaller = { NULL };
//
//		// get free disk space 
//		ASSERT2(GetDiskFreeSpaceExW(szCurrentDisk, &freeBytesToCaller, nullptr, nullptr), L"Can't get free disk space");
//
//		if (freeBytesToCaller.QuadPart < (UINT64)pFile.size())
//		{
//			int MsgBox = MessageBoxW(
//				NULL,
//				L"This file is bigger then free space on your drive. Continue?",
//				L"Warning",
//				MB_ICONWARNING | MB_YESNO
//			);
//
//			// if yes - continue
//			if (MsgBox != IDYES) { return FS_OSR_NO_SPACE; }
//		}
//	}
//
//	if (dwFlags == 1) { WriteFileFromMediaBufferEx(pSourceReader, hFile, pFile, nullptr, NULL); }
//	return OSR_SUCCESS;
//}
