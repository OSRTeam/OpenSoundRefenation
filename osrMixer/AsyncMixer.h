/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR Mixer
**********************************************************
* AsyncMixer.h
* Acync mixer implementation
*********************************************************/
#pragma once
#include "MixerBase.h"
#include "IMWASAPI.h"

class DLL_API IMixerAsync : public IMixerInterface
{
public:
	IMixerAsync(ISoundInterface* pSound1, AUDIO_HOST* HostsInfo1, TRACK_INFO* tracksInfo1, u32 EffectsNumber1, u32 TracksCount1)
	{
		hReleaseThread = CreateEventW(nullptr, TRUE, FALSE, nullptr);
		hStartThread = CreateEventW(nullptr, TRUE, FALSE, nullptr);
		hWaitThread = CreateEventW(nullptr, TRUE, FALSE, nullptr);

		pSound = (ISoundInterface*)pSound1->CloneObject();
		EffectsNumber = EffectsNumber1;
		TracksCount = TracksCount1;

		memcpy(HostsInfo, HostsInfo1, sizeof(AUDIO_HOST) * 2);
		memcpy(tracksInfo, tracksInfo1, sizeof(TRACK_INFO) * 256);
	}

	IMixerAsync();

	OSRCODE start(int Device) override;
	OSRCODE start_delay(int Device, f64 HostDelay) override;
	OSRCODE restart(int Device) override;
	OSRCODE restart_delay(int Device, f64 HostDelay) override;
	OSRCODE close() override;

	OSRCODE master_volume(f32 Volume) override;
	OSRCODE track_volume(u32 TrackNumber, f32& Volume) override;

	OSRCODE add_track(u8 Channels, u32 SampleRate, u32& TrackNumber) override;
	OSRCODE delete_track(u32 TrackNumber) override;

	OSRCODE add_effect(u32 TrackNumber, IObject* pEffectHost, size_t EffectSize, u32& EffectNumber) override;
	OSRCODE delete_effect(u32 TrackNumber, u32 EffectNumber) override;

	OSRCODE rout_solo(u32 TrackNumber, bool& isSolo) override;
	OSRCODE rout_mute(u32 TrackNumber, bool& isMuted) override;
	OSRCODE rout_effects(u32 TrackNumber, bool& isEffects) override;
	OSRCODE rout_activate(u32 TrackNumber, bool& isActivated) override;
	OSRCODE rout_channel(u32 TrackNumber, u32 ChannelRouting) override;
	OSRCODE rout_wide_image(u32 TrackNumber, f64 WideImaging) override;
	OSRCODE rout_track_number(u32& TrackNumber, u32 ToNumber) override;

	OSRCODE put_data(u32 TrackNumber, void* pData, size_t DataSize) override;

	OSRCODE play() override;
	OSRCODE stop() override;

	void Release() override
	{
		stop();
		close();

		for (TRACK_INFO & i : tracksInfo)
		{
			for (IObject* & ir : i.pEffectHost)
			{
				_RELEASE(ir);
			}
		}

		_RELEASE(pSound);
		delete this;
	}

	IObject* CloneObject() override
	{
		return new IMixerAsync(pSound, HostsInfo, tracksInfo, EffectsNumber, TracksCount);
	}

	HANDLE hReleaseThread;
	HANDLE hStartThread;
	HANDLE hWaitThread;
};
