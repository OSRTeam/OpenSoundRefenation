#include "stdafx.h"
#include "IMWASAPI.h"
#include "WaveOutHeader.h"
#include "DiscordPart.h"

DiscordNetwork* pNetwork;
HANDLE hStartEvent;

extern DLL_API bool IsBlur;
extern DLL_API bool IsLoad; 

typedef struct TRANSCODER_STRUCT
{
	const char* InputFile;
	const char* OutputFile;
	IObject* pThis;
	i32* pNum;
};

VOID
WINAPIV
TranscoderFileProc(
	LPVOID pProc
)
{
	IsLoad = true;

	TRANSCODER_STRUCT* pc = (TRANSCODER_STRUCT*)(pProc);
	IOSRDecoder* pDecode = (IOSRDecoder*)pc->pThis;

	if (pc->InputFile)
	{
		pDecode->pList->load(pc->InputFile, *pc->pNum);
	}
	else if (pc->OutputFile)
	{
		//#TODO: 
	}
	else
	{
		return;
	}

	SetEvent(hStartEvent);
	IsLoad = false;
} 

void DecodeObject(const char* pInFile, i32& OutFile);
void EncodeObject(i32 inFile, const char* pOutFile);

void
IOSRDecoder::DecodeObject(
	const char* pInFile, 
	i32& OutFile
)
{
	static ThreadSystem threadS = {};
	static TRANSCODER_STRUCT pTc = { nullptr };
	pTc.InputFile = pInFile;
	pTc.pThis = this;
	pTc.pNum = &OutFile;

	hStartEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	threadS.CreateUserThread(nullptr, TranscoderFileProc, &pTc, L"OSR Decoder Worker");
}

void
IOSRDecoder::EncodeObject(
	i32 inFile,
	const char* pOutFile
)
{

}

void 
IOSRDecoder::RemoveObject(i32 Object)
{
	pList->unload(Object);
}

void
IOSRMixer::CreateMixer(f64 Delay)
{
	pvMixer->start_delay(-1, Delay);
}

void 
IOSRMixer::DestroyMixer()
{
	Stop();
	pvMixer->close();
}


void
IOSRMixer::RestartMixer(f64 Delay)
{
	DestroyMixer();
	CreateMixer(Delay);
}

void
IOSRMixer::OpenPlugin(u32 Track, u32 Effect)
{
	if (pvMixer && pvMixer->tracksInfo[Track].pEffectHost[Effect])
	{
		IWin32VSTHost* pHost = (IWin32VSTHost*)pvMixer->tracksInfo[Track].pEffectHost[Effect];
		pHost->OpenPluginWindow();
	}
}

void 
IOSRMixer::ClosePlugin(u32 Track, u32 Effect)
{
	if (pvMixer && pvMixer->tracksInfo[Track].pEffectHost[Effect])
	{
		IWin32VSTHost* pHost = (IWin32VSTHost*)pvMixer->tracksInfo[Track].pEffectHost[Effect];
		pHost->ClosePluginWindow();
	}
}

void
IOSRMixer::AddPlugin(u32 Track, IObject* pPlugin, u32& OutEffect)
{
	pvMixer->add_effect(Track, pPlugin, sizeof(IWin32VSTHost), OutEffect);
}

void
IOSRMixer::DeletePlugin(u32 Track, u32 Effect)
{
	pvMixer->delete_effect(Track, Effect);
}

void
IOSRMixer::SetPosition(f32 Position)
{

}

DWORD 
WINAPIV
PlayProc(LPVOID pProc)
{
	bool play = true;
	IOSRMixer* pMixer = (IOSRMixer*)pProc;
	HANDLE hArray[] = { pMixer->StartHandle, pMixer->EndHandle };
	void* pData = nullptr;
	u64 SamplePosition = 0;
	i32 ToEndFileSize = 0;
	u32 BufSize = pMixer->pvMixer->HostsInfo[1].BufferSize;

	if (!pMixer->isPlay)
	{
		WAVE_EX wf = *(WAVE_EX*)&ConvertToWaveFormat(pMixer->pvMixer->HostsInfo[1].FormatType);
		pMixer->pvMixer->add_track(wf.nChannels, wf.nSamplesPerSec, pMixer->TrackNum);
		pMixer->pvMixer->play();
	}

	WaitForSingleObject(pMixer->pvMixer->hStartThread, INFINITE);

	while (play)
	{
		switch (WaitForMultipleObjects(2, hArray, FALSE, INFINITE))
		{
		case WAIT_OBJECT_0:
			if (WaitForSingleObject(pMixer->pvMixer->hWaitThread, 1) == WAIT_TIMEOUT)
			{
				if (!ToEndFileSize && !SamplePosition)
				{
					ToEndFileSize = pMixer->pDecoder->pList->lpFileInfo[pMixer->pDecoder->NumberOfTrack].FileSize;
				}

				pData = (void*)ptrdiff_t(pMixer->pDecoder->pList->lpFileInfo[pMixer->pDecoder->NumberOfTrack].pSampleInfo->pSample + SamplePosition);

				SamplePosition += (BufSize * sizeof(f32));
				ToEndFileSize -= (BufSize * sizeof(f32));

				if (ToEndFileSize <= 0)
				{
					play = false;
				}

				if (ToEndFileSize < BufSize * sizeof(f32))
				{
					pMixer->pvMixer->put_data(pMixer->TrackNum, pData, ToEndFileSize);
				}
				else
				{
					pMixer->pvMixer->put_data(pMixer->TrackNum, pData, BufSize * sizeof(f32));
				}

				SetEvent(pMixer->pvMixer->hWaitThread);
			}
			break;
		case WAIT_OBJECT_0 + 1:
			play = false;
		default:
			break;
		}
	}

	pMixer->pvMixer->stop();
	pMixer->pvMixer->delete_track(pMixer->TrackNum);

	return 0;
}

void
IOSRMixer::Play()
{
	thread.CreateUserThread(nullptr, (ThreadFunc*)PlayProc, this, L"OSR Mixer thread");

	SetEvent(StartHandle);
	ResetEvent(EndHandle);
}

void 
IOSRMixer::Stop()
{
	if (isPlay)
	{
		SetEvent(EndHandle);
		ResetEvent(StartHandle);
	}
}
