/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* main.cpp
* entry-point
*********************************************************/
#include <windows.h>
#include "OSRClasses.h"

BOOL
WINAPI
wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	OSR::UserInterface Userinterface;

	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nShowCmd);

	if (wcsstr(lpCmdLine, L"-admin")) { RunWithAdminPrivilege(); }

	// critical section
	InitApplication();
	Userinterface.CreateMainWindow();

	return TRUE;
}
