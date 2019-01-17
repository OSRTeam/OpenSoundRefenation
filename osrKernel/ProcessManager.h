/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR Process Manager
**********************************************************
* ProcessManager.cpp
* Process Manager implementation
*********************************************************/
#include "stdafx.h"
#include "ThreadSystem.h"
#pragma once

#ifdef WIN32
typedef struct tagPROCESS_INFO
{
	LPVOID	lpProcessHeap;
	LPVOID	lpProcessHeaps;
	LPVOID	lpPEBBaseAddress;
	LPVOID	lpAffinityMask;
	LPWSTR	szArgs;
	DWORD	dwNumberOfHeaps;
	DWORD	dwProcessId;
	DWORD	dwParentProcessID;
	DWORD	dwSessionID;
	LONG	dwBasePriority;
	LONG	dwExitStatus;
	BYTE	cBeingDebugged;
} PROCESS_INFO;

template <class T>
struct LIST_ENTRY_T
{
	T Flink;
	T Blink;
};

template <class T>
struct UNICODE_STRING_T
{
	union
	{
		struct
		{
			WORD Length;
			WORD MaximumLength;
		};
		T dummy;
	};
	T _Buffer;
};

template <class T, class NGF, int A>
struct _PEB_T
{
	union
	{
		struct
		{
			BYTE InheritedAddressSpace;
			BYTE ReadImageFileExecOptions;
			BYTE BeingDebugged;
			BYTE _SYSTEM_DEPENDENT_01;
		};
		T dummy01;
	};
	T Mutant;
	T ImageBaseAddress;
	T Ldr;
	T* ProcessParameters;	//_RTL_USER_PROCESS_PARAMETERS*
	T SubSystemData;
	T ProcessHeap;
	T FastPebLock;
	T _SYSTEM_DEPENDENT_02;
	T _SYSTEM_DEPENDENT_03;
	T _SYSTEM_DEPENDENT_04;
	union
	{
		T KernelCallbackTable;
		T UserSharedInfoPtr;
	};
	DWORD SystemReserved;
	DWORD _SYSTEM_DEPENDENT_05;
	T _SYSTEM_DEPENDENT_06;
	T TlsExpansionCounter;
	T TlsBitmap;
	DWORD TlsBitmapBits[2];
	T ReadOnlySharedMemoryBase;
	T _SYSTEM_DEPENDENT_07;
	T ReadOnlyStaticServerData;
	T AnsiCodePageData;
	T OemCodePageData;
	T UnicodeCaseTableData;
	DWORD NumberOfProcessors;
	union
	{
		DWORD NtGlobalFlag;
		NGF dummy02;
	};
	LARGE_INTEGER CriticalSectionTimeout;
	T HeapSegmentReserve;
	T HeapSegmentCommit;
	T HeapDeCommitTotalFreeThreshold;
	T HeapDeCommitFreeBlockThreshold;
	DWORD NumberOfHeaps;
	DWORD MaximumNumberOfHeaps;
	T ProcessHeaps;
	T GdiSharedHandleTable;
	T ProcessStarterHelper;
	T GdiDCAttributeList;
	T LoaderLock;
	DWORD OSMajorVersion;
	DWORD OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	DWORD OSPlatformId;
	DWORD ImageSubsystem;
	DWORD ImageSubsystemMajorVersion;
	T ImageSubsystemMinorVersion;
	union
	{
		T ImageProcessAffinityMask;
		T ActiveProcessAffinityMask;
	};
	T GdiHandleBuffer[A];
	T PostProcessInitRoutine;
	T TlsExpansionBitmap;
	DWORD TlsExpansionBitmapBits[32];
	T SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	T pShimData;
	T AppCompatInfo;
	UNICODE_STRING_T<T> CSDVersion;
	T ActivationContextData;
	T ProcessAssemblyStorageMap;
	T SystemDefaultActivationContextData;
	T SystemAssemblyStorageMap;
	T MinimumStackCommit;
};
typedef _PEB_T<DWORD64, DWORD, 30> PEB64;

LONG WINAPI NtQueryInformationProcessEx(HANDLE hProcess, DWORD ProcessClass, PVOID processInfoPointer, ULONG uSize, PULONG lpSize);
LONG WINAPI NtReadVirtualMemory(HANDLE hProcess, PVOID pAddressToRead, PVOID pAddressToWrite, ULONG uSize, PULONG lpSize);

class DLL_API ProcessManager
{
public:
	VOID CreateNewProcess(BOOL bUseShellExecute, BOOL bRunAsAdmin, LPCWSTR lpFullPathToModule, LPCWSTR lpCmdLine, LPDWORD lpProcessId);
	VOID CreateSharedMemory(LPVOID* lpSharedMemory, LPVOID lpSharedData, DWORD dwSharedMemorySize, LPCWSTR lpSharedMemoryName);
	DWORD GetProcessesNumber() { return dwNumberOfProcesses; }
	VOID GetUserSharedMemory(LPVOID* lpSharedMemory, LPCWSTR lpSharedMemoryName, DWORD dwSharedMemorySize);
	VOID GetUserProcessInfo(PROCESS_INFO* pPeb, DWORD dwProcessId);
	VOID WriteUserSharedMemory(LPCWSTR lpSharedMemoryName, LPVOID lpSharedData, DWORD dwSharedMemorySizeToWrite);

private:
	DWORD dwNumberOfProcesses = 0;
	PEB64 pebStruct = { 0 };
};

extern DLL_API ProcessManager processManager;
#endif
