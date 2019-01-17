/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* ThreadSystem.h
* thread system implementation
*********************************************************/
#pragma once
#include "stdafx.h"

#ifdef WIN32
#include <subauth.h>

typedef enum __THREAD_INFORMATION_CLASS
{
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger
} ___THREAD_INFORMATION_CLASS, *__PTHREAD_INFORMATION_CLASS;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_LDR_DATA {
	BYTE Reserved1[8];
	PVOID Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef VOID (NTAPI *PPS_POST_PROCESS_INIT_ROUTINE) ( VOID );


typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;

typedef struct _PEB
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	ULONG ImageUsesLargePages : 1;
	ULONG IsProtectedProcess : 1;
	ULONG IsLegacyProcess : 1;
	ULONG IsImageDynamicallyRelocated : 1;
	ULONG SpareBits : 4;
	PVOID Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	PRTL_CRITICAL_SECTION FastPebLock;
	PVOID AtlThunkSListPtr;
	PVOID IFEOKey;
	ULONG CrossProcessFlags;
	ULONG ProcessInJob : 1;
	ULONG ProcessInitializing : 1;
	ULONG ReservedBits0 : 30;
	union
	{
		PVOID KernelCallbackTable;
		PVOID UserSharedInfoPtr;
	};
	ULONG SystemReserved[1];
	ULONG SpareUlong;
	DWORD FreeList[4];
	ULONG TlsExpansionCounter;
	PVOID TlsBitmap;
	ULONG TlsBitmapBits[2];
	PVOID ReadOnlySharedMemoryBase;
	PVOID HotpatchInformation;
	VOID * * ReadOnlyStaticServerData;
	PVOID AnsiCodePageData;
	PVOID OemCodePageData;
	PVOID UnicodeCaseTableData;
	ULONG NumberOfProcessors;
	ULONG NtGlobalFlag;
	LARGE_INTEGER CriticalSectionTimeout;
	ULONG HeapSegmentReserve;
	ULONG HeapSegmentCommit;
	ULONG HeapDeCommitTotalFreeThreshold;
	ULONG HeapDeCommitFreeBlockThreshold;
	ULONG NumberOfHeaps;
	ULONG MaximumNumberOfHeaps;
	VOID * * ProcessHeaps;
	PVOID GdiSharedHandleTable;
	PVOID ProcessStarterHelper;
	ULONG GdiDCAttributeList;
	PRTL_CRITICAL_SECTION LoaderLock;
	ULONG OSMajorVersion;
	ULONG OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	ULONG OSPlatformId;
	ULONG ImageSubsystem;
	ULONG ImageSubsystemMajorVersion;
	ULONG ImageSubsystemMinorVersion;
	ULONG ImageProcessAffinityMask;
	ULONG GdiHandleBuffer[34];
	PVOID PostProcessInitRoutine;
	PVOID TlsExpansionBitmap;
	ULONG TlsExpansionBitmapBits[32];
	ULONG SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	PVOID pShimData;
	PVOID AppCompatInfo;
	UNICODE_STRING CSDVersion;
	LPVOID ActivationContextData;
	LPVOID ProcessAssemblyStorageMap;
	LPVOID SystemDefaultActivationContextData;
	LPVOID SystemAssemblyStorageMap;
	ULONG MinimumStackCommit;
	LPVOID FlsCallback;
	LIST_ENTRY FlsListHead;
	PVOID FlsBitmap;
	ULONG FlsBitmapBits[4];
	ULONG FlsHighIndex;
	PVOID WerRegistrationData;
	PVOID WerShipAssertPtr;
} PEB, *PPEB;

typedef struct __TEB {

	NT_TIB                  Tib;
	PVOID                   EnvironmentPointer;
	CLIENT_ID               Cid;
	PVOID                   ActiveRpcInfo;
	PVOID                   ThreadLocalStoragePointer;
	PEB*                    Peb;
	ULONG                   LastErrorValue;
	ULONG                   CountOfOwnedCriticalSections;
	PVOID                   CsrClientThread;
	PVOID                   Win32ThreadInfo;
	ULONG                   Win32ClientInfo[0x1F];
	PVOID                   WOW32Reserved;
	ULONG                   CurrentLocale;
	ULONG                   FpSoftwareStatusRegister;
	PVOID                   SystemReserved1[0x36];
	PVOID                   Spare1;
	ULONG                   ExceptionCode;
	ULONG                   SpareBytes1[0x28];
	PVOID                   SystemReserved2[0xA];
	ULONG                   GdiRgn;
	ULONG                   GdiPen;
	ULONG                   GdiBrush;
	CLIENT_ID               RealClientId;
	PVOID                   GdiCachedProcessHandle;
	ULONG                   GdiClientPID;
	ULONG                   GdiClientTID;
	PVOID                   GdiThreadLocaleInfo;
	PVOID                   UserReserved[5];
	PVOID                   GlDispatchTable[0x118];
	ULONG                   GlReserved1[0x1A];
	PVOID                   GlReserved2;
	PVOID                   GlSectionInfo;
	PVOID                   GlSection;
	PVOID                   GlTable;
	PVOID                   GlCurrentRC;
	PVOID                   GlContext;
	NTSTATUS                LastStatusValue;
	UNICODE_STRING          StaticUnicodeString;
	WCHAR                   StaticUnicodeBuffer[0x105];
	PVOID                   DeallocationStack;
	PVOID                   TlsSlots[0x40];
	LIST_ENTRY              TlsLinks;
	PVOID                   Vdm;
	PVOID                   ReservedForNtRpc;
	PVOID                   DbgSsReserved[0x2];
	ULONG                   HardErrorDisabled;
	PVOID                   Instrumentation[0x10];
	PVOID                   WinSockData;
	ULONG                   GdiBatchCount;
	ULONG                   Spare2;
	ULONG                   Spare3;
	ULONG                   Spare4;
	PVOID                   ReservedForOle;
	ULONG                   WaitingOnLoaderLock;
	PVOID                   StackCommit;
	PVOID                   StackCommitMax;
	PVOID                   StackReserved;
} ___TEB, *__PTEB;

typedef LONG KPRIORITY;

typedef struct _THREAD_BASIC_INFORMATION 
{
	NTSTATUS ExitStatus;
	PVOID TebBaseAddress;
	CLIENT_ID ClientId;
	KAFFINITY AffinityMask;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct PBI {
	PVOID Reserved1;
	PVOID PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} ___PROCESS_BASIC_INFORMATION;

typedef struct _TEB {
	PVOID Reserved1[12];
	PPEB  ProcessEnvironmentBlock;
	PVOID Reserved2[399];
	BYTE  Reserved3[1952];
	PVOID TlsSlots[64];
	BYTE  Reserved4[8];
	PVOID Reserved5[26];
	PVOID ReservedForOle;
	PVOID Reserved6[4];
	PVOID TlsExpansionSlots;
} TEB, *PTEB;

typedef struct tagTHREAD_NAME
{
	DWORD dwType;
	LPCSTR lpName;
	DWORD dwThreadID;
	DWORD dwFlags;
} THREAD_NAME, PTHREAD_NAME;

typedef enum __PROCESSINFOCLASS {
	ProcessBasicInformation = 0,
	ProcessDebugPort = 7,
	ProcessWow64Information = 26,
	ProcessImageFileName = 27,
	ProcessBreakOnTermination = 29
} ___PROCESSINFOCLASS;

using SETTHREADDESCRIPTION = HRESULT(WINAPI *)(HANDLE handle, PCWSTR name);
using GETTHREADDESCRIPTION = HRESULT(WINAPI *)(HANDLE handle, PWSTR* name);
typedef LONG(NTAPI *NTQUERYINFORMATIONTHREAD)(HANDLE, __THREAD_INFORMATION_CLASS, PVOID, ULONG, PULONG);
typedef LONG(NTAPI *NTQUERYINFORMATIONPROCESS)(HANDLE, DWORD, PVOID, ULONG, PULONG);
typedef LONG(NTAPI *NTREADVIRTUALMEMORY)(HANDLE, PVOID, PVOID, ULONG, PULONG);
typedef void ThreadFunc(void *);

typedef struct tagTHREAD_ENTRY
{
	LPVOID pArgs;
	ThreadFunc* pFunction;
	LPCWSTR lpThreadName;
	_TEB* funcTeb;
} THREAD_ENTRY, PTHREAD_ENTRY;

typedef struct tagTHREAD_INFO
{
	DWORD cbSize;
	DWORD dwThreadId;
	DWORD dwStackSize;
	HANDLE hThread;
	LPWSTR lpThreadName;
} THREAD_INFO, *PTHREAD_INFO;

DLL_API VOID WINAPIV UserThreadProc(LPVOID pArgs);
LONG WINAPI NtQueryInformationThreadEx(HANDLE hThread, __THREAD_INFORMATION_CLASS threadInfo, PVOID pThreadInfo, ULONG uSize, PULONG lpSize);

class DLL_API ThreadSystem
{
public:
	ThreadSystem() 
	{ 
		dwNumberOfThreads = NULL; 
		ASSERT2(InitializeCriticalSectionAndSpinCount(&CriticalSection, 0x00000400), L"Can't create critical section.")
	}

	~ThreadSystem()
	{
		DeleteCriticalSection(&CriticalSection);
	}

	DWORD GetThreadsNumber() { return dwNumberOfThreads; }
	DWORD CreateUserThread(THREAD_INFO* pThreadInfo, ThreadFunc* pFunction, LPVOID pArgs, LPCWSTR pName);

	VOID GetUserThreadName(DWORD dwThreadId, LPWSTR* lpName);
	VOID SetUserThreadName(DWORD dwThreadId, LPWSTR lpName);
	VOID GetThreadInformation(THREAD_INFO* pThreadInfo, DWORD dwThreadId);

	VOID EnterSection() { EnterCriticalSection(&CriticalSection); }
	VOID LeaveSection() { LeaveCriticalSection(&CriticalSection); }

private:
	DWORD dwNumberOfThreads;
	CRITICAL_SECTION CriticalSection;
};

extern DLL_API ThreadSystem threadSystem;

#endif
