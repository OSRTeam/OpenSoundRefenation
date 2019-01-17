/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* KernCPU.h
* CPU-part implementation
*********************************************************/
#pragma once
#include "stdafx.h"

#ifdef WIN32
#include <intrin.h> 
#include <bitset>  
#include <array>  

enum Instruction
{
	SSE = 1,
	SSE2,
	SSE3,
	SSSE3,
	SSE41,
	AVX,
	AVX2,
	AVX512
};

class CPUID
{
public:
	CPUID();
	DLL_API BOOL IsFeaturePresent(Instruction cpuInstruction);

private:
	// 32-bit registers
	int nIds_;
	int nExIds_;
	std::bitset<32> f_1_ECX;
	std::bitset<32> f_1_EDX;
	std::bitset<32> f_7_EBX;
	std::bitset<32> f_7_ECX;
	std::bitset<32> f_81_ECX;
	std::bitset<32> f_81_EDX;

	std::vector<std::array<int, 4>> data_;
	std::vector<std::array<int, 4>> extdata_;
};

class CPUExt : public CPUID 
{
public:
	DLL_API VOID SwapData(VOID** pData, UINT64 uSize, DWORD dwFlags);

	std::vector<BYTE> byteVector;
};
#endif

