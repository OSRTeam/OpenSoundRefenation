/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR Mixer
**********************************************************
* AsyncMixer.cpp
* Acync mixer implementation
*********************************************************/
#include "stdafx.h"
#include "AsyncMixer.h"

DWORD
WINAPIV
AsyncMixerThreadProc(LPVOID pData)
{
	u32 dwWait = 0;
	BOOL isPlay = TRUE;
	bool isFirst = true;
	IMixerAsync* pMixer = (IMixerAsync*)pData;
	HANDLE hArray[] = { pMixer->hReleaseThread, pMixer->hWaitThread, pMixer->hStartThread };
	WAVEFORMATEX waveFormat = ConvertToWaveFormat(pMixer->HostsInfo[1].FormatType);
	size_t BufSize = pMixer->HostsInfo[1].BufferSize;
	u8* pOutBuffer = (u8*)FastAlloc(BufSize);
	float* pInVstData[16] = { nullptr };
	float* pOutVstData[16] = { nullptr };

	// allocate input buffers
	for (f32*& pvData : pInVstData)
	{
		pvData = (f32*)FastAlloc(BufSize * sizeof(f32) / waveFormat.nChannels);
	}

	// allocate output buffers
	for (f32*& pvData : pOutVstData)
	{
		pvData = (f32*)FastAlloc(BufSize * sizeof(f32) / waveFormat.nChannels);
	}

	if (OSRFAILED(pMixer->pSound->PlayHost()))
	{
		dwWait = u32(-1);
		goto clean;
	}

	// wait for any event
	while (((dwWait = WaitForMultipleObjects(3, hArray, FALSE, INFINITE)) != WAIT_FAILED) && isPlay)
	{
		switch (dwWait)
		{
		case WAIT_OBJECT_0:
			isPlay = FALSE;
			break;
		case WAIT_OBJECT_0 + 1:
			Sleep(5);
			break;
		case WAIT_OBJECT_0 + 2:
		{
			for (TRACK_INFO track : pMixer->tracksInfo)
			{
				// process by VST plugin
				for (IObject* ThisObject : track.pEffectHost)
				{
					IWin32VSTHost* pVstHost = reinterpret_cast<IWin32VSTHost*>(ThisObject);

					if (pVstHost)
					{
						// copy data from track buffer
						for (size_t i = 0; i < waveFormat.nChannels; i++)
						{
							memcpy(pInVstData[i], track.pData->pOutputBuffer[i], BufSize * sizeof(f32) / waveFormat.nChannels);
						}

						// process VST
						pVstHost->ProcessAudio(pInVstData, pOutVstData, BufSize / 2);

						// copy data from output buffer to track buffer
						for (size_t i = 0; i < waveFormat.nChannels; i++)
						{
							memcpy(track.pData->pOutputBuffer[i], pOutVstData[i], BufSize * sizeof(f32) / waveFormat.nChannels);
						}
					}
				}

				// mix data
				for (size_t u = 0; u < waveFormat.nChannels; u++)
				{
					// Output buffer data + local track data = mixed data
					for (size_t i = 0; i < pMixer->HostsInfo[1].BufferSize / 2; i++)
					{
						pMixer->pData->pOutputBuffer[u][i] += track.pData->pOutputBuffer[u][i];
					}
				}
			}

			pMixer->pData->ConvertToPlay(pOutBuffer);

			if (isFirst)
			{
				// create output thread
				pMixer->pSound->RecvPacket((LPVOID)2, DeviceOutputData, 8);
				isFirst = false;
			}

			pMixer->pSound->RecvPacket(pOutBuffer, SoundData, pMixer->tracksInfo[0].BufferSize);
		}
		}
		break;
	}

	pMixer->pSound->StopHost();
	dwWait = 0;

clean:
	// release input buffers
	for (float* pvData : pInVstData)
	{
		FREEKERNELHEAP(pvData);
	}

	// release output buffer
	for (float* pvData : pOutVstData)
	{
		FREEKERNELHEAP(pvData);
	}

	FREEKERNELHEAP(pOutBuffer);
	isFirst = false;

	return dwWait;
}

OSRCODE 
IMixerAsync::start(int Device)
{
	if (Device != -1)
	{
		OSRFAIL2(pSound->CreateRenderSoundDevice(100, Device), L"Can't create render device");
	}
	else
	{
		OSRFAIL2(pSound->CreateRenderDefaultSoundDevice(100), L"Can't create render device");
	}

	memcpy(&HostsInfo[1], &pSound->OutputHost, sizeof(AUDIO_HOST));

	WAVEFORMATEX wf = ConvertToWaveFormat(HostsInfo[1].FormatType);

	if (!pData)
	{
		pData = new OSRSample(wf.wBitsPerSample, wf.nChannels, HostsInfo[1].BufferSize / 2, wf.nSamplesPerSec);
		for (u32 i = 0; i < pData->ChannelsOutput; i++)
		{
			pData->pOutputBuffer[i] = (f32*)AdvanceAlloc(HostsInfo[1].BufferSize / 2 * sizeof(f32), NULL);
		}
	}

	return OSR_SUCCESS;
}

OSRCODE
IMixerAsync::start_delay(int Device, f64 HostDelay)
{
	if (Device != -1)
	{
		OSRFAIL2(pSound->CreateRenderSoundDevice(HostDelay, Device), L"Can't create render device");
	}
	else
	{
		OSRFAIL2(pSound->CreateRenderDefaultSoundDevice(HostDelay), L"Can't create render device");
	}

	memcpy(&HostsInfo[1], &pSound->OutputHost, sizeof(AUDIO_HOST));

	WAVEFORMATEX wf = ConvertToWaveFormat(HostsInfo[1].FormatType);

	if (!pData)
	{
		pData = new OSRSample(wf.wBitsPerSample, wf.nChannels, HostsInfo[1].BufferSize / 2, wf.nSamplesPerSec);
		for (u32 i = 0; i < pData->ChannelsOutput; i++)
		{
			pData->pOutputBuffer[i] = (f32*)AdvanceAlloc(HostsInfo[1].BufferSize / 2 * sizeof(f32), NULL);
		}
	}

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::restart(int Device)
{
	OSRFAIL2(close(), L"Can't close devices");
	return start(Device);
}

OSRCODE 
IMixerAsync::restart_delay(int Device, f64 HostDelay)
{
	OSRFAIL2(close(), L"Can't close devices");
	return start_delay(Device, HostDelay);
}

OSRCODE 
IMixerAsync::close()
{
	FREEKERNELHEAP(pData);

	OSRFAIL2(pSound->CloseCaptureSoundDevice(), L"Can't close capture device");
	OSRFAIL2(pSound->CloseRenderSoundDevice(), L"Can't close render device");

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::master_volume(f32 Volume)
{
	return pSound->SetVolumeLevel(Volume);
}

OSRCODE 
IMixerAsync::track_volume(u32 TrackNumber, f32& Volume)
{
	Volume = tracksInfo[TrackNumber].GainLevel;
	
	return OSR_SUCCESS;
}

OSRCODE
IMixerAsync::add_track(u8 Channels, u32 SampleRate, u32& TrackNumber)
{
	if (TracksCount <= 256)
	{
		WAVEFORMATEX wf = ConvertToWaveFormat(HostsInfo[1].FormatType);

		tracksInfo[TrackNumber].TrackNumber = TrackNumber;
		tracksInfo[TrackNumber].SampleRate = SampleRate;
		tracksInfo[TrackNumber].isActivated = true;
		tracksInfo[TrackNumber].bEffects = true;
		tracksInfo[TrackNumber].bMute = false;
		tracksInfo[TrackNumber].bSolo = false;
		tracksInfo[TrackNumber].Bits = 32;
		tracksInfo[TrackNumber].BufferSize = HostsInfo[1].BufferSize;
		tracksInfo[TrackNumber].ChannelRouting = 0;
		tracksInfo[TrackNumber].Channels = Channels;
		tracksInfo[TrackNumber].GainLevel = 0.f;
		tracksInfo[TrackNumber].WideImaging = 0.f;
		tracksInfo[TrackNumber].pData = new OSRSample(wf.wBitsPerSample, wf.nChannels, HostsInfo[1].BufferSize, wf.nSamplesPerSec);

		for (u32 i = 0; i < tracksInfo[TrackNumber].pData->ChannelsOutput; i++)
		{
			tracksInfo[TrackNumber].pData->pOutputBuffer[i] = (f32*)AdvanceAlloc(HostsInfo[1].BufferSize * sizeof(f32), NULL);
		}
		
		TrackNumber = ++TracksCount;

		memset(tracksInfo[TrackNumber].szTrackName, 0, 256);
		memset(tracksInfo[TrackNumber].pEffectHost, 0, sizeof(IObject*) * 90);
		strcpy_s(tracksInfo[TrackNumber].szTrackName, u8"Default Track");

		return OSR_SUCCESS;
	}

	return MXR_OSR_BUFFER_CORRUPT;
}

OSRCODE 
IMixerAsync::delete_track(u32 TrackNumber)
{
	if (TrackNumber > TracksCount)		{ return KERN_OSR_BAD_ALLOC; }
	if (tracksInfo[TrackNumber].pData)	{ delete tracksInfo[TrackNumber].pData; }

	memset(&tracksInfo[TrackNumber], 0, sizeof(TRACK_INFO));

	if (TrackNumber != (TracksCount - 1))
	{
		for (u32 i = TrackNumber; i < TracksCount; i++)
		{
			memcpy(&tracksInfo[TrackNumber], &tracksInfo[TrackNumber + 1], sizeof(TRACK_INFO));
		}
	}

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::add_effect(u32 TrackNumber, IObject* pEffectHost, size_t EffectSize, u32& EffectNumber)
{
	bool isFull = true;
	size_t EffectN = 0;

	if (TrackNumber > TracksCount) { return KERN_OSR_BAD_ALLOC; }

	if (EffectSize == sizeof(IWin32VSTHost))
	{
		for (size_t i = 0; i < 90; i++)
		{
			if (!tracksInfo[TrackNumber].pEffectHost[i]) { EffectN = i; isFull = false; break; };
		}

		if (!isFull)
		{
			tracksInfo[TrackNumber].pEffectHost[EffectN] = pEffectHost;
			EffectNumber = EffectN;
			EffectsNumber++;
		}
	}

	return isFull ? MXR_OSR_BUFFER_CORRUPT : OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::delete_effect(u32 TrackNumber, u32 EffectNumber)
{
	if (TrackNumber > TracksCount) { return KERN_OSR_BAD_ALLOC; }

	_RELEASE(tracksInfo[TrackNumber].pEffectHost[EffectNumber]);

	if (EffectNumber != (EffectsNumber - 1))
	{
		for (u32 i = EffectNumber; i < EffectsNumber; i++)
		{
			tracksInfo[TrackNumber].pEffectHost[EffectNumber] = tracksInfo[TrackNumber].pEffectHost[EffectNumber + 1];
		}
	}

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::rout_solo(u32 TrackNumber, bool& isSolo)
{
	tracksInfo[TrackNumber].bSolo = !tracksInfo[TrackNumber].bSolo;
	isSolo = tracksInfo[TrackNumber].bSolo;

	return OSR_SUCCESS;
}

OSRCODE
IMixerAsync::rout_mute(u32 TrackNumber, bool& isMuted)
{
	tracksInfo[TrackNumber].bMute = !tracksInfo[TrackNumber].bMute;
	isMuted = tracksInfo[TrackNumber].bMute;

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::rout_effects(u32 TrackNumber, bool& isEffects)
{
	tracksInfo[TrackNumber].bEffects = !tracksInfo[TrackNumber].bEffects;
	isEffects = tracksInfo[TrackNumber].bEffects;

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::rout_activate(u32 TrackNumber, bool& isActivated)
{
	tracksInfo[TrackNumber].isActivated = !tracksInfo[TrackNumber].isActivated;
	isActivated = tracksInfo[TrackNumber].isActivated;

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::rout_channel(u32 TrackNumber, u32 ChannelRouting)
{
	tracksInfo[TrackNumber].ChannelRouting = ChannelRouting;

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::rout_wide_image(u32 TrackNumber, f64 WideImaging)
{
	tracksInfo[TrackNumber].WideImaging = WideImaging;

	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::rout_track_number(u32& TrackNumber, u32 ToNumber)
{
	//#TODO:
	return OSR_SUCCESS;
}

OSRCODE 
IMixerAsync::put_data(u32 TrackNumber, void* pData, size_t DataSize)
{
	if (pData && DataSize == tracksInfo[TrackNumber].BufferSize)
	{
		memcpy(tracksInfo[TrackNumber].pData, pData, DataSize);
		return OSR_SUCCESS;
	}

	return MXR_OSR_BUFFER_CORRUPT;
}

OSRCODE
IMixerAsync::play()
{
	if (thread.CreateUserThread(nullptr, (ThreadFunc*)AsyncMixerThreadProc, this, L"OSR Async mixer worker"))
	{
		// set start event for mixer
		SetEvent(hStartThread);
		ResetEvent(hReleaseThread);

		return OSR_SUCCESS;
	}

	return MXR_OSR_NO_OUT;
}

OSRCODE
IMixerAsync::stop()
{
	SetEvent(hReleaseThread);
	ResetEvent(hStartThread);

	return OSR_SUCCESS;
}

IMixerAsync::IMixerAsync()
{
	pSound = new ISoundInterfaceWASAPI();
}
