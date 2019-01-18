/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR Mixer base 
**********************************************************
* MixerBase.h
* Master-include mixer header
*********************************************************/
#pragma once
#include "OSR.h"
#include "OSRSystem.h"
#ifdef WIN32
#include "ThreadSystem.h"
#endif

typedef enum
{
	SoundData,
	DeviceOutputData,
	DeviceInputData,
	DevicePacketData		// pack data with zlib
} PacketType;

typedef enum 
{
	AcceptData,
	DiscardData,
	StartRecording,
	StopRecording,
	FlushBuffers
} InputData;

typedef enum
{
	AcceptData2,
	DiscardData2,
	StartPlay2,	
	StopPlay2,				// is not equ to "StopHost" function: just put 0 if this packet was accepted
	FlushBuffers2
} OutputData;

typedef enum HostDeviceType
{
	InputDevice = 0,
	OutputDevice = 1,
	SpecialDevice = 2,		// for OSRHostProc.sys
	UnknownType = u8(-1)
};

typedef struct
{
	HostDeviceType DeviceType;
	HostFormatType FormatType;
	STRING256 DeviceName;		// UTF-8 only
	u32 BufferSize;				// in frames
	i32 DeviceNumber;			// current device number
} AUDIO_HOST;

typedef struct
{
	STRING256 szTrackName;
	IObject* pEffectHost[90];
	OSRSample* pData;
	size_t BufferSize;
	f64 WideImaging;			// M = (L + R) / 2		S = (L - R) / 2
	f32 GainLevel;
	u32 ChannelRouting;			// LOWORD - Left, HIWORD - Right
	u32 TrackNumber;
	u32 SampleRate;
	u16 Bits;
	u16 Channels;
	bool bMute;
	bool bSolo;
	bool bEffects;
	bool isActivated;
} TRACK_INFO;

/**************************************************
* RecvPacket() - Receive packet to output/input
***************************************************
* to send packet to input or output (like accept
* new data), include "DeviceInputData" or 
* "DeviceOutputData" packet type, and set pData 
* pointer to variable with current state.
***************************************************/
class ISoundInterface : public IObject
{
public:
	virtual OSRCODE EnumerateAudioInput(AUDIO_HOST** pHostsList, u32& HostCounts) = 0;
	virtual OSRCODE EnumerateAudioOutputs(AUDIO_HOST** pHostsList, u32& HostCounts) = 0;

	virtual OSRCODE GetHostDeviceInInfo(u32 DeviceNumber, AUDIO_HOST& HostInfo) = 0;
	virtual OSRCODE GetHostDeviceOutInfo(u32 DeviceNumber, AUDIO_HOST& HostInfo) = 0;
	virtual OSRCODE GetHostDevicesInCount(u32& DeviceCount) = 0;
	virtual OSRCODE GetHostDevicesOutCount(u32& DeviceCount) = 0;
	virtual OSRCODE GetVolumeLevel(f32& Volume) = 0;
	virtual OSRCODE GetDefaultDevice(u32& DeviceNumber) = 0;

	virtual OSRCODE SetDelayLevel(f64 HostDelay) = 0;
	virtual OSRCODE SetVolumeLevel(f32 Volume) = 0;

	virtual OSRCODE CreateRenderSoundDevice(f64 HostDelay, u32 DeviceNumber) = 0;
	virtual OSRCODE CreateCaptureSoundDevice(f64 HostDelay, u32 DeviceNumber) = 0;

	virtual OSRCODE CreateRenderDefaultSoundDevice(f64 HostDelay) = 0;
	virtual OSRCODE CreateCaptureDefaultSoundDevice(f64 HostDelay) = 0;

	virtual OSRCODE RestartRenderSoundDevice(f64 HostDelay, u32 DeviceNumber) = 0;
	virtual OSRCODE RestartCaptureSoundDevice(f64 HostDelay, u32 DeviceNumber) = 0;

	virtual OSRCODE RestartRenderDefaultSoundDevice(f64 HostDelay) = 0;
	virtual OSRCODE RestartCaptureDefaultSoundDevice(f64 HostDelay) = 0;

	virtual OSRCODE CloseRenderSoundDevice() = 0;
	virtual OSRCODE CloseCaptureSoundDevice() = 0;

	virtual OSRCODE PlayHost() = 0;
	virtual OSRCODE StopHost() = 0;

	virtual OSRCODE RecvPacket(void* pData, PacketType Type, size_t DataSize) = 0;
	virtual OSRCODE GetPacket(void*& pData, PacketType Type, size_t& DataSize) = 0;

	virtual OSRCODE GetLoadBuffer(void*& pData, size_t& BufferSize) = 0;

	AUDIO_HOST InputHost;
	AUDIO_HOST OutputHost;
};

class IMixerInterface : public IObject
{
public:
	virtual OSRCODE start(int Device) = 0;
	virtual OSRCODE start_delay(int Device, f64 HostDelay) = 0;
	virtual OSRCODE restart(int Device) = 0;
	virtual OSRCODE restart_delay(int Device, f64 HostDelay) = 0;
	virtual OSRCODE close() = 0;
	
	virtual OSRCODE master_volume(f32 Volume) = 0;
	virtual OSRCODE track_volume(u32 TrackNumber, f32& Volume) = 0;

	virtual OSRCODE add_track(u8 Channels, u32 SampleRate, u32& TrackNumber) = 0;
	virtual OSRCODE delete_track(u32 TrackNumber) = 0;

	virtual OSRCODE add_effect(u32 TrackNumber, IObject* pEffectHost, size_t EffectSize, u32& EffectNumber) = 0;
	virtual OSRCODE delete_effect(u32 TrackNumber, u32 EffectNumber) = 0;

	virtual OSRCODE rout_solo(u32 TrackNumber, bool& isSolo) = 0;
	virtual OSRCODE rout_mute(u32 TrackNumber, bool& isMuted) = 0;
	virtual OSRCODE rout_effects(u32 TrackNumber, bool& isEffects) = 0;
	virtual OSRCODE rout_activate(u32 TrackNumber, bool& isActivated) = 0;
	virtual OSRCODE rout_channel(u32 TrackNumber, u32 ChannelRouting) = 0;
	virtual OSRCODE rout_wide_image(u32 TrackNumber, f64 WideImaging) = 0;
	virtual OSRCODE rout_track_number(u32& TrackNumber, u32 ToNumber) = 0;

	virtual OSRCODE put_data(u32 TrackNumber, void* pData, size_t DataSize) = 0;

	virtual OSRCODE play() = 0;
	virtual OSRCODE stop() = 0;

	ThreadSystem thread;
	ISoundInterface* pSound;
	OSRSample* pData;
	AUDIO_HOST HostsInfo[2];		// 0 - Input, 1 - Output
	TRACK_INFO tracksInfo[256];
	u32 EffectsNumber;
	u32 TracksCount;
};

#ifdef WIN32
__forceinline 
HostFormatType 
ConvertToSingleFormat(
	WAVEFORMATEX waveFormat
)
{
	switch (waveFormat.nSamplesPerSec)
	{
	case 22050:
		switch (waveFormat.wBitsPerSample)
		{
		case 8:
			if (waveFormat.nChannels == 1)
			{
				return MONO_22050_U8;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_22050_U8;
			}
			break;

		case 16:
			if (waveFormat.nChannels == 1)
			{
				return MONO_22050_S16;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_22050_S16;
			}
			break;

		case 24:
			if (waveFormat.nChannels == 1)
			{
				return MONO_22050_S24;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_22050_S24;
			}
			break;

		case 32:
			if (waveFormat.wFormatTag == 1)
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_22050_S32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_22050_S32;
				}
			}
			else
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_22050_F32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_22050_F32;
				}
			}
			break;

		default:
			break;
		}
		break;
	case 44100:
		switch (waveFormat.wBitsPerSample)
		{
		case 8:
			if (waveFormat.nChannels == 1)
			{
				return MONO_44100_U8;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_44100_U8;
			}
			break;

		case 16:
			if (waveFormat.nChannels == 1)
			{
				return MONO_44100_S16;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_44100_S16;
			}
			break;

		case 24:
			if (waveFormat.nChannels == 1)
			{
				return MONO_44100_S24;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_44100_S24;
			}
			break;

		case 32:
			if (waveFormat.wFormatTag == 1)
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_44100_S32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_44100_S32;
				}
			}
			else
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_44100_F32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_44100_F32;
				}
			}
			break;

		default:
			break;
		}
		break;

	case 88200:
		switch (waveFormat.wBitsPerSample)
		{
		case 8:
			if (waveFormat.nChannels == 1)
			{
				return MONO_88200_U8;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_88200_U8;
			}
			break;

		case 16:
			if (waveFormat.nChannels == 1)
			{
				return MONO_88200_S16;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_88200_S16;
			}
			break;

		case 24:
			if (waveFormat.nChannels == 1)
			{
				return MONO_88200_S24;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_88200_S24;
			}
			break;

		case 32:
			if (waveFormat.wFormatTag == 1)
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_88200_S32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_88200_S32;
				}
			}
			else
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_88200_F32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_88200_F32;
				}
			}
			break;

		default:
			break;
		}
		break;

	case 96000:
		switch (waveFormat.wBitsPerSample)
		{
		case 8:
			if (waveFormat.nChannels == 1)
			{
				return MONO_96000_U8;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_96000_U8;
			}
			break;

		case 16:
			if (waveFormat.nChannels == 1)
			{
				return MONO_96000_S16;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_96000_S16;
			}
			break;

		case 24:
			if (waveFormat.nChannels == 1)
			{
				return MONO_96000_S24;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_96000_S24;
			}
			break;

		case 32:
			if (waveFormat.wFormatTag == 1)
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_96000_S32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_96000_S32;
				}
			}
			else
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_96000_F32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_96000_F32;
				}
			}
			break;

		default:
			break;
		}
		break;

	case 192000:
		switch (waveFormat.wBitsPerSample)
		{
		case 8:
			if (waveFormat.nChannels == 1)
			{
				return MONO_192000_U8;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_192000_U8;
			}
			break;

		case 16:
			if (waveFormat.nChannels == 1)
			{
				return MONO_192000_S16;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_192000_S16;
			}
			break;

		case 24:
			if (waveFormat.nChannels == 1)
			{
				return MONO_192000_S24;
			}
			else if (waveFormat.nChannels == 2)
			{
				return STEREO_192000_S24;
			}
			break;

		case 32:
			if (waveFormat.wFormatTag == 1)
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_192000_S32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_192000_S32;
				}
			}
			else
			{
				if (waveFormat.nChannels == 1)
				{
					return MONO_192000_F32;
				}
				else if (waveFormat.nChannels == 2)
				{
					return STEREO_192000_F32;
				}
			}
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
}

__forceinline
WAVEFORMATEX
ConvertToWaveFormat(
	HostFormatType HostFormat
)
{
	WAVEFORMATEX waveFormat = { 0 };
	switch (HostFormat)
	{
	case MONO_22050_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_22050_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_22050_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_22050_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_22050_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;
	case STEREO_22050_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_22050_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_22050_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_22050_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_22050_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 22050;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;

	case MONO_44100_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_44100_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_44100_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_44100_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_44100_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 3;
		break;
	case STEREO_44100_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_44100_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_44100_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_44100_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_44100_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;

	case MONO_48000_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_48000_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_48000_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_48000_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_48000_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;
	case STEREO_48000_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_48000_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_48000_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_48000_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_48000_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 48000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;

	case MONO_88200_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_88200_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_88200_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_88200_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_88200_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;
	case STEREO_88200_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_88200_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_88200_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_88200_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_88200_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 88200;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;

	case MONO_96000_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_96000_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_96000_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_96000_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_96000_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;
	case STEREO_96000_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_96000_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_96000_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_96000_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_96000_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 96000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;

	case MONO_192000_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_192000_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_192000_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_192000_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case MONO_192000_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 1;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;
	case STEREO_192000_U8:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 8;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_192000_S16:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 16;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_192000_S24:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 24;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_192000_S32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 1;
		break;
	case STEREO_192000_F32:
		waveFormat.cbSize = sizeof(WAVEFORMATEX);
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 192000;
		waveFormat.wBitsPerSample = 32;
		waveFormat.wFormatTag = 3;
		break;
	default:
		break;
	}

	waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nChannels * waveFormat.wBitsPerSample / 8;

	return waveFormat;
}
#endif