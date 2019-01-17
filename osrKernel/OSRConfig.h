/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR config
**********************************************************
* OSRConfig.h
* Config system implementation
*********************************************************/
#pragma once
#include "OSR.h"

typedef struct  
{
	DWORD dwMagic;					// magic 'OCFG' dword (0x4746434F)
	DWORD dwParamsCount;			// count of parameters
	DWORD dwParamsTreeCount;		// reserved
	STRING64 szConfigName;			// 64 - 5 (size of '.ocfg' string) = 59 max symbols for name
} CONFIG_DATA;

typedef struct  
{
	CHAR cbParamName[32];			// parameter name
	BYTE pData[1024];				// static 1024 byte data array
	DWORD dwSharedMemorySize;		// size of shared memory (if 0 - without shared memory)
	LPVOID pSharedMemory;			// pointer to shared memory (in file - first 8 bytes of data)
} PARAMS_DATA;

typedef struct  
{
	BYTE Type;

	f16 Float16Array[512];			// 1
	f32 Float32Array[256];			// 2
	f64 Float64Array[128];			// 3

	CHAR AnsiString[1024];			// 4
	WCHAR WideString[512];			// 5

	u8 ByteArray[1024];				// 6
	u16 WordArray[512];				// 7
	u24 ThreeByteArray[320];		// 8
	u32 DwordArray[256];			// 9
	u64 QwordArray[128];			// 10

	BIT_BYTE BitBiteArray[1024];	// 11

} SETTINGS_DATA;

typedef struct  
{
	RECT WindowRect;				// 4x4 bytes

} UI_SETTINGS;

/****************************************************
* BoolSettings[0] - primary (output info)
*
* first 2 bits - type of output
* 00 - WASAPI
* 01 - WaveOut
* 10 - DirectX
* 11 - ASIO
*
* 3-rd bit - with VST host 
* 0 - no
* 1 - yes
*
* 4-th bit - enable multiple VST Hosts (DAW mode)
* 0 - no
* 1 - yes
*
* 5-th bit - exclusive mode (WASAPI-only)
* 0 - disable
* 1 - enable
*
* 6-th bit - allocator type
* 0 - use HeapAlloc (needy for medium buffers)
* 1 - use VirtualAlloc (needy for large buffers)
*
* last 2 bits - type of window (for effects)
* 00 - Hann window
* 01 - Hamming window
* 10 - Blackman window
* 11 - Blackman-Harris window
*
***************************************************
* BoolSettings[1] - primary (main settings)
*
* 1-st bit - temp folder settings
* 0 - use custom temp folder (at working directory)
* 1 - use windows temp folder
*
* 2-nd bit - use FFMpeg (deprecated)
* 0 - no
* 1 - yes
*
* 3-rd bit - DirectX device
* 0 - D3D11
* 1 - D3D9 (legacy)
*
* 4-th bit - always run as administrator
* 0 - no
* 1 - yes
*
* 5-th bit - use UTF-8 
* 0 - no
* 1 - yes
*
* 6-th bit - hide alt menu
* 0 - no
* 1 - yes
*
* 7-th bit - restart device every time
* 0 - no
* 1 - yes
*
* 8-th bit - almost top VST plugin window
* 0 - no 
* 1 - yes
*
***************************************************
* BoolSettings[2] - system
*
* 1-st bit - use network library
* 0 - no
* 1 - yes
*
* 2-nd bit - use plugin library
* 0 - no
* 1 - yes
*
***************************************************/
typedef struct  
{
	u64 MaxLocalBufferSize;			// 8 bytes
	BIT_BYTE BoolSettings[4];		// 4 bytes
	u32 LastKnownSampleRate;		// 4 bytes
	u32 LastKnownBitsRate;			// 4 bytes
	u32 LastKnownDeviceDelay;		// 4 bytes
	f32 LastKnownVolume;			// 4 bytes

	WSTRING_PATH PathToVST;			// 520 bytes

} INTERNAL_SETTINGS;

DLL_API OSRCODE GetConfigPath(LPCWSTR lpPath);
DLL_API OSRCODE ConvertToPath(LPCSTR lpConfig, LPCWSTR lpPath);
DLL_API BOOL IsConfigExist(LPCSTR lpConfig);

DLL_API OSRCODE CreateConfig(LPCSTR lpConfigName, LPVOID lpConfigData = NULL, DWORD DataSize = 0);
DLL_API OSRCODE WriteConfig(LPCSTR lpConfigName, LPVOID lpConfigData, DWORD DataSize);
DLL_API OSRCODE OpenConfig(LPCSTR lpConfigName, VOID** lpConfigData, LPDWORD DataSize);
DLL_API OSRCODE DeleteConfig(LPCSTR lpConfigName);

DLL_API OSRCODE CreateParameter(LPCSTR lpConfigName, LPCSTR lpParamName, PARAMS_DATA* ParamsData);
DLL_API OSRCODE WriteParameter(LPCSTR lpConfigName, LPCSTR lpParamName, PARAMS_DATA* ParamsData);
DLL_API OSRCODE OpenParameter(LPCSTR lpConfigName, LPCSTR lpParamName, PARAMS_DATA* ParamsData);
DLL_API OSRCODE DeleteParameter(LPCSTR lpConfigName, LPCSTR lpParamName);

class ConfigSystem
{
private:
	OSRCODE WriteParam(LPCSTR Param, LPVOID Data, BYTE Type, size_t Size)
	{
		PARAMS_DATA paramData = { 0 };
		OSRCODE sCode = OSR_SUCCESS;

		thsys.EnterSection();
		switch (Type)
		{
		case 1:
			memcpy(paramData.pData, Data, Size * sizeof(f16));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 2:
			memcpy(paramData.pData, Data, Size * sizeof(f32));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 3:
			memcpy(paramData.pData, Data, Size * sizeof(f64));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 4:
			memcpy(paramData.pData, Data, Size * sizeof(CHAR));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 5:
			memcpy(paramData.pData, Data, Size * sizeof(WCHAR));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 6:
			memcpy(paramData.pData, Data, Size * sizeof(u8));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 7:
			memcpy(paramData.pData, Data, Size * sizeof(u16));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 8:
			memcpy(paramData.pData, Data, Size * sizeof(u24));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 9:
			memcpy(paramData.pData, Data, Size * sizeof(u32));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;


		case 10:
			memcpy(paramData.pData, Data, Size * sizeof(u64));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;

		case 11:
			memcpy(paramData.pData, Data, Size * sizeof(BIT_BYTE));

			sCode = WriteParameter(ConfigData.szConfigName, Param, &paramData);
			break;
		}

		thsys.LeaveSection();

		return sCode;
	}

public:
	ConfigSystem()
	{
		memset((LPVOID)ConfigData.szConfigName, 0, sizeof(STRING64));
	}

	ConfigSystem(LPCSTR Config)
	{
		memset((LPVOID)ConfigData.szConfigName, 0, sizeof(STRING64));
		strcpy_s(ConfigData.szConfigName, strlen(Config), Config);
		CreateConfig(Config);
	}

	OSRCODE LoadConfig(LPCSTR Config)
	{
		try
		{
			strcpy_s(ConfigData.szConfigName, strlen(Config), Config);
			return CreateConfig(Config);
		}
		catch (const std::exception& exc)
		{
			STRING256 szLog = { 0 };

			SuspendMainThread();
			_snprintf_s(szLog, 256, "The application throw exception with info: %s \n%s", exc.what(), "Please, open minidump file and send on our GitHub.");
			MessageBoxA(NULL, szLog, "Application throw exception", MB_OK | MB_ICONERROR);
			UnhandledExceptionFilter((_EXCEPTION_POINTERS*)_exception_info);
			ResumeMainThread();
		}

		return OSR_SUCCESS;
	}

	OSRCODE create()
	{
		return CreateConfig(ConfigData.szConfigName);
	}

	OSRCODE new_param(LPCSTR ParamName, void* Data, size_t size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Data, size);

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, f16 Float)
	{
		PARAMS_DATA Param = { 0 };
		f16* pV = (f16*)Param.pData;
		(*pV) = Float;

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, f32 Float)
	{
		PARAMS_DATA Param = { 0 };
		f32* pV = (f32*)Param.pData;
		(*pV) = Float;

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, f64 Float)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, &Float, sizeof(f64));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, f16* Float, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Float, Size * sizeof(f16));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, f32* Float, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Float, Size * sizeof(f32));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, f64* Float, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Float, Size * sizeof(f64));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u8 Byte)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, &Byte, sizeof(u8));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u16 Word)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, &Word, sizeof(u16));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u24 Data)
	{
		PARAMS_DATA Param = { 0 };

		u24* pV = (u24*)Param.pData;
		(*pV) = Data;

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u32 Dword)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, &Dword, sizeof(u32));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u64 Qword)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, &Qword, sizeof(u64));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u8* Byte, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Byte, Size * sizeof(u8));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u16* Word, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Word, Size * sizeof(u16));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u32* Dword, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Dword, Size * sizeof(u32));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, u64* Qword, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Qword, Size * sizeof(u64));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, CHAR* Data, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Data, Size * sizeof(CHAR));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, WCHAR* Data, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Data, Size * sizeof(WCHAR));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, BIT_BYTE Data)
	{
		PARAMS_DATA Param = { 0 };
		BIT_BYTE* pV = (BIT_BYTE*)Param.pData;
		(*pV) = Data;

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE new_param(LPCSTR ParamName, BIT_BYTE* Data, size_t Size)
	{
		PARAMS_DATA Param = { 0 };
		memcpy(Param.pData, Data, Size * sizeof(BIT_BYTE));

		return CreateParameter(ConfigData.szConfigName, ParamName, &Param);
	}

	OSRCODE edit_param(LPCSTR Param, void* Data, size_t Size, BYTE Type)
	{
		return WriteParam(Param, Data, Type, Size);
	}

	OSRCODE open_param(LPCSTR Param, void* Data, size_t DataSizeInBytes)
	{
		PARAMS_DATA Params = { 0 };
		OSRCODE sCode = OSR_SUCCESS;

		if (OSRFAILED((sCode = OpenParameter(ConfigData.szConfigName, Param, &Params))))
		{
			return sCode;
		}

		memcpy(Data, Params.pData, DataSizeInBytes);

		return OSR_SUCCESS;
	}

private:
	CONFIG_DATA ConfigData;
	ThreadSystem thsys;
};
