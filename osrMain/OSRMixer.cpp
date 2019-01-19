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

	FILE_TYPE pType = UNKNOWN_TYPE;
	TRANSCODER_STRUCT pTc = { nullptr };
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

void
IOSRMixer::Play()
{
	pvMixer->play();
}

void 
IOSRMixer::Stop()
{
	pvMixer->stop();
}
