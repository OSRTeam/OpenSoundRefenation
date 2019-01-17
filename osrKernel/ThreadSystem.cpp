/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR Thread System
**********************************************************
* ThreadSystem.cpp
* thread system implementation
*********************************************************/
#include "stdafx.h"
#pragma hdrstop

#ifdef WIN32
#include <process.h>
#include <versionhelpers.h>

DLL_API ThreadSystem threadSystem;
LPWSTR lpNameString[4096];
NTQUERYINFORMATIONTHREAD pNtQueryInformationThread = nullptr;

LONG 
WINAPI
NtQueryInformationThreadEx(
	HANDLE hThread,
	__THREAD_INFORMATION_CLASS threadInfo,
	PVOID pThreadInfo,
	ULONG uSize,
	PULONG lpSize
)
{
	if (!pNtQueryInformationThread)
	{
		pNtQueryInformationThread = (NTQUERYINFORMATIONTHREAD)GetProcAddress(GetModuleHandleW(L"ntdll"), "NtQueryInformationThread");
		if (!pNtQueryInformationThread) { return 41; }
	}

	return pNtQueryInformationThread(hThread, threadInfo, pThreadInfo, uSize, lpSize);
}

VOID
WINAPIV
UserThreadProc(
	LPVOID pArgs
)
{
	// get info from struct
	THREAD_ENTRY* threadEntry = reinterpret_cast<THREAD_ENTRY*>(pArgs);
	LPWSTR lpNewThreadName = const_cast<LPWSTR>(threadEntry->lpThreadName);
	LPVOID pArglist = reinterpret_cast<LPVOID>(threadEntry->pArgs);
	ThreadFunc* threadFunc = threadEntry->pFunction;

	threadSystem.SetUserThreadName(GetCurrentThreadId(), lpNewThreadName);

	threadFunc(pArglist);
}

static THREAD_ENTRY threadStruct[4096] = { NULL };

DWORD
ThreadSystem::CreateUserThread(
	THREAD_INFO* pThreadInfo,
	ThreadFunc* pFunction,
	LPVOID pArgs,
	LPCWSTR pName
)
{
	EnterSection();

	ASSERT2(pFunction, L"No function pointer at CreateUserThread()");
	ASSERT2(pArgs, L"No thread args at CreateUserThread()");

	if (dwNumberOfThreads > 4095)
	{
		dwNumberOfThreads = 0;
	}
	else
	{
		dwNumberOfThreads++;
	}

	UINT_PTR hThread = NULL;
	DWORD dwThreadId = NULL;

	threadStruct[dwNumberOfThreads].lpThreadName = pName;
	threadStruct[dwNumberOfThreads].pArgs = pArgs;
	threadStruct[dwNumberOfThreads].pFunction = pFunction;

	if (pThreadInfo)
	{
		hThread = _beginthread(UserThreadProc, pThreadInfo->dwStackSize, &threadStruct[dwNumberOfThreads]);
		pThreadInfo->hThread = reinterpret_cast<HANDLE>(hThread);
		pThreadInfo->dwThreadId = GetThreadId(pThreadInfo->hThread);
		pThreadInfo->lpThreadName = const_cast<LPWSTR>(pName);
		pThreadInfo->cbSize = sizeof(THREAD_INFO);
		dwThreadId = pThreadInfo->dwThreadId;
	}
	else
	{
		hThread = _beginthread(UserThreadProc, NULL, &threadStruct[dwNumberOfThreads]);
		dwThreadId = GetThreadId(reinterpret_cast<HANDLE>(hThread));
	}

	LeaveSection();

	return dwThreadId;
}

VOID
ThreadSystem::SetUserThreadName(
	DWORD dwThreadId,
	LPWSTR lpName
)
{
	static SETTHREADDESCRIPTION pThreadDesc = nullptr;
	ASSERT2(dwThreadId, L"No thread id at SetUserThreadName");
	ASSERT2(lpName, L"No thread name at SetUserThreadName");

	// only from Windows 10.0.1607
	if (!pThreadDesc) { pThreadDesc = (SETTHREADDESCRIPTION)GetProcAddress(GetModuleHandleW(L"kernel32"), "SetThreadDescription"); }

	if (pThreadDesc)
	{
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
		ASSERT2(SUCCEEDED(pThreadDesc(hThread, lpName)), L"Can't set thread name");
		CloseHandle(hThread);
	}
	else
	{
		THREAD_NAME threadName = { NULL };
		threadName.dwType = 0x1000; 
		threadName.lpName = WCSTRToMBCSTR(lpName);
		threadName.dwThreadID = dwThreadId;
		threadName.dwFlags = NULL;

		__try { RaiseException(0x406D1388, 0, sizeof(threadName) / sizeof(ULONG_PTR), (ULONG_PTR*)&threadName); }
		__except (EXCEPTION_CONTINUE_EXECUTION) { }

		FREEKERNELHEAP(*((void**)&threadName.lpName));
	}
}

VOID
ThreadSystem::GetUserThreadName(
	DWORD dwThreadId,
	LPWSTR* lpName
)
{	
	static GETTHREADDESCRIPTION pThreadDesc = nullptr;
	ASSERT2(dwThreadId, L"No thread id at GetUserThreadName");

	// only from Windows 10.0.1607
	if (!pThreadDesc) { pThreadDesc = (GETTHREADDESCRIPTION)GetProcAddress(GetModuleHandleW(L"kernel32"), "GetThreadDescription"); }

	if (pThreadDesc)
	{
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
		ASSERT2(SUCCEEDED(pThreadDesc(hThread, lpName)), L"Can't get thread name");
		CloseHandle(hThread);
	}
}

VOID
ThreadSystem::GetThreadInformation(
	THREAD_INFO* pThreadInfo,
	DWORD dwThreadId
)
{
	ASSERT2(dwThreadId, L"No thread id at GetThreadInformation");

	// get handle to thread 
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
	THREAD_BASIC_INFORMATION basicInfo = { NULL };
	NT_TIB64 tib = { NULL };
	DWORD cbSize = sizeof(THREAD_INFO);

	// read basic info to our struct
	NtQueryInformationThreadEx(hThread, ThreadBasicInformation, &basicInfo, sizeof(THREAD_BASIC_INFORMATION), NULL);
	NtReadVirtualMemory(GetCurrentProcess(), basicInfo.TebBaseAddress, &tib, sizeof(NT_TIB64), NULL);

	static WSTRING128 lpThreadNameString;
	GetUserThreadName(dwThreadId, (LPWSTR*)&lpThreadNameString);

	//#NOTE: the pThreadInfo variable must exist to delete pointer
	if (!pThreadInfo) { return; }
	pThreadInfo->cbSize = cbSize;
	pThreadInfo->dwStackSize = (DWORD)ptrdiff_t(tib.StackBase - tib.StackLimit);
	pThreadInfo->dwThreadId = dwThreadId;
	pThreadInfo->hThread = hThread;
	pThreadInfo->lpThreadName = lpThreadNameString;

	CloseHandle(hThread);
}
#endif
