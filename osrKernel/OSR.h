/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* OSR.h
* Master-include header
*********************************************************/
#pragma once

#include <string>
#include <vector>

#ifndef POSIX_PLATFORM
#include <windows.h>
#else
#include <dirent.h>

#ifdef OS_LINUX
#include <pthread.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#endif

#ifdef OS_MAC
#include <sys/sysctl.h>
#endif
#endif

#define	MAX_NUM_ARGVS			128
#define VIRTUAL_MEMORY_ALLOC	0xFF00FF00
#define HEAP_MEMORY_ALLOC		0x00FF00FF
#define MAPPED_MEMORY_ALLOC		0xF0F0F0F0

#define USE_FFMPEG				0x1
#define USE_LIBSNDFILE			0x2
#define USE_WMF					0x3

#ifdef WIN32
#define OSR_DECODER_NAME		L##"osrDecoder.dll"
#define OSR_MIXER_NAME			L##"osrMixer.dll"
#define ANSI_LOG(X)				OutputDebugStringA(X);		OutputDebugStringA("\n");
#define MSG_LOG(X)				OutputDebugStringW(L##X);	OutputDebugStringW(L"\n");
#define WMSG_LOG(X)				OutputDebugStringW(X);		OutputDebugStringW(L"\n");
#else
#define MSG_LOG(X)				printf(X); printf("\n");
#define WMSG_LOG(X)				MSG_LOG(X)
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef WIN32
using HANDLE = VOID*;
using LPVOID = VOID*;
using BOOL = int;
using WCHAR = WideChar;
using CHAR = char;
using LPWSTR = WideChar*;
using LPSTR = char*;
using LPCSTR = const char*;
using PCSTR = LPCSTR;
using LPCWSTR = const WideChar*;
using PCWSTR = LPCWSTR;
using INT = int;
using UINT = unsigned int;
using SHORT = short;
using WORD = unsigned short;
using LONG = long;
using DWORD = unsigned long;
using LONGLONG = long long;
using ULONGLONG = unsigned long long;
using DWORD64 = ULONGLONG;

typedef struct tWAVEFORMATEX
{
	WORD        wFormatTag;
	WORD        nChannels;
	DWORD       nSamplesPerSec;
	DWORD       nAvgBytesPerSec;
	WORD        nBlockAlign;
	WORD        wBitsPerSample;
	WORD        cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;

typedef struct waveformat_tag {
	WORD    wFormatTag;
	WORD    nChannels;
	DWORD   nSamplesPerSec; 
	DWORD   nAvgBytesPerSec; 
	WORD    nBlockAlign;    
} WAVEFORMAT, *PWAVEFORMAT, *LPWAVEFORMAT;
#endif

enum OSRCODE
{
	OSR_SUCCESS,

	FS_OSR_BAD_PATH,
	FS_OSR_BAD_ALLOC,
	FS_OSR_BAD_HANDLE,
	FS_OSR_BAD_PTR,
	FS_OSR_NO_SPACE,

	MXR_OSR_NO_FILE,
	MXR_OSR_BAD_WFX,
	MXR_OSR_NO_OUT,
	MXR_OSR_BAD_LIB,
	MXR_OSR_UNSUPPORTED_FMT,
	MXR_OSR_BUFFER_CORRUPT,

	DECODE_OSR_BAD_FILE,
	DECODE_OSR_BAD_PATH,
	DECODE_OSR_UNSUPPORTED_FMT,
	DECODE_OSR_BAD_WFX,
	DECODE_OSR_NO_ACCESS,
	DECODE_OSR_INTER_ERR,
	DECODE_OSR_MWF_FAILED,

	KERN_OSR_NO_LIB,
	KERN_OSR_BAD_ALLOC,
	KERN_OSR_BAD_MAP,
	KERN_OSR_BAD_STR,

	DX_OSR_BAD_DEVICE,
	DX_OSR_BAD_ARGUMENT,
	DX_OSR_BAD_HW,
	DX_OSR_NO_MEMORY
};

#ifdef WIN32
#ifndef USE_INT3
#define DEBUG_BREAK				DebugBreak()
#else
#define DEBUG_BREAK				__debugbreak()
#endif
#else
#define DEBUG_BREAK				__asm { int3; }
#endif

#ifdef WIN32
#ifdef ENGINE_EXPORTS
#define DLL_API					__declspec(dllexport)
#else
#define DLL_API					__declspec(dllimport)
#endif
#else
#define DLL_API					
#endif

#ifdef WIN32
// assert levels
VOID DLL_API ThrowCriticalError(LPCWSTR lpText);		// first level
BOOL DLL_API ThrowApplicationError(LPCWSTR lpText);		// second level
BOOL DLL_API ThrowDebugError(LPCWSTR lpText);			// third level
BOOL DLL_API ThrowWarning(LPCWSTR lpText);				// fourth level
#else
// assert levels
void ThrowCriticalError(const char* lpText);		// first level
bool ThrowApplicationError(const char* lpText);		// second level
bool ThrowDebugError(const char* lpText);			// third level
bool ThrowWarning(const char* lpText);				// fourth level
#endif

#define OSRSUCCEDDED(X)			(X == OSR_SUCCESS)
#define OSRFAILED(X)			(!OSRSUCCEDDED(X))

#define THROW1(X)				ThrowCriticalError(X)
#define THROW2(X)				ThrowApplicationError(X)
#define THROW3(X)				ThrowDebugError(X)
#define THROW4(X)				ThrowWarning(X)

#define OSRFAIL1(X, Y)			if (OSRFAILED(X)) { THROW1(Y); }
#ifdef WIN32
#define OSRFAIL2(X, Y)			if (OSRFAILED(X)) { if (THROW2(Y)) { return X; } else { DestroyApplication(DWORD(-2)); } }
#else
#define OSRFAIL2(X, Y)			if (OSRFAILED(X)) { if (THROW2(Y)) { return X; } else { exit(-1); } }
#endif
#ifdef DEBUG
#define OSRFAIL3(X, Y)			if (OSRFAILED(X)) { if (!THROW3(Y)) { return X; } else DEBUG_BREAK; }
#else
#define OSRFAIL3(X, Y)			if (OSRFAILED(X)) { if (THROW3(Y)) { return X; } }
#endif
#ifdef DEBUG
#define OSRFAIL4(X, Y)			if (OSRFAILED(X)) { if (!THROW4(Y)) { return X; } }
#else
#define OSRFAIL4(X, Y)			if (OSRFAILED(X)) { THROW4(Y); }
#endif

#define ASSERT1(X, Y)			if (!X) { THROW1(Y); }
#ifdef WIN32
#define ASSERT2(X, Y)			if (!X) { if (!THROW2(Y)) { DestroyApplication(DWORD(-2)); } }
#else
#define ASSERT2(X, Y)			if (!X) { if (!THROW2(Y)) { exit(-1); } }
#endif
#ifndef WITHOUT_WIDECHAR
#define OSRASSERT(X)			if (!X) { ASSERT2(X, L"OSRASSERT"); }
#else
#define OSRASSERT(X)			if (!X) { ASSERT2(X, "OSRASSERT"); }
#endif
#ifdef WIN32
#define _DEB(X)					if (FAILED(X)) { DEBUG_BREAK; }
#endif
#define _RELEASE(X)				if (X) { X->Release(); X = NULL; }

#include "OSRTypes.h"

using STRING_PATH				= char[MAX_PATH];
using STRING64					= char[64];
using STRING128					= char[128];
using STRING256					= char[256];
using STRING512					= char[512];
using STRING1024				= char[1024];

#ifndef WITHOUT_WIDECHAR
using WSTRING_PATH				= WideChar[MAX_PATH];
using WSTRING64					= WideChar[64];
using WSTRING128				= WideChar[128];
using WSTRING256				= WideChar[256];
using WSTRING512				= WideChar[512];
using WSTRING1024				= WideChar[1024];
#endif

using OSRHandle					= void*;

#ifdef WIN32
struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
typedef std::unique_ptr<void, handle_closer> SCOPE_HANDLE;

using UnhandledExceptionFilterType = LONG WINAPI(struct _EXCEPTION_POINTERS *pExceptionInfo);
extern DLL_API UnhandledExceptionFilterType *previous_filter;
extern DLL_API HANDLE hHeap;

#define FAILEDX1(X)	\
if (FAILED(X)) \
{ \
static WSTRING1024 szError = { NULL }; \
FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, X, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szError, 1023, NULL); \
THROW1(szError); \
}

#define FAILEDX2(X)	\
if (FAILED(X)) \
{ \
static WSTRING1024 szError = { NULL }; \
FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, X, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szError, 1023, NULL); \
if (!THROW2(szError)) { ExitProcess(X); } \
}
#endif

#ifndef WITHOUT_WIDECHAR
DLL_API LPCSTR WCSTRToMBCSTR(LPCWSTR lpString);
#endif

#ifdef WIN32
DLL_API VOID CreateKernelHeap();
DLL_API VOID DestroyKernelHeap();
DLL_API HANDLE GetKernelHeap();

DLL_API DWORD GetMainThreadId();
DLL_API VOID SuspendMainThread();
DLL_API VOID ResumeMainThread();

DLL_API BOOL IsNetworkInstalled();
DLL_API LPWSTR FormatError(LONG dwErrorCode);

DLL_API OSRCODE CreateNewFileWithData(LPCWSTR lpPath, LPVOID lpData, DWORD dwDataSize);
DLL_API OSRCODE ReadDataFromFile(LPCWSTR lpPath, VOID** lpOutData, LPDWORD lpOutSize);

DLL_API OSRCODE ReadAudioFileEx(LPCWSTR lpPath, VOID** lpData, LONGLONG* uSize, LPDWORD dwHeaderSize);
DLL_API OSRCODE WriteFileFromBuffer(LPCWSTR lpPath, BYTE* pFile, DWORD dwSize, WAVEFORMATEX* waveFormat);

DLL_API OSRCODE ReadAudioFile(LPCWSTR lpPath, VOID** lpData, DWORD* dwSizeWritten);
DLL_API OSRCODE OpenFileDialog(WSTRING_PATH* lpPath);
DLL_API BOOL GetDiskUsage(LARGE_INTEGER largeSize, LPCWSTR lpPath);
DLL_API BOOL UnloadToPage(LPVOID pData, SIZE_T DataSize);
#else
DLL_API OSRCODE ReadAudioFile(const char* lpPath, VOID** lpData, unsigned long long* dwSizeWritten);
DLL_API OSRCODE OpenFileDialog(STRING_PATH* lpPath);
DLL_API BOOL GetDiskUsage(size_t FileSize, const char* lpPath);
#endif

DLL_API VOID InitApplication();
DLL_API VOID DestroyApplication(DWORD dwError);

DLL_API OSRCODE GetWaveFormatExtented(BYTE* lpWaveFile, DWORD dwFileSize, WAVEFORMATEX* waveFormat);

#include "OSRAlloc.inl"

typedef struct  
{
	DWORD FileNumber;
	DWORD FileSize;
	WIN32_FIND_DATAW FindData;
} FILE_INFO;

struct WAV_RIFF_HEADER 
{
	DWORD	dwRIFFId;
	DWORD	dwFileSize;
};

struct WAV_FMT_HEADER 
{
	DWORD	dwFMTId;
	DWORD	dwFileSize;
	WORD	wAudioFormat;
	WORD	wChannels;
	DWORD	dwSampleRate;
	DWORD	dwByterate;
	WORD	wBlockAlign;
	WORD	wBits;
};

struct WAV_CHUNK_HEADER 
{
	DWORD	dwChunkId;
	DWORD	dwFileSize;
};

struct MP_WAVREADER_HEADER 
{
	char chunkId[4];
	unsigned long chunkSize;
	char format[4];
	char subchunk1Id[4];
	unsigned long subchunk1Size;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char subchunk2Id[4];
	unsigned long subchunk2Size;
};

constexpr unsigned int FOURCC_RIFF_TAG					= MAKEFOURCC('R', 'I', 'F', 'F');
constexpr unsigned int FOURCC_FORMAT_TAG				= MAKEFOURCC('f', 'm', 't', ' ');
constexpr unsigned int FOURCC_DATA_TAG					= MAKEFOURCC('d', 'a', 't', 'a');
constexpr unsigned int FOURCC_WAVE_FILE_TAG				= MAKEFOURCC('W', 'A', 'V', 'E');
constexpr unsigned int FOURCC_XWMA_FILE_TAG				= MAKEFOURCC('X', 'W', 'M', 'A');
constexpr unsigned int FOURCC_DLS_SAMPLE				= MAKEFOURCC('w', 's', 'm', 'p');
constexpr unsigned int FOURCC_MIDI_SAMPLE				= MAKEFOURCC('s', 'm', 'p', 'l');
constexpr unsigned int FOURCC_XWMA_DPDS					= MAKEFOURCC('d', 'p', 'd', 's');
constexpr unsigned int FOURCC_XMA_SEEK					= MAKEFOURCC('s', 'e', 'e', 'k');

#ifdef WIN32
#include <Shobjidl.h>
class TaskbarValue
{
public:
	TaskbarValue(HWND hwnd)
	{
		FAILEDX2(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pTaskbar)));

		currentHwnd = hwnd;

		pTaskbar->SetProgressState(currentHwnd, TBPF_NORMAL);
		pTaskbar->SetProgressValue(currentHwnd, 0, 100);
	}

	VOID SetValue(DWORD Value, DWORD MaxValue)
	{
		if (Value >= MaxValue)
		{
			pTaskbar->SetProgressState(currentHwnd, TBPF_NOPROGRESS);
		}

		pTaskbar->SetProgressValue(currentHwnd, Value, MaxValue);
		ThisValue = Value;
	}

	VOID SetError()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_ERROR);
		ThisValue = 0;

	}

	VOID SetPause()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_PAUSED);
		ThisValue = 0;
	}

	VOID SetCompleted()
	{
		pTaskbar->SetProgressState(currentHwnd, TBPF_NOPROGRESS);
		ThisValue = 0;
	}

	DWORD GetValue()
	{
		return ThisValue;
	}

	~TaskbarValue()
	{
		_RELEASE(pTaskbar);
	}

private:
	DWORD ThisValue;
	HWND currentHwnd;
	ITaskbarList3* pTaskbar;
};

DLL_API TaskbarValue** GetTaskbarPointer();
#endif

class IObject
{
public:
	virtual IObject* CloneObject() = 0;
	virtual void Release() = 0;

};

struct WAVE_EX : public WAVEFORMATEX
{

};

typedef enum
{
	AAC_TYPE,
	AIFF_TYPE,
	ALAC_TYPE,
	MP3_TYPE,
	FLAC_TYPE,
	OGG_TYPE,
	OPUS_TYPE,
	WMA_TYPE,
	WAV_TYPE,
	RAW_TYPE,
	UNKNOWN_TYPE
} FILE_TYPE;