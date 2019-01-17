/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* KernCPU.cpp
* CPU-part implementation
*********************************************************/
#include "stdafx.h"

#ifdef WIN32
CPUID::CPUID()
{
	std::array<int, 4> iCPUArray = { };

	__cpuid(iCPUArray.data(), 0);
	nIds_ = iCPUArray[0];

	for (int i = 0; i <= nIds_; i++)
	{
		__cpuidex(iCPUArray.data(), i, NULL);
		data_.push_back(iCPUArray);
	}

	if (nIds_ >= 1)
	{
		f_1_ECX = data_[1][2];
		f_1_EDX = data_[1][3];
	}

	if (nIds_ >= 7)
	{
		f_7_EBX = data_[7][1];
		f_7_ECX = data_[7][2];
	}

	// get highest valid extended CPUID
	__cpuid(iCPUArray.data(), 0x80000000);
	nExIds_ = iCPUArray[0];

	for (int i = 0x80000000; i <= nExIds_; ++i)
	{
		__cpuidex(iCPUArray.data(), i, 0);
		extdata_.push_back((iCPUArray));
	}

	if (nExIds_ >= 0x80000001)
	{
		f_81_ECX = extdata_[1][2];
		f_81_EDX = extdata_[1][3];
	}
}

BOOL
CPUID::IsFeaturePresent(
	Instruction cpuInstruction
)
{
	BOOL bRet = FALSE;
	switch (cpuInstruction)
	{
	case SSE:
		bRet = f_1_EDX[25];
		break;
	case SSE2:
		bRet = f_1_EDX[26];
		break;
	case SSE3:
		bRet = f_1_ECX[0];
		break;
	case SSSE3:
		bRet = f_1_ECX[9];
		break;
	case SSE41:
		bRet = f_1_ECX[19];
		break;
	case AVX:
		bRet = f_1_ECX[28];
		break;
	case AVX2:
		bRet = f_7_EBX[5];
		break;
	case AVX512:
		bRet = f_7_EBX[16];		// AVX512F
		break;
	}

	return !!bRet;
}

VOID
CPUExt::SwapData(
	VOID** pData,
	UINT64 uSize,
	DWORD dwFlags
)
{
	// Important: function must get only RAW data.
	if (dwFlags == 2)
	{
		BYTE* Array8 = reinterpret_cast<BYTE*>(*pData);

		DWORD64 i = 0;

		if (IsFeaturePresent(SSSE3))
		{
			// SSSE3
			for (DWORD64 j = i; j < ((uSize / 2) / 16); j++)
			{
				const __m128i xmmShuffleRev = _mm_set_epi8(0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF);

				__m128i xmmLow = _mm_loadu_si128(reinterpret_cast<__m128i*>(&Array8[i]));
				__m128i xmmUp = _mm_loadu_si128(reinterpret_cast<__m128i*>(&Array8[uSize - i - 16]));

				xmmLow = _mm_shuffle_epi8(xmmLow, xmmShuffleRev);
				xmmUp = _mm_shuffle_epi8(xmmUp, xmmShuffleRev);

				_mm_storeu_si128(reinterpret_cast<__m128i*>(&Array8[i]), xmmUp);
				_mm_storeu_si128(reinterpret_cast<__m128i*>(&Array8[uSize - i - 16]), xmmLow);

				i += 16;
			}
		}

		// bswap64
		for (DWORD64 j = i; j < ((uSize / 2) / 8); j++)
		{
			DWORD64 uLow = _byteswap_uint64(*reinterpret_cast<DWORD64*>(&Array8[i]));
			DWORD64 uUp = _byteswap_uint64(*reinterpret_cast<DWORD64*>(&Array8[uSize - i - 8]));

			*reinterpret_cast<DWORD64*>(&Array8[i]) = uUp;
			*reinterpret_cast<DWORD64*>(&Array8[uSize - i - 8]) = uLow;

			i += 8;
		}

		// bswap32
		for (DWORD64 j = i / 4; j < ((uSize / 2) / 4); j++)
		{
			DWORD dwLow = _byteswap_ulong(*reinterpret_cast<DWORD*>(&Array8[i]));
			DWORD dwUp = _byteswap_ulong(*reinterpret_cast<DWORD*>(&Array8[uSize - i - 4]));

			*reinterpret_cast<DWORD*>(&Array8[i]) = dwUp;
			*reinterpret_cast<DWORD*>(&Array8[uSize - i - 4]) = dwLow;

			i += 4;
		}

		// bswap16
		for (DWORD64 j = i / 2; j < ((uSize / 2) / 2); j++)
		{
			WORD wLow = _byteswap_ushort(*reinterpret_cast<WORD*>(&Array8[i]));
			WORD wUp = _byteswap_ushort(*reinterpret_cast<WORD*>(&Array8[uSize - i - 2]));

			*reinterpret_cast<WORD*>(&Array8[i]) = wUp;
			*reinterpret_cast<WORD*>(&Array8[uSize - i - 2]) = wLow;

			i += 2;
		}

		// serial
		for (; i < uSize / 2; i++)
		{
			BYTE byteTemp(Array8[i]);
			Array8[i] = Array8[uSize - i - 1];
			Array8[uSize - i - 1] = byteTemp;
		}
		// 5506
	}
	else
	{
// 		// very slow, but stable
// 		BYTE* pLocalByte = (BYTE*)*pData;
// 
// 		for (DWORD i = 0; i < uSize; i++)
// 		{
// 			byteVector.push_back(pLocalByte[i]);
// 		}
// 		std::reverse(std::begin(byteVector), std::end(byteVector));
// 		*pData = (BYTE*)byteVector[0];
	}
}
#endif