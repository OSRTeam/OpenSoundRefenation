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

bool isStarted = false;

IMFDecoder::IMFDecoder() : pSourceReader(nullptr), pMediaType(nullptr), pSecondMediaType(nullptr)
{

}

void
IMFDecoder::Release()
{
	_RELEASE(pSourceReader);
	_RELEASE(pMediaType);
	_RELEASE(pSecondMediaType);
}

void
IMFDecoder::DecodeFile(const char* PathToFileUTF8, void*& pOutFile, size_t& OutSize, WAVE_EX& waveFormat, FILE_TYPE& fileType)
{
	IMFPresentationDescriptor* pPresentDesc = nullptr;
	IMFSourceResolver* pSrcResolver = nullptr;
	IMFMediaSource* pMediaSrc = nullptr;
	IUnknown* pSrc = nullptr;
	WSTRING_PATH szPath = { 0 };
	std::vector<BYTE> lpData;

	if (MultiByteToWideChar(CP_UTF8, 0, PathToFileUTF8, strlen(PathToFileUTF8) + 1, szPath, sizeof(WSTRING_PATH)))
	{
		MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

		// create source resolver to get media source
		MFCreateSourceResolver(&pSrcResolver);
		pSrcResolver->CreateObjectFromURL(szPath, MF_RESOLUTION_MEDIASOURCE, nullptr, &ObjectType, &pSrc);


		if (!pSrc) { return; }
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
		MFCreateSourceReaderFromMediaSource(pMediaSrc, nullptr, &pSourceReader);

		// select first and deselect all other streams
		pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);
		pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

		DWORD32 dwSizeOfWaveFormat = 0;

		WAVEFORMATEX* pWave = nullptr;

		// get media type
		pSourceReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, &pMediaType);
		MFCreateWaveFormatExFromMFMediaType(pMediaType, &pWave, &dwSizeOfWaveFormat);

		if (pWave->wBitsPerSample == 24 && pWave->wFormatTag == 1) 
		{
			_RELEASE(pSourceReader);
			_RELEASE(pMediaSrc);
			return;
		}

		CoTaskMemFree(pWave);
		pWave = nullptr;

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

		// set cuurent format
		pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pUncompressedAudioType);
		MFCreateWaveFormatExFromMFMediaType(pUncompressedAudioType, &pWave, &dwSizeOfWaveFormat);
		pSourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

		IMFSample* pSample = nullptr;
		IMFMediaBuffer* pBuffer = nullptr;
		BYTE* localAudioData = nullptr;
		DWORD dwLocalAudioDataLength = 0;
		DWORD dwSampleCount = 0;
		DWORD flags = 0;

		size_t Bitrate = pWave->nChannels + pWave->nAvgBytesPerSec + (pWave->wBitsPerSample / 8);
		size_t OutSize = Bitrate * (uDuration / 1000) + Bitrate * (uDuration % 1000);
		lpData.reserve(OutSize);

		memcpy(&waveFormat, pWave, sizeof(WAVEFORMATEX));
		CoTaskMemFree(pWave);

		TaskbarValue* pValue = *GetTaskbarPointer();
		size_t ti = 0;
		f32 Percent = OutSize / 100;

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

			CurrentPercent = uSamplesLength / Percent;

			if (ti % 10)
			{
				pValue->SetValue(uSamplesLength, OutSize);
			}

			pBuffer->Unlock();
			localAudioData = nullptr;
			ti++;
		}

		OutSize = lpData.size();
		pOutFile = FastAlloc(OutSize * 2);

		switch (waveFormat.wBitsPerSample)
		{
		case 8:
		{
			BYTE* pD = nullptr;
			pD = (BYTE*)&lpData[0];

			for (size_t i = 0; i < OutSize; i++)
			{
				((f32*)(pOutFile))[i] = i16tof32(pD[i]);
			}
		}
		break;
		case 16:
		{
			WORD* pD = nullptr;
			pD = (WORD*)&lpData[0];

			for (size_t i = 0; i < OutSize; i++)
			{
				((f32*)(pOutFile))[i] = i16tof32(pD[i]);
			}
		}
		break;
		case 24:
		{
			i24* pD = nullptr;
			pD = (i24*)&lpData[0];

			for (size_t i = 0; i < OutSize; i++)
			{
				((f32*)(pOutFile))[i] = i24tof32(pD[i]);
			}
		}
		break;
		default:
			break;
		}

		waveFormat.wFormatTag = 3;
		waveFormat.wBitsPerSample = 32;
		waveFormat.nAvgBytesPerSec = (waveFormat.nSamplesPerSec * waveFormat.wBitsPerSample * waveFormat.nChannels) / 8;
		fileType = MP3_TYPE;

		pValue->SetCompleted();

		_RELEASE(pSample);
		_RELEASE(pBuffer);
		_RELEASE(pUncompressedAudioType);
		_RELEASE(pSourceReader);
		_RELEASE(pMediaSrc);
	}
}

void 
IMFDecoder::EncodeFile(const char* OutPath, void* pFile, size_t FileSize, WAVE_EX waveFormat, FILE_TYPE& fileType)
{

}

void 
IMFDecoder::GetDecoderString(STRING128& OutString)
{
	memcpy(OutString, u8"Media Foundation", strlen(u8"Media Foundation"));
}
