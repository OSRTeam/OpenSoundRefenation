/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* OSRAlloc.inl
* allocator header
*********************************************************/

template
<class T>
inline
T*
AllocateClass()
{
	return new T;
}

#ifdef WIN32
template
<typename T>
__forceinline
T*
AllocatePointer()
{
	return (T*)HeapAlloc(GetKernelHeap(), HEAP_ZERO_MEMORY, sizeof(T));
}

template
<typename T, SIZE_T Size>
__forceinline
T*
AllocatePointer()
{
	ASSERT2(Size, L"Can't alloc file with 0 size");
	return (T*)HeapAlloc(GetKernelHeap(), HEAP_ZERO_MEMORY, Size);
}

template
<typename T, SIZE_T Size>
__forceinline
T*
AllocateFile()
{
	ASSERT2(Size, L"Can't alloc file with 0 size");
	return (T*)VirtualAlloc(NULL, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

__forceinline
LPVOID
FastAlloc(
	SIZE_T uSize
)
{
	ASSERT2(uSize, L"Alloc size can't be 0");
	ASSERT2(GetKernelHeap(), L"No kernel heap");
	return HeapAlloc(GetKernelHeap(), HEAP_ZERO_MEMORY, uSize);
}

__forceinline
LPVOID
MapFile(
	SIZE_T PointerSize,
	LPCWSTR lpSharedMemoryName
)
{
	LPVOID lpSharedMemory = nullptr;

	// create file mapping at paging file
	HANDLE hSharedMemory = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, (DWORD)PointerSize, lpSharedMemoryName);
	lpSharedMemory = MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, NULL, NULL, PointerSize);

	return lpSharedMemory;
}

__forceinline
LPVOID
MapFile(
	SIZE_T PointerSize,
	LPCWSTR lpSharedMemoryName,
	HANDLE FileHandle
)
{
	LPVOID lpSharedMemory = nullptr;

	// create file mapping at paging file
	HANDLE hSharedMemory = CreateFileMappingW(FileHandle, NULL, PAGE_READONLY, NULL, (DWORD)PointerSize, lpSharedMemoryName);
	lpSharedMemory = MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, NULL, NULL, PointerSize);

	return lpSharedMemory;
}

/***********************************************************
* WINDOWS FUNCTION
* dwType reference:
* MAPPED_MEMORY_ALLOC - pointer from mapped pagefile memory
* HEAP_MEMORY_ALLOC - pointer from kernel heap
* VIRTUAL_MEMORY_ALLOC - pointer from mapped memory
* NULL - pointer from process heap
************************************************************/
__forceinline
LPVOID
AdvanceAlloc(
	SIZE_T PointerSize,
	DWORD dwType
)
{
	LPVOID pRet = nullptr;

	switch (dwType)
	{
	case MAPPED_MEMORY_ALLOC:
		pRet = MapFile(PointerSize, NULL);
		break;
	case VIRTUAL_MEMORY_ALLOC:
		pRet = VirtualAlloc(NULL, PointerSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		break;
	case HEAP_MEMORY_ALLOC:
		pRet = HeapAlloc(GetKernelHeap(), HEAP_ZERO_MEMORY, PointerSize);
		break;
	case NULL:
		pRet = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, PointerSize);
		break;
	}

	return pRet;
}

__forceinline
VOID
FreePointer(
	LPVOID pPointer,
	SIZE_T PointerSize,
	DWORD dwType
)
{
	switch (dwType)
	{
	case MAPPED_MEMORY_ALLOC:
		if (pPointer) { ASSERT2(UnmapViewOfFile(pPointer), L"Can't free pointer. Maybe file doesn't allocated by mapped memory?"); }
		pPointer = nullptr;
		break;
	case VIRTUAL_MEMORY_ALLOC:
		if (pPointer) { ASSERT2(VirtualFree(pPointer, PointerSize, MEM_RELEASE | MEM_DECOMMIT), L"Can't free pointer. Maybe file doesn't allocated by virtual memory?"); }
		pPointer = nullptr;
		break;
	case HEAP_MEMORY_ALLOC:
		if (pPointer) { ASSERT2(HeapFree(GetKernelHeap(), NULL, pPointer), L"Can't free pointer. Maybe file doesn't allocated by kernel heap?"); }
		pPointer = nullptr;
		break;
	case NULL:
		if (pPointer) { ASSERT2(HeapFree(GetProcessHeap(), NULL, pPointer), L"Can't free pointer. Maybe file doesn't allocated by process heap?"); }
		pPointer = nullptr;
		break;
	default:
		THROW2(L"Can't free pointer because type is N/A");
		break;
	}
}

// DEPRECATED
__forceinline
VOID
UnloadFile(
	LPVOID pFile
)
{
	ASSERT1(HeapFree(GetKernelHeap(), NULL, pFile), L"Can't free pointer from kernel heap");
	pFile = nullptr;
}

#else
template
<typename T>
inline
T*
AllocatePointer()
{
	return (T*)malloc(sizeof(T));
}

template
<typename T, size_t Size>
inline
T*
AllocatePointer()
{
	ASSERT2(Size, L"Can't alloc file with 0 size");
	return (T*)malloc(Size);
}

template
<typename T, size_t Size>
inline
T*
AllocateFile()
{
	// map pointer by POSIX mapping
	void* pFile = mmap(NULL, Size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT2(pFile, "Can't map pointer");

	// map fixed pointer 
	pFile = mmap(pFile, Size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
	ASSERT2(pFile, "Can't alloc fixed memory");

	// protect mapped memory for R/W only
	mprotect(pFile, Size, PROT_READ | PROT_WRITE);
	return pFile;
}

inline
void*
FastAlloc(
	size_t uSize
)
{
	ASSERT2(uSize, L"Alloc size can't be 0");
	return malloc(uSize);
}

/***********************************************************
* POSIX FUNCTION
* dwType reference:
* HEAP_MEMORY_ALLOC & NULL - pointer from kernel heap
* VIRTUAL_MEMORY_ALLOC & MAPPED_MEMORY_ALLOC -
* pointer from mapped memory
*
* #NOTE: for free pointer in POSIX systems, we must know
* the size of it, because system doesn't know about it.
************************************************************/
inline
void*
AdvanceAlloc(
	size_t PointerSize,
	unsigned long  dwType
)
{
	void* pRet = nullptr;

	switch (dwType)
	{
	case VIRTUAL_MEMORY_ALLOC:
	case MAPPED_MEMORY_ALLOC:
		// map pointer by POSIX mapping
		pRet = mmap(NULL, PointerSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		ASSERT2(pFile, "Can't map pointer");

		// map fixed pointer 
		pRet = mmap(pRet, PointerSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
		ASSERT2(pRet, "Can't alloc fixed memory");

		// protect mapped memory for R/W only
		mprotect(pRet, PointerSize, PROT_READ | PROT_WRITE);
		break;
	case HEAP_MEMORY_ALLOC:
	case NULL:
		pRet = malloc(PointerSize);
		break;
	}

	return pRet;
}

inline
void*
MapFile(
	size_t PointerSize
)
{
	return AdvanceAlloc(PointerSize, MAPPED_MEMORY_ALLOC);
}

inline
void
FreePointer(
	void* pPointer,
	size_t PointerSize,
	unsigned long dwType
)
{
	switch (dwType)
	{
	case VIRTUAL_MEMORY_ALLOC:
	case MAPPED_MEMORY_ALLOC:
		// free paged memory by POSIX
		if (pPointer)
		{
			mprotect(pPointer, pointerSize, PROT_NONE);
			munmap(pPointer, pointerSize);
		}
		pPointer = nullptr;
		break;
	case HEAP_MEMORY_ALLOC:
	case NULL:
		if (pPointer)
		{
			free(pPointer);
		}
		pPointer = nullptr;
		break;
	default:
		THROW2("Can't free pointer because type is N/A");
		break;
	}
}

inline
void
UnloadFile(
	void* pFile
)
{
	// #NOTE: it's must be allocated by malloc
	if (pFile)
	{
		free(pFile);
		pFile = nullptr;
	}
}
#endif

#define FREEPROCESSHEAP(Pointer)		if (Pointer) { FreePointer(Pointer, NULL, NULL);					Pointer = nullptr; }
#define FREEKERNELHEAP(Pointer)			if (Pointer) { FreePointer(Pointer, NULL, HEAP_MEMORY_ALLOC);		Pointer = nullptr; }
#define FREEVIRTUALMEM(Pointer, Size)	if (Pointer) { FreePointer(Pointer, Size, VIRTUAL_MEMORY_ALLOC);	Pointer = nullptr; }
#define FREEMAPPEDMEM(Pointer, Size)	if (Pointer) { FreePointer(Pointer, Size, MAPPED_MEMORY_ALLOC);		Pointer = nullptr; }		// in windows set Size to NULL
