/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR network
**********************************************************
* osrNetwork.h
* Network system implementation
*********************************************************/
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <versionhelpers.h>
#include <ws2tcpip.h>

#ifndef USE_INT3
#define DEBUG_BREAK				DebugBreak()
#else
#define DEBUG_BREAK				__debugbreak()
#endif
#define DLL_API					__declspec(dllexport)

/**********************************************************
* InitNetworkModule():
* Create new network WinSock module
*
* Params:
* NULL
**********************************************************/
DLL_API 
BOOL
WINAPI
InitNetworkModule();

/**********************************************************
* FreeNetworkModule():
* Destroy current network WinSock module
*
* Params:
* NULL
**********************************************************/
DLL_API 
BOOL
WINAPI
FreeNetworkModule();

/**********************************************************
* SetNetworkConnection():
* Create new connection with remoted PC
*
* Params:
* lpPort - port to communicate
* lpOutAddress - IP address of current PC
* lpOutPort - port of current PC
**********************************************************/
DLL_API 
BOOL
WINAPI
SetNetworkConnection(
	LPCSTR lpPort, 
	LPSTR lpOutAddress,
	WORD* lpOutPort
);

/**********************************************************
* CloseNetworkConnection():
* Close current connection with remoted PC
*
* Params:
* NULL
**********************************************************/
DLL_API
BOOL
WINAPI
CloseNetworkConnection();

/**********************************************************
* SendDataToAddress():
* Send packet with data to remoted PC
*
* Params:
* lpData - packet with data
* dwDataSize - size of all packet data
* lpIpAddress - address of remoted PC
* wPort - port of remoted PC
**********************************************************/
DLL_API 
BOOL
WINAPI
SendDataToAddress(
	BYTE* lpData, 
	WORD dwDataSize, 
	LPCSTR lpIpAddress,
	WORD wPort
);

/**********************************************************
* GetDataFromSender():
* Get packet data from remoted  PC
*
* Params:
* lpData - packet with data
* dwDataSize - size of all packet data
* lpIpAddress - address of remoted PC
* wPort - port of remoted PC
**********************************************************/
DLL_API 
BOOL
WINAPI
GetDataFromSender(
	BYTE* lpData, 
	WORD* wDataSize, 
	LPCSTR lpIpAddress, 
	WORD wPort
);
