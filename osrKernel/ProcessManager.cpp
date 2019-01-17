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
#pragma hdrstop

#ifdef WIN32
DLL_API ProcessManager processManager;

NTQUERYINFORMATIONPROCESS pNtQueryInformationProcess = nullptr;
NTREADVIRTUALMEMORY pNtReadVirtualMemory = nullptr;

LONG 
WINAPI 
NtQueryInformationProcessEx(
	HANDLE hProcess,
	DWORD ProcessClass,
	PVOID processInfoPointer,
	ULONG uSize,
	PULONG lpSize
)
{
	if (!pNtQueryInformationProcess)
	{
		pNtQueryInformationProcess = (NTQUERYINFORMATIONPROCESS)GetProcAddress(GetModuleHandleW(L"ntdll"), "NtQueryInformationProcess");
		if (!pNtQueryInformationProcess) { return -41541; }
	}

	return pNtQueryInformationProcess(hProcess, ProcessClass, processInfoPointer, uSize, lpSize);
}

LONG 
WINAPI
NtReadVirtualMemory(
	HANDLE hProcess,
	PVOID pAddressToRead,
	PVOID pAddressToWrite,
	ULONG uSize,
	PULONG lpSize
)
{
	if (!pNtReadVirtualMemory)
	{
		pNtReadVirtualMemory = (NTREADVIRTUALMEMORY)GetProcAddress(GetModuleHandleW(L"ntdll"), "NtReadVirtualMemory");
		if (!pNtReadVirtualMemory) { return -41541; }
	}

	return pNtReadVirtualMemory(hProcess, pAddressToRead, pAddressToWrite, uSize, lpSize);
}

VOID
ProcessManager::GetUserProcessInfo(
	PROCESS_INFO* pPeb, 
	DWORD dwProcessId
)
{
	ASSERT2(dwProcessId, L"No process id at GetThreadInformation");

	// get procs to funcs
	HANDLE hProcess = NULL;
	___PROCESS_BASIC_INFORMATION ProcessBInfo = { NULL };
	RTL_USER_PROCESS_PARAMETERS processInfo = { NULL };
	static WCHAR szBuf[1024] = { NULL };
	
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (!hProcess) { return; }

	// read PEB by native API function
	NtQueryInformationProcessEx(hProcess, ProcessBasicInformation, &ProcessBInfo, sizeof(___PROCESS_BASIC_INFORMATION), NULL);
	NtReadVirtualMemory(hProcess, ProcessBInfo.PebBaseAddress, &pebStruct, sizeof(PEB64), NULL);

	// get data to struct
	pPeb->lpPEBBaseAddress = ProcessBInfo.PebBaseAddress;
	pPeb->cBeingDebugged = pebStruct.BeingDebugged;
	pPeb->dwSessionID = (DWORD)pebStruct.SessionId;
	pPeb->dwNumberOfHeaps = pebStruct.NumberOfHeaps;
	pPeb->lpAffinityMask = (LPVOID)pebStruct.ActiveProcessAffinityMask;
	pPeb->lpProcessHeap = (LPVOID)pebStruct.ProcessHeap;
	pPeb->lpProcessHeaps = (LPVOID)pebStruct.ProcessHeaps;
	
	// read cmd line
	NtReadVirtualMemory(hProcess, pebStruct.ProcessParameters, &processInfo, sizeof(RTL_USER_PROCESS_PARAMETERS), NULL);
	
	if (processInfo.CommandLine.Length)
	{
		NtReadVirtualMemory(hProcess, processInfo.CommandLine.Buffer, szBuf, processInfo.CommandLine.Length, NULL);	
	}

	pPeb->szArgs = szBuf;

	CloseHandle(hProcess);
}

/**********************************************
* #NOTE: Don't use bRunAsAdmin, if you
* don't use ShellExecuteEx method.
* CreateProcess() can be failed if you
* create process with admin rules.
**********************************************/
VOID
ProcessManager::CreateNewProcess(
	BOOL bUseShellExecute,
	BOOL bRunAsAdmin,
	LPCWSTR lpFullPathToModule, 
	LPCWSTR lpCmdLine,
	LPDWORD lpProcessId
)
{
	ASSERT2(lpFullPathToModule, L"No module path");

	HANDLE hProcess = NULL;
	STARTUPINFO startupInformation = { NULL };
	PROCESS_INFORMATION processInformation = { NULL };

	if (!bUseShellExecute)
	{
		BOOL bProcess = FALSE;
		bProcess = CreateProcessW(
			lpFullPathToModule,
			const_cast<LPWSTR>(lpCmdLine),
			NULL, 
			NULL, 
			FALSE, 
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&startupInformation,
			&processInformation
		);
		if (!bProcess) { return; }
	}
	else
	{
		SHELLEXECUTEINFOW shellInfo = { sizeof(SHELLEXECUTEINFOW) };

		if (bRunAsAdmin) { shellInfo.lpVerb = L"runas"; }
		else { shellInfo.lpVerb = NULL; }
		shellInfo.lpFile = lpFullPathToModule;
		shellInfo.lpParameters = lpCmdLine;
		shellInfo.hwnd = NULL;
		shellInfo.nShow = SW_NORMAL;
		shellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

		if (ShellExecuteExW(&shellInfo))
		{
			processInformation.hProcess = shellInfo.hProcess;
			if (!processInformation.hProcess) { return; }
			processInformation.dwProcessId = GetProcessId(processInformation.hProcess);
		}
	}

	*lpProcessId = processInformation.dwProcessId;
	dwNumberOfProcesses++;
}

VOID
ProcessManager::CreateSharedMemory(
	LPVOID* lpSharedMemory,
	LPVOID lpSharedData,
	DWORD dwSharedMemorySize,
	LPCWSTR lpSharedMemoryName
)
{
	// create file mapping at paging file
	HANDLE hSharedMemory = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, dwSharedMemorySize, lpSharedMemoryName);
	if (!hSharedMemory) { return; }
	*lpSharedMemory = MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, NULL, NULL, dwSharedMemorySize);

	// copy user data to shared memory
	if (*lpSharedMemory && lpSharedData) 
	{
		memcpy_s(*lpSharedMemory, dwSharedMemorySize, lpSharedData, dwSharedMemorySize);
	}
}

VOID
ProcessManager::GetUserSharedMemory(
	LPVOID* lpSharedMemory, 
	LPCWSTR lpSharedMemoryName,
	DWORD dwSharedMemorySize
)
{
	// open user file mapping
	HANDLE hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, lpSharedMemoryName);
	if (!hMapFile) { return; }
	*lpSharedMemory = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, NULL, NULL, dwSharedMemorySize);

	ASSERT2(*lpSharedMemory, L"Can't map pointer");
}

VOID 
ProcessManager::WriteUserSharedMemory(
	LPCWSTR lpSharedMemoryName,
	LPVOID lpSharedData,
	DWORD dwSharedMemorySizeToWrite
)
{
	// open file mapping from paging file
	HANDLE hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, lpSharedMemoryName);
	if (!hMapFile) { return; }
	LPVOID lpSharedMemory = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, NULL, NULL, dwSharedMemorySizeToWrite);

	// copy user data to shared memory
	if (lpSharedMemory && lpSharedData)
	{
		memcpy_s(lpSharedMemory, dwSharedMemorySizeToWrite, lpSharedData, dwSharedMemorySizeToWrite);
	}
}

#endif