#pragma once
#include <windows.h>

class YoutubeDownloader
{
public:
	void InjectLoader(LPCWSTR PathToExecutable);
	void UnloadLoader();

	HANDLE hProcess;
	DWORD ProcessId;
};