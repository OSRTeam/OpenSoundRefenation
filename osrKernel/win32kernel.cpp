/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* win32kernel.cpp
* WIN32 kernel implementation
*********************************************************/
#include "stdafx.h"

#ifdef WIN32
#include <DbgHelp.h>

DLL_API UnhandledExceptionFilterType *previous_filter = nullptr;

BOOL
IsAdminUser()
{
	BOOL isAdmin = FALSE;
	DWORD dwTokenSize = NULL;
	DWORD dwSecondSize = NULL;
	HANDLE hProcessToken = nullptr;
	HANDLE hSecondToken = nullptr;
	BYTE adminSID[SECURITY_MAX_SID_SIZE];
	TOKEN_ELEVATION_TYPE tokenValue = {};
	ZeroMemory(&tokenValue, sizeof(TOKEN_ELEVATION_TYPE));

	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &hProcessToken);
	GetTokenInformation(hProcessToken, TokenElevationType, &tokenValue, sizeof(TOKEN_ELEVATION_TYPE), &dwTokenSize);

	// if is our token is limited
	if (tokenValue == TokenElevationTypeLimited)
	{
		GetTokenInformation(hProcessToken, TokenLinkedToken, &hSecondToken, sizeof(HANDLE), &dwTokenSize);
	}

	// if second token is empty - duplicate it from process token
	if (!hSecondToken)
	{
		DuplicateToken(hProcessToken, SecurityIdentification, &hSecondToken);
	}

	dwSecondSize = sizeof(adminSID);

	// check user token
	CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, &adminSID, &dwSecondSize);
	CheckTokenMembership(hSecondToken, &adminSID, &isAdmin);

	// clean handles
	if (hProcessToken)
	{
		CloseHandle(hProcessToken);
		hProcessToken = nullptr;
	}
	if (hProcessToken)
	{
		CloseHandle(hSecondToken);
		hSecondToken = nullptr;
	}
	return isAdmin;
}

BOOL
IsProcessWithAdminPrivilege()
{
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	LPVOID pAdministratorsGroup = nullptr;
	BOOL bRet = FALSE;

	// init SID to control privileges
	AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdministratorsGroup);

	// ckeck membership
	CheckTokenMembership(nullptr, pAdministratorsGroup, &bRet);

	// clean pointer
	if (pAdministratorsGroup) { FreeSid(pAdministratorsGroup); pAdministratorsGroup = nullptr; }

	return bRet;
}

VOID
RunWithAdminPrivilege()
{
	if (!IsProcessWithAdminPrivilege())
	{
		WSTRING_PATH szPath = { NULL };
		if (GetModuleFileNameW(nullptr, szPath, ARRAYSIZE(szPath)))
		{
			SHELLEXECUTEINFOW shellInfo = { sizeof(SHELLEXECUTEINFOW) };
			shellInfo.lpVerb = L"runas";
			shellInfo.lpFile = szPath;
			shellInfo.hwnd = nullptr;
			shellInfo.nShow = SW_NORMAL;

			if (ShellExecuteExW(&shellInfo)) { ExitProcess(GetCurrentProcessId()); }
		}
	}
}

WSTRING_PATH szPathTemp;

LPCWSTR
GetTempDirectory()
{
	return szPathTemp;
}

VOID
CreateTempDirectory()
{
	LPWSTR szFullPath = nullptr;

	// get program directory to create 'temp' directory
	GetApplicationDirectory(&szFullPath);
	_snwprintf_s(szPathTemp, sizeof(WSTRING_PATH), L"%s%s", szFullPath, L"\\Temp"); //-V575

	DWORD dwGetDir = GetFileAttributesW(szPathTemp);
	if (dwGetDir == INVALID_FILE_ATTRIBUTES || !(dwGetDir & FILE_ATTRIBUTE_DIRECTORY))
	{
		if (!CreateDirectoryW(szPathTemp, nullptr))
		{
			DWORD dwError = GetLastError();
			if (!IsProcessWithAdminPrivilege())
			{
				if (dwError == ERROR_ACCESS_DENIED && THROW4(L"Can't create temp directory because access denied. Re-run application with 'Administrator' privilege?"))
				{ 
					RunWithAdminPrivilege(); 
				}
			}
			else
			{
				THROW1(L"Can't create temp directory");
			}
		}
	}
}

VOID
GetTimeString(
	LPCWSTR lpString
)
{
	if (!lpString) { return; }

	static WSTRING128 szTimeString = { NULL };
	SYSTEMTIME sysTime = { NULL };
	GetSystemTime(&sysTime);
	
	_snwprintf_s(szTimeString, sizeof(WSTRING128), L"%d%d%d%d%d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute); //-V575

	memcpy((LPVOID)lpString, szTimeString, sizeof(WSTRING128));
}

LONG 
CreateMinidump(
	_EXCEPTION_POINTERS* pExceptionInfo
)
{
	WSTRING512 szFullPath = { NULL };
	WSTRING512 szTemp = { NULL };
	WSTRING_PATH szPath = { NULL };
	LPWSTR pr = nullptr;

	// get current working directory
	GetApplicationDirectory(&pr);
	memcpy(szPath, pr, 520);

	_snwprintf_s(szTemp, sizeof(WSTRING512), L"%s%s", szPath, L"\\Dump"); //-V575

	// create new path "dump"
	DWORD dwGetDir = GetFileAttributesW(szTemp);
	if (dwGetDir == INVALID_FILE_ATTRIBUTES || !(dwGetDir & FILE_ATTRIBUTE_DIRECTORY))
	{
		// we can't create temp directory at kernel paths or "Program files"
		if (!CreateDirectoryW(szTemp, nullptr))
		{
			DWORD dwError = GetLastError();
			THROW1(L"Can't create temp directory. Please, change working directory");
		}
	}

	WSTRING128 szName = { NULL };
	DWORD dwNameSize = sizeof(WSTRING128);
	GetUserNameW(szName, &dwNameSize);

	WSTRING128 szTime = { NULL };
	GetTimeString(szTime);

	// create minidump file handle
	_snwprintf_s(szFullPath, sizeof(WSTRING512), L"%s%s%s%s%s%s%s", szTemp, L"\\", L"OpenSoundRefenation_", szName, L"_", szTime, L".mdmp"); //-V575
	HANDLE hFile = CreateFileW(szFullPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_snwprintf_s(szFullPath, sizeof(WSTRING512), L"%s%s%s%s%s%s%s", szPath, L"\\", L"OpenSoundRefenation_", szName, L"_", szTime, L".mdmp"); //-V575
		hFile = CreateFileW(szFullPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	}

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo = { NULL };
		MINIDUMP_TYPE dump_flags = MINIDUMP_TYPE(MiniDumpNormal | MiniDumpFilterMemory | MiniDumpScanMemory);

		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;

		// write the dump
		if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, nullptr, nullptr))
		{
			_snwprintf_s(szTemp, sizeof(WSTRING512), L"%s%s%s", L"Minidump saved at (", szFullPath, L")"); //-V575
			MessageBoxW(nullptr, szTemp, L"Minidump saved", MB_OK | MB_ICONINFORMATION);
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

LONG 
WINAPI
UnhandledFilter(
	struct _EXCEPTION_POINTERS* pExceptionInfo
)
{
	return CreateMinidump(pExceptionInfo);
}

VOID
WINAPI
GetCurrentPeb(VOID** pPeb)
{
	___PROCESS_BASIC_INFORMATION processInformation = { nullptr };
	DWORD cbLength = NULL;

	NtQueryInformationProcessEx(GetCurrentProcess(), ProcessBasicInformation, &processInformation, sizeof(___PROCESS_BASIC_INFORMATION), &cbLength);

	*pPeb = processInformation.PebBaseAddress;
}

VOID
GetApplicationDirectory(
	LPWSTR* lpPath
)
{
	static bool IsFirst = true;
	static WCHAR Buffer[260] = { 0 };

	if (IsFirst)
	{
		GetCurrentDirectoryW(520, Buffer);
		IsFirst = false;
	}
	
	*lpPath = Buffer;
}

DWORD
GetWindowsVersion()
{
	static DWORD WinVer = 0;

	if (!WinVer)
	{
		DWORD dwVersion = 0;
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0;
		DWORD dwBuild = 0;

		typedef DWORD(WINAPI *LPFN_GETVERSION)(VOID);
		static LPFN_GETVERSION fnGetVersion = nullptr; 

		if (!fnGetVersion)
		{
			fnGetVersion = (LPFN_GETVERSION)GetProcAddress(GetModuleHandleA("kernel32"), "GetVersion");
		}

		if (fnGetVersion)
		{
			dwVersion = fnGetVersion();
			dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
			dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

			if (dwVersion < 0x80000000) { dwBuild = (DWORD)(HIWORD(dwVersion)); }

			switch (dwMajorVersion)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				break;
			case 6:
				switch (dwMinorVersion)
				{
				case 1:	 WinVer = WIN_7_SERVER2008R2;		break;
				case 2:	 WinVer = WIN_8_SERVER2012;  		break;
				case 3:	 WinVer = WIN_81_SERVER2012R2;		break;
				default: WinVer = WINDOWS_FUTURE;			break;
				}
			case 10:
				switch (dwMinorVersion)
				{
				case 0:	 WinVer = WIN_10_SERVER2016;		break;
				default: WinVer = WINDOWS_FUTURE;			break;
				}
				break;
			default:
				WinVer = WINDOWS_FUTURE;
				break;
			}
		}
	}

	return WinVer;
}
#endif