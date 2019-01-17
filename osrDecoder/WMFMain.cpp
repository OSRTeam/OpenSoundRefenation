/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: WMF decoder
**********************************************************
* WMFMain.cpp
* WMF decoder implementation
*********************************************************/
#include "stdafx.h"

VOID
WMFReader::WMFInit()
{
	FAILEDX2((MFCreateAttributes(&pAttribute, 1)));
	FAILEDX2(pAttribute->SetUINT32(MF_LOW_LATENCY, TRUE));
}

BOOL
WMFReader::IsSupportedByMWF(
	LPCWSTR lpPath,
	WAVEFORMATEX** waveFormat
)
{
	IMFPresentationDescriptor* pPresentDesc = nullptr;
	IMFSourceResolver* pSrcResolver = nullptr;
	IMFMediaSource* pMediaSrc = nullptr;
	IUnknown* pSrc = nullptr;

	{
		MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

		// create source resolver to get media source
		_DEB(MFCreateSourceResolver(&pSrcResolver));
		_DEB(pSrcResolver->CreateObjectFromURL(lpPath, MF_RESOLUTION_MEDIASOURCE, NULL, &ObjectType, &pSrc));
	}

	if (!pSrc) { return FALSE; }
	// query with media source
	pSrc->QueryInterface(&pMediaSrc);

	// get file duration (100 nanosecs count)
	pMediaSrc->CreatePresentationDescriptor(&pPresentDesc);
	pPresentDesc->GetUINT64(MF_PD_DURATION, &uDuration);
	if (uDuration) { uDuration /= 10000; }

	// release all stuff
	_RELEASE(pPresentDesc);
	_RELEASE(pSrcResolver);
	_RELEASE(pSrc);

	// open source reader
	MFCreateSourceReaderFromMediaSource(pMediaSrc, pAttribute, &pSourceReader);

	// select first and deselect all other streams
	pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);
	pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

	DWORD32 dwSizeOfWaveFormat = 0;

	// get media type
	pSourceReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, &pMediaType);
	MFCreateWaveFormatExFromMFMediaType(pMediaType, waveFormat, &dwSizeOfWaveFormat);

	if ((*waveFormat)->wBitsPerSample != 24) { return TRUE; }

	if ((*waveFormat)->wFormatTag == 1)
	{
		CoTaskMemFree(*waveFormat);
		*waveFormat = nullptr;
		_RELEASE(pSourceReader);
		_RELEASE(pMediaSrc);
	}

	return FALSE;
}

VOID
WMFReader::LoadFileToMediaBuffer(
	std::vector<BYTE>& lpData,
	WAVEFORMATEX** waveFormat
)
{
	GUID MajorType = { NULL };
	GUID SubType = { NULL };

	// get GUID
	pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &MajorType);

#ifdef DEBUG
	// Video is not supported 
	if (MajorType != MFMediaType_Audio) { DEBUG_BREAK; }
#endif

	pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &SubType);

	// get info about compress
	BOOL isCompressed = FALSE;
	pMediaType->IsCompressedFormat(&isCompressed);

	// if file is PCM - don't create media type
	if (SubType == MFAudioFormat_Float || SubType == MFAudioFormat_PCM || !isCompressed)
	{
		pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	}
	else
	{
		// set reader to PCM 
		MFCreateMediaType(&pSecondMediaType);
		pSecondMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		pSecondMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pSecondMediaType);
	}

	IMFMediaType* pUncompressedAudioType = nullptr;
	DWORD32 dwSizeOfWaveFormat = NULL;

	// set cuurent format
	pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pUncompressedAudioType);
	MFCreateWaveFormatExFromMFMediaType(pUncompressedAudioType, waveFormat, &dwSizeOfWaveFormat);
	pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

	IMFSample* pSample = nullptr;
	IMFMediaBuffer* pBuffer = nullptr;
	BYTE* localAudioData = nullptr;
	DWORD dwLocalAudioDataLength = 0;
	DWORD dwSampleCount = 0;
	DWORD flags = 0;

	size_t Bitrate = ((*waveFormat)->nChannels + (*waveFormat)->nAvgBytesPerSec + ((*waveFormat)->wBitsPerSample / 8));
	size_t OutSize = Bitrate * (uDuration / 1000) + Bitrate * (uDuration % 1000);
	lpData.reserve(OutSize);

	TaskbarValue* pValue = *GetTaskbarPointer();
	size_t ti = 0;

	while (true)
	{
		pSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, nullptr, &flags, nullptr, &pSample);

		// check whether the data is still valid
		if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED || flags & MF_SOURCE_READERF_ENDOFSTREAM) { break; }
		if (pSample == nullptr) { continue; }

		// convert data to contiguous buffer
		pSample->ConvertToContiguousBuffer(&pBuffer);
		pBuffer->Lock(&localAudioData, nullptr, &dwLocalAudioDataLength);
		uSamplesLength += dwLocalAudioDataLength;

		for (DWORD i = 0; i < dwLocalAudioDataLength; i++)
		{
			lpData.push_back(localAudioData[i]);
		}

		if (ti % 10)
		{
			pValue->SetValue(uSamplesLength, OutSize);
		}

		pBuffer->Unlock();
		localAudioData = nullptr;
		ti++;
	}

	pValue->SetCompleted();

	_RELEASE(pSample);
	_RELEASE(pBuffer);
}

BOOL 
WriteToFile(
	HANDLE hFile, 
	LPVOID pData,
	DWORD cbSize
)
{
	DWORD cbWritten = 0;

	BOOL bResult = WriteFile(hFile, pData, cbSize, &cbWritten, NULL);
	return bResult;
}

VOID
WMFReader::WriteFileFromMediaBufferEx(
	IMFSourceReader* pSourceReader,
	HANDLE hFile, 
	std::vector<BYTE>& pData,
	BYTE** pSecondData,
	DWORD dwDataSize
)
{
	DWORD32 dwWaveFormatSize = 0;
	DWORD dwHeader = 0;
	DWORD dwAudioData = 0;
	DWORD dwMaxAudioData = 0;
	WAVEFORMATEX* waveFormatToRecord = { NULL };

	IMFMediaType* pPartialType = nullptr;
	IMFMediaType* pNeedyType = nullptr;

	// create media type from source reader
	FAILEDX2(MFCreateMediaType(&pPartialType));

	// set PCM Audio type
	FAILEDX2(pPartialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
	FAILEDX2(pPartialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM));

	// set our new type
	FAILEDX2(pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pPartialType));
	FAILEDX2(pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pNeedyType));

	// select stream position
	FAILEDX2(pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE));

	// get WAVEFORMATEX
	FAILEDX2(MFCreateWaveFormatExFromMFMediaType(pNeedyType, &waveFormatToRecord, &dwWaveFormatSize));

	// set header
	DWORD dwHead[] = { FCC('RIFF'), NULL, FCC('WAVE'), FCC('fmt '), dwWaveFormatSize };
	DWORD dwHeadData[] = { FCC('data'), NULL };

	// write first data
	ASSERT2(WriteToFile(hFile, dwHead, sizeof(dwHead)), L"Can't write header to file");
	ASSERT2(WriteToFile(hFile, waveFormatToRecord, dwWaveFormatSize), L"Can't write WAVEFORMATEX to file");
	ASSERT2(WriteToFile(hFile, dwHeadData, sizeof(dwHeadData)), L"Can't write header data to file");

	CoTaskMemFree(waveFormatToRecord);
	waveFormatToRecord = nullptr;

	dwHeader = sizeof(dwHead) + dwWaveFormatSize + sizeof(dwHeadData);	// get size of header 

	size_t pRawDataSize = NULL;
	if (!dwDataSize || !(*pSecondData)) { pRawDataSize = pData.size(); }
	else { pRawDataSize = dwDataSize; }

	// write full audio data
	if (!*pSecondData)
	{
		ASSERT2(WriteToFile(hFile, &pData[0], (DWORD)pRawDataSize), L"Can't write raw PCM data to file")
	}
	else 
	{
		ASSERT2(WriteToFile(hFile, *pSecondData, (DWORD)pRawDataSize), L"Can't write raw PCM data to file")
	}

	// get size of header
	LARGE_INTEGER largeInt = { NULL };
	largeInt.QuadPart = dwHeader - sizeof(DWORD);

	// set pointer pos to write file size
	ASSERT2(SetFilePointerEx(hFile, largeInt, NULL, FILE_BEGIN), L"Can't set pointer position");
	ASSERT2(WriteToFile(hFile, &pRawDataSize, sizeof(DWORD)), L"Can't write raw data size t file");

	DWORD dwRIFFFileSize = dwHeader + ((DWORD)pRawDataSize) - 8;
	largeInt.QuadPart = sizeof(FOURCC);

	// finally write a file
	ASSERT2(SetFilePointerEx(hFile, largeInt, NULL, FILE_BEGIN), L"Can't set pointer position");
	ASSERT2(WriteToFile(hFile, &dwRIFFFileSize, sizeof(dwRIFFFileSize)), L"Can't write RIFF file data size");

	// clode file handle
	CloseHandle(hFile);
}
