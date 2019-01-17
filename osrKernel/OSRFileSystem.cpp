/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* OSRFileSystem.cpp
* filesystem implementation
*********************************************************/
#include "stdafx.h"

#ifdef WIN32
DLL_API HANDLE hHeap;

VOID
CreateKernelHeap()
{
	// 64kb heap (if needy more - heap get to virtual)
	hHeap = HeapCreate(NULL, 0x010000, NULL); ASSERT1(hHeap, L"Can't create heap");
}

VOID 
DestroyKernelHeap()
{
	HeapDestroy(hHeap);
}

HANDLE
GetKernelHeap()
{
	return hHeap;
}
#endif

#ifndef WITHOUT_WIDECHAR
LPCSTR 
WCSTRToMBCSTR(
	LPCWSTR lpString
) 
{
#ifdef WIN32
	// we need to get size of data to allocate
	int StringSize = WideCharToMultiByte(CP_UTF8, 0, lpString, -1, nullptr, 0, NULL, NULL);
	LPSTR lpNewString = nullptr;

	if (StringSize)
	{
		// allocate new string at kernel heap
		lpNewString = (LPSTR)FastAlloc(StringSize++);
		
		ASSERT2(WideCharToMultiByte(CP_UTF8, 0, lpString, -1, lpNewString, StringSize, NULL, NULL), L"Can't convert wchar_t to char");
	}
	return lpNewString;
#else
	// we need to get size of data to allocate
	int StringSize = wcslen(lpString);
	int OutSize = 0;
	LPSTR lpNewString = (LPSTR)FastAlloc(StringSize++);

#ifndef SECURE_BUILD
	wctomb(lpNewString, const_cast<wchar_t>(*lpString));
#else
	wctomb_s(&OutSize, lpNewString, StringSize, const_cast<wchar_t>(*lpString));
#endif
#endif
}
#endif

#ifdef WIN32
LPWSTR
FormatError(
	LONG dwErrorCode
)
{
	static WSTRING512 szError;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szError, sizeof(szError) - 1, NULL);
	return szError;
}
#endif

#ifdef WIN32
OSRCODE
OpenFileDialog(
	WSTRING_PATH* lpPath
)
{
	// set params to struct
	OPENFILENAMEW openFile = { NULL };
	openFile.lStructSize = sizeof(OPENFILENAMEW);
	openFile.nMaxFile = MAX_PATH;
	openFile.lpstrFile = *lpPath;
	openFile.lpstrFilter = L"All Files\0*.*\0\0"; //L"Audio files (.wav)\0*.wav\0";
	openFile.lpstrTitle = L"Open audio file";
	openFile.nFilterIndex = 1;
	openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// open file dialog
	if (!GetOpenFileNameW(&openFile)) { return FS_OSR_BAD_PATH; }
	return OSR_SUCCESS;
}
#else
OSRCODE
OpenFileDialog(
	STRING_PATH* lpPath
)
{
	printf("Unfortunately, the open file dialog at POSIX system is N/A. Please, write here path to your file: \n");
	scanf("%s", (char*)*lpPath);
}
#endif

#ifdef WIN32
BOOL
GetDiskUsage(
	LARGE_INTEGER largeSize,
	LPCWSTR lpPath
)
{
	// get current disk
	WCHAR szCurrentDisk[4] = { lpPath[0], L':', L'\\', L'\0' };
	ULARGE_INTEGER freeBytesToCaller = { NULL };

	// get free disk space 
	GetDiskFreeSpaceExW(szCurrentDisk, &freeBytesToCaller, nullptr, nullptr);

	if ((UINT64)largeSize.QuadPart > freeBytesToCaller.QuadPart)
	{
		if (!THROW4(L"This file is bigger then free space on your drive. Continue?")) { return FALSE; }
	}

	return TRUE;
}
#else
BOOL
GetDiskUsage(
	size_t FileSize,
	const char* lpPath
)
{
	statvfs fileInfo = { NULL };

	statvfs(lpPath, &fileInfo);
	unsigned long freeSpaceSize = fileInfo.f_bsize * fileInfo.f_bfree;

	if (FileSize > freeSpaceSize)
	{
		if (!THROW4("This file is bigger then free space on your drive. Continue?")) { return FALSE; }
	}

	return TRUE;
}
#endif

#ifdef WIN32
OSRCODE 
CreateNewFileWithData(
	LPCWSTR lpPath, 
	LPVOID lpData, 
	DWORD dwDataSize
)
{
	DWORD DataOutSize = 0;
	HANDLE hFile = CreateFileW(lpPath, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile || hFile == INVALID_HANDLE_VALUE) { return FS_OSR_BAD_HANDLE; }

	WriteFile(hFile, lpData, dwDataSize, &DataOutSize, nullptr);
	CloseHandle(hFile);

	return OSR_SUCCESS;
}

OSRCODE 
ReadDataFromFile(
	LPCWSTR lpPath,
	VOID** lpOutData,
	LPDWORD lpOutSize
)
{
	LARGE_INTEGER largeCount = { 0 };
	size_t uSize = 0;
	HANDLE hFile = NULL;

	// open handle and read audio file to buffer 
	hFile = CreateFileW(lpPath, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (!hFile || hFile == INVALID_HANDLE_VALUE) { return FS_OSR_BAD_HANDLE; }

	GetFileSizeEx(hFile, &largeCount);
	uSize = largeCount.QuadPart;

	// allocate pointer and get data to it
	*lpOutData = FastAlloc(uSize);

	if (!ReadFile(hFile, *lpOutData, uSize, lpOutSize, nullptr))
	{
		FREEKERNELHEAP(*lpOutData);
		CloseHandle(hFile);
		return FS_OSR_BAD_ALLOC;
	}

	CloseHandle(hFile);
	return OSR_SUCCESS;
}

OSRCODE
ReadAudioFile(
	LPCWSTR lpPath,
	VOID** lpData,
	DWORD* dwSizeWritten
)
{	
	LARGE_INTEGER largeCount = { 0 };
	size_t uSize = 0;
	HANDLE hFile = NULL;

	// open handle and read audio file to buffer 
	hFile = CreateFileW(lpPath, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (!hFile || hFile == INVALID_HANDLE_VALUE) { return FS_OSR_BAD_HANDLE; }

	FILE_STANDARD_INFO fileInfo = {};
	GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo));

	GetFileSizeEx(hFile, &largeCount);
	uSize = largeCount.QuadPart;

	// allocate pointer and get data to it
	*lpData = FastAlloc(uSize);
	ASSERT1(*lpData, L"Can't alloc pointer");

	if (!ReadFile(hFile, *lpData, uSize, dwSizeWritten, nullptr)) 
	{ 
		FREEKERNELHEAP(*lpData)
		CloseHandle(hFile);
		return FS_OSR_BAD_ALLOC; 
	}

	CloseHandle(hFile);
	return OSR_SUCCESS;
}

#else
OSRCODE
ReadAudioFile(
	const char* lpPath,
	VOID** lpData,
	unsigned long long* dwSizeWritten
)
{
	// read-only mode
	FILE* pFile = fopen(lpPath, "r");
	if (!pFile) { return FS_OSR_BAD_HANDLE; }
	*dwSizeWritten = filelength(fileno(pFile));

	*lpData = FastAlloc(*dwSizeWritten);
	if (!*lpData) { return FS_OSR_BAD_ALLOC; }
	ASSERT2(!(fread(*lpData, dwSizeWritten, dwSizeWritten, pFile) == *dwSizeWritten), "Can't read file from disk");

	fclose(pFile);
	return OSR_SUCCESS;
}
#endif

#ifdef WIN32
OSRCODE
ReadAudioFileEx(
	LPCWSTR lpPath,
	VOID** lpData,
	LONGLONG* uSize,
	LPDWORD dwHeaderSize
)
{
	// open handle and read audio file to buffer 
	HANDLE hFile = CreateFileW(lpPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile || hFile == INVALID_HANDLE_VALUE) { return FS_OSR_BAD_HANDLE; }

	LARGE_INTEGER largeSize = { NULL };
	size_t sizeWritten = NULL;

	GetFileSizeEx(hFile, &largeSize);
	*uSize = largeSize.QuadPart;

	// if file smaller then 4GB
	if (largeSize.QuadPart < DWORD(-1))
	{
		// allocate pointer and get data to it
		*lpData = AdvanceAlloc((UINT64)largeSize.QuadPart, HEAP_MEMORY_ALLOC);
		ASSERT1(*lpData, L"Can't alloc pointer");

		if (!ReadFile(hFile, *lpData, (DWORD)((UINT64)largeSize.QuadPart), (DWORD*)&sizeWritten, NULL)) 
		{ 
			return FS_OSR_BAD_ALLOC;
		}
	}
	else
	{
		if (!GetDiskUsage(largeSize, lpPath)) { return FS_OSR_NO_SPACE; }
		DWORD dwTempWritten = NULL;
		LARGE_INTEGER largeInt = {};
		largeInt.QuadPart = DWORD(-1);
		DWORD dwSizeFinal = (DWORD)(*uSize - DWORD(-2));

		*lpData = AdvanceAlloc((UINT64)largeSize.QuadPart, VIRTUAL_MEMORY_ALLOC);
		ASSERT1(*lpData, L"Can't alloc pointer");

		if (!ReadFile(hFile, *lpData, DWORD(-2), &dwTempWritten, NULL)) { return FS_OSR_BAD_PTR; }
		if (!SetFilePointerEx(hFile, largeInt, NULL, FILE_BEGIN)) { return FS_OSR_BAD_PTR; }
		if (!ReadFile(hFile, *lpData, dwSizeFinal, &dwTempWritten, NULL)) { return FS_OSR_BAD_PTR; }
	}

	DWORD dwHeaderId = (*(LPDWORD)*lpData);
	if (dwHeaderId != 0x46464952 && dwHeaderId != 0x45564157 && dwHeaderId != 0x20746d66)
	{
		THROW4(L"Can't open file, because this is not sound file.");
		FreePointer(*lpData, (UINT64)largeSize.QuadPart, VIRTUAL_MEMORY_ALLOC);
		return FS_OSR_BAD_PTR;
	}

	WAV_CHUNK_HEADER waveTemp = { NULL };
	DWORD dwPointerToData = 0;

	for (DWORD i = 0; i < 8; i++)
	{
		WAV_CHUNK_HEADER waveHeader = *((WAV_CHUNK_HEADER*)(*lpData) + dwPointerToData);
		
		// find 'data' chunk
		if (waveHeader.dwChunkId == 0x61746164) { break; }
		dwPointerToData += waveHeader.dwFileSize;
	}

	*dwHeaderSize = dwPointerToData;

	CloseHandle(hFile);
	return OSR_SUCCESS;
}
#endif

//RIFFChunk* FindSoundChunk(
//	_In_reads_bytes_(sizeBytes) BYTE* data,
//	_In_ size_t sizeBytes,
//	_In_ UINT tag
//)
//{
//	if (!data) { return NULL; }
//
//	BYTE* ptr = data;
//	BYTE* end = data + sizeBytes;
//
//	while (end > (ptr + sizeof(RIFFChunk)))
//	{
//		RIFFChunk* header = reinterpret_cast<RIFFChunk*>(ptr);
//		if (header->tag == tag) { return header; }
//
//		ptrdiff_t offset = header->size + sizeof(RIFFChunk);
//		ptr += offset;
//	}
//
//	return nullptr;
//}

OSRCODE
GetWaveFormatExtented(
	BYTE* lpWaveFile,
	DWORD dwFileSize,
	WAVEFORMATEX* waveFormat
)
{
	//// check RIFF tag
	//RIFFChunk* riffChunk = FindSoundChunk(lpWaveFile, dwFileSize, FOURCC_RIFF_TAG);

	//// if chunk is empty or size smaller than 4 - take message
	//if (!riffChunk || riffChunk->size < 4) { return FS_OSR_BAD_PTR; }

	//// get RIFF chunk header info
	//BYTE* wavEnd = lpWaveFile + dwFileSize;
	//RIFFChunkHeader* riffHeader = reinterpret_cast<RIFFChunkHeader*>(riffChunk);

	//// if this file isn't RIFF - take message
	//if (riffHeader->riff != FOURCC_WAVE_FILE_TAG && riffHeader->riff != FOURCC_XWMA_FILE_TAG) { return FS_OSR_BAD_PTR; }

	//// locate 'fmt ' at file
	//BYTE* ptr = reinterpret_cast<BYTE*>(riffHeader) + sizeof(RIFFChunkHeader);

	//if ((ptr + sizeof(RIFFChunk)) > wavEnd) { return FS_OSR_BAD_PTR; }

	//// find fmt chunk
	//RIFFChunk* fmtChunk = FindSoundChunk(ptr, riffHeader->size, FOURCC_FORMAT_TAG);

	//// if chunk is empty or size smaller than size of PCMWAVEFORMAT - take message
	//if (!fmtChunk || fmtChunk->size < sizeof(PCMWAVEFORMAT)) { return FS_OSR_BAD_PTR; }

	//// reinterpret fmt chunk to pointer
	//ptr = reinterpret_cast<BYTE*>(fmtChunk) + sizeof(RIFFChunk);
	//if (ptr + fmtChunk->size > wavEnd) { return FS_OSR_BAD_PTR; }

	//WAVEFORMAT* wf = reinterpret_cast<WAVEFORMAT*>(ptr);

	//// check formatTag
	//switch (wf->wFormatTag)
	//{
	//case WAVE_FORMAT_PCM:
	//case 0x0002:
	//	break;
	//default:
	//{
	//	if (fmtChunk->size < sizeof(WAVEFORMATEX)) { return FS_OSR_BAD_PTR; }
	//	WAVEFORMATEX* wfx = reinterpret_cast<WAVEFORMATEX*>(ptr);

	//	if (fmtChunk->size < (sizeof(WAVEFORMATEX) + wfx->cbSize)) { return FS_OSR_BAD_PTR; }
	//}
	//}

	//// reinterpretate RIFF header to pointer
	//ptr = reinterpret_cast<BYTE*>(riffHeader) + sizeof(RIFFChunkHeader);
	//if ((ptr + sizeof(RIFFChunk)) > wavEnd) { return FS_OSR_BAD_PTR; }

	//// find 'data' chunk
	//RIFFChunk* dataChunk = FindSoundChunk(ptr, riffChunk->size, FOURCC_DATA_TAG);
	//if (!dataChunk || !dataChunk->size) { return FS_OSR_BAD_PTR; }

	//// reinterpretate 'data' header to pointer
	//ptr = reinterpret_cast<BYTE*>(dataChunk) + sizeof(RIFFChunk);
	//if (ptr + dataChunk->size > wavEnd) { return FS_OSR_BAD_PTR; }

	//*waveFormat = *reinterpret_cast<WAVEFORMATEX*>(wf);
	//waveFormat->cbSize = sizeof(WAVEFORMATEX);

	return OSR_SUCCESS;
}

#ifdef WIN32
OSRCODE
WriteFileFromBuffer(
	LPCWSTR lpPath,
	BYTE* pFile,
	DWORD dwSize,
	WAVEFORMATEX* waveFormat
)
{
	HANDLE hFile = CreateFileW(lpPath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile || hFile == (HANDLE)LONG_PTR(-1)) 
	{ 
		if (!THROW4(L"Application can't save this file because file handle is invalid. Continue?")) { return FS_OSR_BAD_HANDLE; }
	}

	LARGE_INTEGER largeSize = { NULL };
	largeSize.QuadPart = dwSize;

	if (!GetDiskUsage(largeSize, lpPath))
	{
		CloseHandle(hFile);
		return FS_OSR_NO_SPACE; 
	}

	DWORD dwFileWritten = NULL;
	DWORD dwHeaderSize = NULL;

	// set header
	DWORD dwHead[] = { FOURCC_RIFF_TAG, NULL, FOURCC_WAVE_FILE_TAG, FOURCC_FORMAT_TAG, sizeof(WAVEFORMATEX) };
	DWORD dwHeadData[] = { FOURCC_DATA_TAG, NULL };

	// write first data
	if (!WriteFile(hFile, dwHead, sizeof(dwHead), &dwFileWritten, NULL)) { return FS_OSR_BAD_PTR; }
	if (!WriteFile(hFile, waveFormat, sizeof(WAVEFORMATEX), &dwFileWritten, NULL)) { return FS_OSR_BAD_PTR; }
	if (!WriteFile(hFile, dwHeadData, sizeof(dwHeadData), &dwFileWritten, NULL)) { return FS_OSR_BAD_PTR; }

	// get size of header 
	dwHeaderSize = sizeof(dwHead) + sizeof(WAVEFORMATEX) + sizeof(dwHeadData);

	// write full audio data
	if (!WriteFile(hFile, pFile, dwSize, &dwFileWritten, NULL)) { return FS_OSR_BAD_PTR; }

	// get size of header
	LARGE_INTEGER largeInt = { NULL };
	largeInt.QuadPart = dwHeaderSize - sizeof(DWORD);

	// set pointer pos to write file size
	if (!SetFilePointerEx(hFile, largeInt, NULL, FILE_BEGIN)) { return FS_OSR_BAD_PTR; }
	if (!WriteFile(hFile, &dwSize, sizeof(DWORD), &dwFileWritten, NULL)) { return FS_OSR_BAD_PTR; }

	DWORD dwRIFFFileSize = dwHeaderSize + dwSize - 8;
	largeInt.QuadPart = sizeof(FOURCC);

	// finally write a file
	if (!SetFilePointerEx(hFile, largeInt, NULL, FILE_BEGIN)) { return FS_OSR_BAD_PTR; }
	if (!WriteFile(hFile, &dwRIFFFileSize, sizeof(dwRIFFFileSize), &dwFileWritten, NULL)) { return FS_OSR_BAD_PTR; }

	// close file handle
	CloseHandle(hFile);
	
	return OSR_SUCCESS;
}
#endif
