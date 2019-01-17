/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* OSRSystem.h
* Sample system implementation
*********************************************************/
#pragma once
#include "OSRTypes.h"
#include "OSR.h"

#ifdef WIN32
#include <intrin.h> 
#endif

typedef struct
{
	u32 StructSize;					// size of struct
	u32 SampleSize;					// size of current sample
	u32 SampleNumber;				// if 0 - random sample, else - numeric sample of audio file
	u64 SampleDuration;				// duration of sample (in msecs)
	WAVEFORMATEX waveFormat;		// sample format info
	u8* pSample;					// pointer to data of sample
} LOOP_INFO, *LPLOOP_INFO, *PLOOP_INFO;

typedef struct
{
	u32 SampleCount;				// count of all samples in 1 file
	u32 FileSize;					// file size (includes header information)
	u64 FileDuration;				// final file duration (include all samples duration)
	LPLOOP_INFO pSampleInfo;		// to count size of this struct - sizeof(LOOP_INFO) * dwSampleCount
} AUDIO_INFO, *LPAUDIO_INFO, *PAUDIO_INFO;

class OSRSample
{
public:
	OSRSample()
	{
		IsFloatInput = 0;
		IsFloatOutput = 0;
		BitsInput = 0;
		BitsOutput = 0;
		ChannelsInput = 0;
		ChannelsOutput = 0;
		BufferSizeInput = 0;
		BufferSizeOutput = 0;
		SampleRateInput = 0;
		SampleRateOutput = 0;
		SamplePosition = 0;
		ToEndFileSize = 0;

		for (u8 i = 0; i < 16; i++)
		{
			pInputBuffer[i] = nullptr;
			pOutputBuffer[i] = nullptr;
		}

		pNextSample = nullptr;
		pPreviousSample = nullptr;
	}

	OSRSample(u8 Bits, u8 Channels, u32 BufferSize, u32 SampleRate)
	{
		IsFloatInput = (!!(Bits >= 32));
		IsFloatOutput = (!!(Bits >= 32));

		BitsInput = Bits;
		BitsOutput = Bits;
		ChannelsInput = Channels;
		ChannelsOutput = Channels;
		BufferSizeInput = BufferSize;
		BufferSizeOutput = BufferSize;
		SampleRateInput = SampleRate;
		SampleRateOutput = SampleRate;
		ToEndFileSize = NULL;
		SamplePosition = NULL;

		for (u8 i = 0; i < 16; i++)
		{
			pInputBuffer[i] = nullptr;
			pOutputBuffer[i] = nullptr;
		}

		pNextSample = nullptr;
		pPreviousSample = nullptr;
	}

	void LoadSample(void* pData, u32 BufferSize, u8 Bits, u8 Channels, u32 SampleRate)
	{
		u8* p8 = nullptr;
		i16* p16 = nullptr;
		i24* p24 = nullptr;
		f32* pf32 = nullptr;

		for (u32 i = 0; i < Channels; i++)
		{
			if (!pOutputBuffer[i]) { pOutputBuffer[i] = (f32*)AdvanceAlloc(BufferSize * sizeof(f32), NULL); }
		}

		switch (Bits)
		{
		case 8:
			p8 = (u8*)pData;

			for (u32 i = 0; i < BufferSize; i++)
			{
				pOutputBuffer[i % Channels][i / Channels] = i16tof32((i16)p8[i]);
			}
			break;
		case 16:
			p16 = (i16*)pData;

			for (u32 i = 0; i < BufferSize; i++)
			{
				pOutputBuffer[i % Channels][i / Channels] = i16tof32(p16[i]);
			}
			break;
		case 24:
			p24 = (i24*)pData;

			for (u32 i = 0; i < BufferSize; i++)
			{
				pOutputBuffer[i % Channels][i / Channels] = i24tof32(p24[i]);
			}
			break;
		case 32:
			pf32 = (f32*)pData;

			for (u32 i = 0; i < BufferSize; i++)
			{
				pOutputBuffer[i % Channels][i / Channels] = pf32[i];
			}
			break;
		}

		IsFloatInput = (!!(Bits >= 32));
		IsFloatOutput = (!!(Bits >= 32));

		BitsInput = Bits;
		BitsOutput = Bits;
		ChannelsInput = Channels;
		ChannelsOutput = Channels;
		BufferSizeInput = BufferSize;
		BufferSizeOutput = BufferSize;
		SampleRateInput = SampleRate;
		SampleRateOutput = SampleRate;
	}

	OSRSample* OnBufferEnd(LPLOOP_INFO pLoop)
	{
		void* pData = nullptr;
		bool isEnd = false;

		if (!BitsOutput)
		{
			BitsOutput = (BYTE)pLoop->waveFormat.wBitsPerSample;
			ChannelsOutput = (BYTE)pLoop->waveFormat.nChannels;
			SampleRateOutput = pLoop->waveFormat.nSamplesPerSec;
			BufferSizeOutput = 16384;
		}

		if (!pNextSample) 
		{ 
			pNextSample = new OSRSample(BitsOutput, ChannelsOutput, BufferSizeOutput, SampleRateOutput); 
			pNextSample->pPreviousSample = this;

			if (!ToEndFileSize && !SamplePosition)  { ToEndFileSize = pLoop->SampleSize; }
			pNextSample->SamplePosition = SamplePosition + (BufferSizeOutput * (BitsOutput / 8));
			pNextSample->ToEndFileSize = ToEndFileSize - (BufferSizeOutput * (BitsOutput / 8));
		}

		pData = (void*)ptrdiff_t(pLoop->pSample + pNextSample->SamplePosition);

		if (pNextSample->ToEndFileSize <= 0) 
		{
			isEnd = true;
		}

		if (pNextSample->ToEndFileSize > (BufferSizeOutput * (BitsOutput / 8)) && !isEnd)
		{
			pNextSample->LoadSample(pData, BufferSizeOutput, BitsOutput, ChannelsOutput, SampleRateOutput);
		}
		else
		{	
			u8 bufFloat[44100 * 2 * 4] = { NULL };

			if (pNextSample->ToEndFileSize > 0) { memcpy(bufFloat, pData, pNextSample->ToEndFileSize); }
			pNextSample->LoadSample(bufFloat, BufferSizeOutput, BitsOutput, ChannelsOutput, SampleRateOutput);
		}

		return pNextSample;
	}

	void ConvertToPlay(void* pOutData)
	{
		f32* pOutBuf = (f32*)pOutData;

		if (pOutData)
		{
			for (u32 i = 0; i < BufferSizeOutput; i++)
			{
				pOutBuf[i] = pOutputBuffer[i % ChannelsOutput][i / ChannelsOutput];
			}
		}
	}

	void ConvertToPlay(void* pOutData, u8 Bits)
	{
		u8* p8 = nullptr;
		i16* p16 = nullptr;
		i24* p24 = nullptr;
		f32* pf32 = nullptr;

		switch (Bits)
		{
		case 8:
			p8 = (u8*)pOutData;

			for (u32 i = 0; i < BufferSizeOutput; i++)
			{
				p8[i] = (u8)f32toi16(pOutputBuffer[i % ChannelsOutput][i / ChannelsOutput]);
			}
			break;
		case 16:
			p16 = (i16*)pOutData;

			for (u32 i = 0; i < BufferSizeOutput; i++)
			{
				p16[i] = f32toi16(pOutputBuffer[i % ChannelsOutput][i / ChannelsOutput]);
			}
			break;
		case 24:
			p24 = (i24*)pOutData;

			for (u32 i = 0; i < BufferSizeOutput; i++)
			{
				p24[i] = f32toi24(pOutputBuffer[i % ChannelsOutput][i / ChannelsOutput]);
			}
			break;
		case 32:
			pf32 = (f32*)pOutData;

			for (u32 i = 0; i < BufferSizeOutput; i++)
			{
				pf32[i] = pOutputBuffer[i % ChannelsOutput][i / ChannelsOutput];
			}
			break;
		}
	}

	void FreeSample()
	{
		for (u32 i = 0; i < 8; i++)
		{
			FREEPROCESSHEAP(pOutputBuffer[i]);
			FREEPROCESSHEAP(pInputBuffer[i]);
		}
	}

	~OSRSample()
	{
		FreeSample();
	}

	u8 BitsInput;
	u8 BitsOutput;
	u8 IsFloatInput;
	u8 IsFloatOutput;
	u8 ChannelsInput;
	u8 ChannelsOutput;
	u32 BufferSizeInput;
	u32 BufferSizeOutput;
	u32 SampleRateInput;
	u32 SampleRateOutput;
	u64 SamplePosition; 
	i32 ToEndFileSize;

	f32* pInputBuffer[16];
	f32* pOutputBuffer[16];

	OSRSample* pNextSample;
	OSRSample* pPreviousSample;
};

class OSRSampleEx : public OSRSample
{
	u32 CurrentSystem;
	u8 IsBigEndian;
};

class IOSRFileSystem : public IObject
{
public:
	virtual void Open(const char* PathToFile, u8*& pOutStream, size_t& OutSize) = 0;
	virtual void SetPosition(OSRHandle& Handle, size_t FilePosition) = 0;
	virtual size_t GetCurrentSize(OSRHandle& Handle) = 0;

	virtual void Read(OSRHandle& Handle, u8*& pOutStream, size_t& OutSize) = 0;
	virtual void ReadSize(OSRHandle& Handle, size_t& SizeToRead, void*& pOutFile) = 0;
	virtual void Write(OSRHandle& Handle, u8*& pOutStream, size_t& OutSize) = 0;

	virtual void GetPathUTF8(OSRHandle& OutHandle, const char*& OutString, size_t& OutStringSize) = 0;
	virtual bool IsAudio(const char* PathToFile) = 0;

	virtual void OpenHandle(const char* PathToFile, OSRHandle& OutHandle) = 0;
	virtual void CloseThisHandle(OSRHandle& OutHandle) = 0;

	OSRHandle LocalHandle;
};

#ifdef WIN32
#include <psapi.h>
#include <strsafe.h>
#include <strsafe.h>

class IOSRWin32FileSystem : public IOSRFileSystem
{
public:
	IOSRWin32FileSystem() {};
	IOSRWin32FileSystem(OSRHandle LocalHandle1)
	{
		LocalHandle = LocalHandle1;
	}

	IOSRWin32FileSystem(LPCWSTR PathToFile)
	{
		OpenFileHandleW(PathToFile, LocalHandle);
	}

	IOSRWin32FileSystem(LPCSTR PathToFile)
	{
		OpenHandle(PathToFile, LocalHandle);
	}

	~IOSRWin32FileSystem()
	{
		CloseThisHandle(LocalHandle);
	}

	// allocating by AdvanceAlloc (can be VirtualAlloc or HeapAlloc)
	void Open(const char* PathToFile, u8*& pOutStream, size_t& OutSize) override
	{
		OpenFileToByteStreamA(PathToFile, pOutStream, OutSize);
	}

	void OpenFileToByteStreamA(const char* PathToFile, u8*& pOutStream, size_t& OutSize)
	{
		if (!PathToFile) { return; }

		WSTRING_PATH szPath = { 0 };
		DWORD OutSizeword = 0;
		LARGE_INTEGER largeCount = { 0 };
		size_t uSize = 0;

		if (MultiByteToWideChar(CP_UTF8, 0, PathToFile, strlen(PathToFile), szPath, sizeof(WSTRING_PATH)))
		{
			// open handle and read audio file to buffer
			LocalHandle = CreateFileW(szPath, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (!LocalHandle && LocalHandle == INVALID_HANDLE_VALUE) { return; }

			GetFileSizeEx(LocalHandle, &largeCount);
			uSize = largeCount.QuadPart;

			// allocate pointer and get data to it
			pOutStream = (u8*)FastAlloc(uSize);

			if (!ReadFile(LocalHandle, pOutStream, uSize, &OutSizeword, nullptr))
			{
				FREEKERNELHEAP(pOutStream);
				CloseHandle(LocalHandle);
				return;
			}

			CloseHandle(LocalHandle);
		}
	}

	void OpenFileToByteStreamW(const wchar_t* PathToFile, u8*& pOutStream, size_t& OutSize)
	{
		if (!PathToFile) { return; }

		DWORD OutSizeword = 0;
		LARGE_INTEGER largeCount = { 0 };
		size_t uSize = 0;

		// open handle and read audio file to buffer
		LocalHandle = CreateFileW(PathToFile, GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (!LocalHandle && LocalHandle == INVALID_HANDLE_VALUE) { return; }

		GetFileSizeEx(LocalHandle, &largeCount);
		uSize = largeCount.QuadPart;

		// allocate pointer and get data to it
		pOutStream = (u8*)FastAlloc(uSize);

		if (!ReadFile(LocalHandle, pOutStream, uSize, &OutSizeword, nullptr)) { FREEKERNELHEAP(pOutStream); }

		CloseHandle(LocalHandle);
		LocalHandle = nullptr;
	}

	void OpenHandle(const char* PathToFile, OSRHandle& OutHandle) override
	{
		OpenFileHandleA(PathToFile, OutHandle);
	}

	void OpenFileHandleA(const char* PathToFile, OSRHandle& OutHandle)
	{
		WSTRING_PATH szPath = { 0 };

		if (MultiByteToWideChar(CP_UTF8, 0, PathToFile, strlen(PathToFile), szPath, sizeof(WSTRING_PATH)))
		{
			OutHandle = CreateFileW(szPath, GENERIC_READ | GENERIC_WRITE, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (OutHandle == INVALID_HANDLE_VALUE) { OutHandle = nullptr; return; }
			LocalHandle = OutHandle;
		}
	}

	void OpenFileHandleW(const wchar_t* PathToFile, OSRHandle& OutHandle)
	{
		OutHandle = CreateFileW(PathToFile, GENERIC_READ | GENERIC_WRITE, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (OutHandle == INVALID_HANDLE_VALUE) { OutHandle = nullptr; return;}
		LocalHandle = OutHandle;
	}

	void SetPosition(OSRHandle& Handle, size_t FilePosition) override
	{
		LARGE_INTEGER largeInt = { NULL };
		GetFileSizeEx(Handle, &largeInt);
		
		if (largeInt.QuadPart >= FilePosition)
		{
			largeInt.QuadPart = FilePosition;
			SetFilePointerEx(Handle, largeInt, nullptr, FILE_BEGIN);
		}
	}

	void Read(OSRHandle& Handle, u8*& pOutStream, size_t& OutSize) override
	{
		if (!Handle) { return; };
		DWORD OutSizeword = 0;
		LARGE_INTEGER largeCount = { 0 };

		GetFileSizeEx(Handle, &largeCount);
		OutSize = largeCount.QuadPart;

		// allocate pointer and get data to it
		pOutStream = (u8*)FastAlloc(OutSize);

		if (!ReadFile(Handle, pOutStream, OutSize, &OutSizeword, nullptr))
		{
			FREEKERNELHEAP(pOutStream);
			CloseHandle(Handle);
			Handle = nullptr;
			return;
		}
	}

	void ReadSize(OSRHandle& Handle, size_t& SizeToRead, void*& pOutFile) override
	{
		if (!Handle) { return; };
		DWORD OutSizeword = 0;
		SIZE_T OutSize = 0;
		LARGE_INTEGER largeCount = { 0 };

		GetFileSizeEx(Handle, &largeCount);
		OutSize = largeCount.QuadPart;
		
		if (OutSize > SizeToRead)
		{
			// allocate pointer and get data to it
			pOutFile = (u8*)FastAlloc(SizeToRead);

			if (!ReadFile(Handle, pOutFile, SizeToRead, &OutSizeword, nullptr))
			{
				FREEKERNELHEAP(pOutFile);
				CloseHandle(Handle);
				Handle = nullptr;
				return;
			}
		}
		else
		{
			// allocate pointer and get data to it
			pOutFile = (u8*)FastAlloc(OutSize);

			if (!ReadFile(Handle, pOutFile, OutSize, &OutSizeword, nullptr))
			{
				FREEKERNELHEAP(pOutFile);
				CloseHandle(Handle);
				Handle = nullptr;
				return;
			}

			SizeToRead = OutSizeword;
		}
	}

	size_t GetCurrentSize(OSRHandle& Handle) override
	{
		LARGE_INTEGER Larges = { 0 };
		GetFileSizeEx(Handle, &Larges);

		return Larges.QuadPart;
	}

	void Write(OSRHandle& Handle, u8*& pOutStream, size_t& OutSize) override
	{
		if (!Handle) { return; };

		DWORD OutSizeword = 0;

		if (!WriteFile(Handle, pOutStream, OutSize, &OutSizeword, nullptr))
		{
			CloseHandle(Handle);
			Handle = nullptr;
			return;
		}
	}

	void GetPathUTF8(OSRHandle& OutHandle, const char*& OutString, size_t& OutStringSize) override
	{
		BOOL bSuccess = FALSE;
		WSTRING_PATH FileNameWideChar = { 0 };
		HANDLE hFileMap;
		DWORD dwFileSizeHi = 0;
		DWORD dwFileSizeLo = 0;
		LARGE_INTEGER FileSize = { 0 };

		GetFileSizeEx(OutHandle, &FileSize);
		dwFileSizeLo = FileSize.QuadPart;

		hFileMap = CreateFileMapping(OutHandle, nullptr, PAGE_READONLY, 0, 1, nullptr);
		if (hFileMap)
		{
			// create a file mapping to get the file name.
			void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

			if (pMem)
			{
				if (GetMappedFileNameW(GetCurrentProcess(), pMem, FileNameWideChar, MAX_PATH))
				{
					// translate path with device name to drive letters.
					WCHAR szTemp[520];
					szTemp[0] = '\0';

					if (GetLogicalDriveStringsW(520 - 1, szTemp))
					{
						WSTRING_PATH szName;
						WCHAR szDrive[3] = L" :";
						BOOL bFound = FALSE;
						WCHAR* p = szTemp;

						do
						{
							// copy the drive letter to the template string
							*szDrive = *p;

							// look up each device name
							if (QueryDosDeviceW(szDrive, szName, MAX_PATH))
							{
								size_t uNameLen = wcslen(szName);

								if (uNameLen < MAX_PATH)
								{
									bFound = _wcsnicmp(FileNameWideChar, szName, uNameLen) == 0 && *(FileNameWideChar + uNameLen) == L'\\';

									if (bFound)
									{
										// reconstruct pszFilename using szTempFile
										// replace device path with DOS path
										WSTRING_PATH szTempFile;
										StringCchPrintfW(szTempFile,
											MAX_PATH,
											L"%s%s",
											szDrive,
											FileNameWideChar + uNameLen);
										StringCchCopyNW(FileNameWideChar, MAX_PATH + 1, szTempFile, wcslen(szTempFile));
									}
								}
							}

							// go to the next NULL character.
							while (*p++);
						} while (!bFound && *p); // end of string
					}
				}
				bSuccess = TRUE;
				UnmapViewOfFile(pMem);
			}

			CloseHandle(hFileMap);
		}

		if (bSuccess)
		{
			// we need to get size of data to allocate
			OutStringSize = WideCharToMultiByte(CP_UTF8, 0, FileNameWideChar, -1, nullptr, 0, nullptr, nullptr);

			if (OutStringSize)
			{
				// allocate new string at kernel heap
				OutString = (LPSTR)FastAlloc(++OutStringSize);

				ASSERT2(WideCharToMultiByte(CP_UTF8, 0, FileNameWideChar, -1, const_cast<LPSTR>(OutString), OutStringSize, nullptr, nullptr), L"Can't convert wchar_t to char");
			}
		}
	}

	void GetPathUTF16(OSRHandle& OutHandle, WSTRING_PATH& OutString)
	{
		BOOL bSuccess = FALSE;
		HANDLE hFileMap;
		DWORD dwFileSizeHi = 0;
		DWORD dwFileSizeLo = 0;
		LARGE_INTEGER FileSize = { 0 };

		GetFileSizeEx(OutHandle, &FileSize);
		dwFileSizeLo = FileSize.QuadPart;

		hFileMap = CreateFileMappingW(OutHandle, nullptr, PAGE_READONLY, 0, 1, nullptr);
		if (hFileMap)
		{
			// create a file mapping to get the file name.
			void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

			if (pMem)
			{
				if (GetMappedFileNameW(GetCurrentProcess(), pMem, OutString, MAX_PATH))
				{
					// translate path with device name to drive letters.
					WCHAR szTemp[520];
					szTemp[0] = '\0';

					if (GetLogicalDriveStringsW(520 - 1, szTemp))
					{
						WSTRING_PATH szName;
						WCHAR szDrive[3] = L" :";
						BOOL bFound = FALSE;
						WCHAR* p = szTemp;

						do
						{
							// copy the drive letter to the template string
							*szDrive = *p;

							// look up each device name
							if (QueryDosDeviceW(szDrive, szName, MAX_PATH))
							{
								size_t uNameLen = wcslen(szName);

								if (uNameLen < MAX_PATH)
								{
									bFound = _wcsnicmp(OutString, szName, uNameLen) == 0 && *(OutString + uNameLen) == L'\\';

									if (bFound)
									{
										// reconstruct pszFilename using szTempFile
										// replace device path with DOS path
										WSTRING_PATH szTempFile;
										StringCchPrintfW(szTempFile,
											MAX_PATH,
											L"%s%s",
											szDrive,
											OutString + uNameLen);
										StringCchCopyNW(OutString, MAX_PATH + 1, szTempFile, wcslen(szTempFile));
									}
								}
							}

							// go to the next NULL character.
							while (*p++);
						} while (!bFound && *p); // end of string
					}
				}
				bSuccess = TRUE;
				UnmapViewOfFile(pMem);
			}

			CloseHandle(hFileMap);
		}
	}

	bool IsAudio(const char* PathToFile) override
	{
		return IsFileIsAudioA(PathToFile);
	}

	bool IsFileIsAudioA(const char* PathToFile)
	{
		int StringSize = 0;
		int TypeSize = 0;
		WSTRING_PATH szPath = { 0 };
		LPCWSTR szTypes[] = { 
		L"mp3", L"MP3", L"wav", L"WAV", L"flac", L"FLAC", L"aiff", L"AIFF", L"ogg", L"OGG", L"opus", L"OPUS", L"alac", L"ALAC", L"aac", L"AAC", L"m4a", L"M4A" };

		if ((StringSize = MultiByteToWideChar(CP_UTF8, 0, PathToFile, strlen(PathToFile), szPath, sizeof(WSTRING_PATH))))
		{
			for (size_t i = wcslen(szPath); i > 0; i--)
			{
				WCHAR cbSymbol = szPath[i];
			
				if (cbSymbol == L'.') { break; }

				TypeSize++;
			}
		
			if (TypeSize)
			{
				for (LPCWSTR CurrentType : szTypes)
				{
					if (!wcscmp(CurrentType, (&szPath[0] + StringSize - TypeSize)))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	bool IsFileIsAudioW(const wchar_t* PathToFile)
	{
		int StringSize = 0;
		int TypeSize = 0;
		LPCWSTR szTypes[] = {
		L"mp3", L"MP3", L"wav", L"WAV", L"flac", L"FLAC", L"aiff", L"AIFF", L"ogg", L"OGG", L"opus", L"OPUS", L"alac", L"ALAC", L"aac", L"AAC", L"m4a", L"M4A" };

		if (PathToFile)
		{
			for (size_t i = wcslen(PathToFile); i > 0; i--)
			{
				WCHAR cbSymbol = PathToFile[i];

				if (cbSymbol == L'.') { break; }

				TypeSize++;
			}

			if (TypeSize)
			{
				for (LPCWSTR CurrentType : szTypes)
				{
					if (!wcscmp(CurrentType, (&PathToFile[0] + StringSize - TypeSize)))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	void CloseThisHandle(OSRHandle& OutHandle) override
	{
		if (OutHandle) { CloseHandle(OutHandle); };
	}

	void Release() override
	{
		CloseThisHandle(LocalHandle);

		delete this;
	}

	IObject* CloneObject() override
	{
		return new IOSRWin32FileSystem(LocalHandle);
	}
};

#else
class IOSRPosixFileSystem : public IOSRFileSystem
{
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

public:
	void Open(const char* PathToFile, u8*& pOutStream, size_t& OutSize) override
	{
		int FileHandle = 0;
		int err = 0;
		size_t CurrentPos = 0;

		if ((FileHandle = _open(PathToFile, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE)) == -1) { return; }
		CurrentPos = lseek(FileHandle, 0, SEEK_CUR);
		OutSize = lseek(FileHandle, 0, SEEK_END);	// get size 

		pOutStream = (u8*)FastAlloc(OutSize);

		if ((err = read(FileHandle, pOutStream, OutSize)) == -1) { FREEKERNELHEAP(pOutStream); return; }
	}

	void OpenHandle(const char* PathToFile, OSRHandle& OutHandle) override
	{
		int FileHandle = 0;
		int err = 0;
		size_t CurrentPos = 0;

		if ((FileHandle = _open(PathToFile, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE)) == -1) { return; }
		OutHandle = (OSRHandle)FileHandle;
	}
};
#endif

class IAllocatorPool : public IObject
{
public:
	IAllocatorPool(void* CurrentPointer1, size_t CurrentSize1)
	{
		CurrentSize = CurrentSize1;
		CurrentPointer = CurrentPointer1;
	}

	void* new_pull(void* Data, size_t Size) 
	{
		void* LocalData = nullptr;
		size_t TempSize = CurrentSize + Size;
		ptrdiff_t PointerTo = 0;

		LocalData = AdvanceAlloc(TempSize, HEAP_MEMORY_ALLOC);
		PointerTo = ptrdiff_t(LocalData) + CurrentSize;

		memcpy_s(LocalData, TempSize, CurrentPointer, CurrentSize);
		memcpy_s((void*)PointerTo, Size, Data, Size);

		FREEKERNELHEAP(CurrentPointer);
		CurrentPointer = LocalData;
		CurrentSize = TempSize;

		return LocalData;
	}

	void* create_pointer()
	{
		void* LocalPointer = nullptr;
		
		LocalPointer = AdvanceAlloc(CurrentSize, HEAP_MEMORY_ALLOC);
		memcpy_s(LocalPointer, CurrentSize, CurrentPointer, CurrentSize);

		FREEKERNELHEAP(CurrentPointer);
		CurrentPointer = LocalPointer;

		return LocalPointer;
	}

	void Release() override
	{
		FREEKERNELHEAP(CurrentPointer);

		delete this;
	}

	IObject* CloneObject() override
	{
		return new IAllocatorPool(CurrentPointer, CurrentSize);
	}

private:
	void* CurrentPointer;
	size_t CurrentSize;
};

typedef enum HostFormatType
{
	MONO_22050_U8,
	MONO_22050_S16,
	MONO_22050_S24,
	MONO_22050_S32,
	MONO_22050_F32,

	STEREO_22050_U8,
	STEREO_22050_S16,
	STEREO_22050_S24,
	STEREO_22050_S32,
	STEREO_22050_F32,

	MONO_44100_U8,
	MONO_44100_S16,
	MONO_44100_S24,
	MONO_44100_S32,
	MONO_44100_F32,

	STEREO_44100_U8,
	STEREO_44100_S16,
	STEREO_44100_S24,
	STEREO_44100_S32,
	STEREO_44100_F32,

	TRIPLE_44100_U8,	// 2.1 systems
	TRIPLE_44100_S16,
	TRIPLE_44100_S24,
	TRIPLE_44100_S32,
	TRIPLE_44100_F32,

	QUAD_44100_U8,
	QUAD_44100_S16,
	QUAD_44100_S24,
	QUAD_44100_S32,
	QUAD_44100_F32,

	HEXA_44100_U8,		// 5.1 systems
	HEXA_44100_S16,
	HEXA_44100_S24,
	HEXA_44100_S32,
	HEXA_44100_F32,

	OCTA_44100_U8,		// 7.1 systems
	OCTA_44100_S16,
	OCTA_44100_S24,
	OCTA_44100_S32,
	OCTA_44100_F32,

	MONO_48000_U8,
	MONO_48000_S16,
	MONO_48000_S24,
	MONO_48000_S32,
	MONO_48000_F32,

	STEREO_48000_U8,
	STEREO_48000_S16,
	STEREO_48000_S24,
	STEREO_48000_S32,
	STEREO_48000_F32,

	TRIPLE_48000_U8,	// 2.1 systems
	TRIPLE_48000_S16,
	TRIPLE_48000_S24,
	TRIPLE_48000_S32,
	TRIPLE_48000_F32,

	QUAD_48000_U8,
	QUAD_48000_S16,
	QUAD_48000_S24,
	QUAD_48000_S32,
	QUAD_48000_F32,

	HEXA_48000_U8,		// 5.1 systems
	HEXA_48000_S16,
	HEXA_48000_S24,
	HEXA_48000_S32,
	HEXA_48000_F32,

	OCTA_48000_U8,		// 7.1 systems
	OCTA_48000_S16,
	OCTA_48000_S24,
	OCTA_48000_S32,
	OCTA_48000_F32,

	MONO_88200_U8,
	MONO_88200_S16,
	MONO_88200_S24,
	MONO_88200_S32,
	MONO_88200_F32,

	STEREO_88200_U8,
	STEREO_88200_S16,
	STEREO_88200_S24,
	STEREO_88200_S32,
	STEREO_88200_F32,

	TRIPLE_88200_U8,	// 2.1 systems
	TRIPLE_88200_S16,
	TRIPLE_88200_S24,
	TRIPLE_88200_S32,
	TRIPLE_88200_F32,

	QUAD_88200_U8,
	QUAD_88200_S16,
	QUAD_88200_S24,
	QUAD_88200_S32,
	QUAD_88200_F32,

	HEXA_88200_U8,		// 5.1 systems
	HEXA_88200_S16,
	HEXA_88200_S24,
	HEXA_88200_S32,
	HEXA_88200_F32,

	OCTA_88200_U8,		// 7.1 systems
	OCTA_88200_S16,
	OCTA_88200_S24,
	OCTA_88200_S32,
	OCTA_88200_F32,

	MONO_96000_U8,
	MONO_96000_S16,
	MONO_96000_S24,
	MONO_96000_S32,
	MONO_96000_F32,

	STEREO_96000_U8,
	STEREO_96000_S16,
	STEREO_96000_S24,
	STEREO_96000_S32,
	STEREO_96000_F32,

	TRIPLE_96000_U8,	// 2.1 systems
	TRIPLE_96000_S16,
	TRIPLE_96000_S24,
	TRIPLE_96000_S32,
	TRIPLE_96000_F32,

	QUAD_96000_U8,
	QUAD_96000_S16,
	QUAD_96000_S24,
	QUAD_96000_S32,
	QUAD_96000_F32,

	HEXA_96000_U8,		// 5.1 systems
	HEXA_96000_S16,
	HEXA_96000_S24,
	HEXA_96000_S32,
	HEXA_96000_F32,

	OCTA_96000_U8,		// 7.1 systems
	OCTA_96000_S16,
	OCTA_96000_S24,
	OCTA_96000_S32,
	OCTA_96000_F32,

	MONO_192000_U8,
	MONO_192000_S16,
	MONO_192000_S24,
	MONO_192000_S32,
	MONO_192000_F32,

	STEREO_192000_U8,
	STEREO_192000_S16,
	STEREO_192000_S24,
	STEREO_192000_S32,
	STEREO_192000_F32,

	TRIPLE_192000_U8,	// 2.1 systems
	TRIPLE_192000_S16,
	TRIPLE_192000_S24,
	TRIPLE_192000_S32,
	TRIPLE_192000_F32,

	QUAD_192000_U8,
	QUAD_192000_S16,
	QUAD_192000_S24,
	QUAD_192000_S32,
	QUAD_192000_F32,

	HEXA_192000_U8,		// 5.1 systems
	HEXA_192000_S16,
	HEXA_192000_S24,
	HEXA_192000_S32,
	HEXA_192000_F32,

	OCTA_192000_U8,		// 7.1 systems
	OCTA_192000_S16,
	OCTA_192000_S24,
	OCTA_192000_S32,
	OCTA_192000_F32,
};
