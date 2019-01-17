/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* OSRTypes.h
* Basic types for using in OSR
*********************************************************/
#pragma once
#include <math.h>
#include <stdint.h>

#define maxmin(a, minimum, maximum)  min(max(a, minimum), maximum)

typedef int8_t i8;
typedef i8* PI8, LPI8;
typedef uint8_t u8;
typedef u8* PU8, LPU8;

typedef int16_t i16;
typedef i16* PI16, LPI16;
typedef uint16_t u16;
typedef u16* PU16, LPU16;

typedef int32_t i32;
typedef i32* PI32, LPI32;
typedef uint32_t u32;
typedef u32* PU32, LPU32;

typedef int64_t i64;
typedef i64* PI64, LPI64;
typedef uint64_t u64;
typedef u64* PU64, LPU64;

typedef float f32;
typedef double f64;

#ifdef WITHOUT_WIDECHAR
typedef i16 WideChar;
#else
typedef wchar_t WideChar;
#endif

#define MAX_U32VALUE u32(-1)
#define MAX_I32VALUE (MAX_U32VALUE / 2)

typedef struct
{
	u8 cbByte1;
	u8 cbByte2;
	u8 cbByte3;
} uint24_t, u24, *PU24;

typedef struct
{
	u32 cbSign : 1;
	u32 cbData : 23;
} int24_t, i24, *PI24;

typedef struct
{
	u16 cbFraction : 10;
	u16 cbExponent : 5;
	u16 cbSign : 1;
} f16;

typedef struct
{
	u8 bit1 : 1;
	u8 bit2 : 1;
	u8 bit3 : 1;
	u8 bit4 : 1;
	u8 bit5 : 1;
	u8 bit6 : 1;
	u8 bit7 : 1;
	u8 bit8 : 1;
} BIT_BYTE;

#ifdef _HAS_CXX17
constexpr i32 iMinus = 0 - MAX_I32VALUE;
constexpr i32 iPlus = MAX_I32VALUE;
#else
const i32 iMinus = 0 - MAX_I32VALUE;
const i32 iPlus = MAX_I32VALUE;
#endif

inline 
float
f16tof32(f16 fValue)
{
	u16 wValue = *(PU16)(&fValue);
	float fReturn = .0f;

	fReturn = ((float)(((wValue & 0x8000) << 16) | (((wValue & 0x7c00) + 0x1C000) << 13) | ((wValue & 0x03FF) << 13)));

	return fReturn;
}

inline
f16
f32tof16(f32 fValue)
{
	u16 wValue = 0;
	u32 dwValue = *(PU32)(&fValue);

	wValue = ((u16)(((dwValue >> 16) & 0x8000) | ((((dwValue & 0x7f800000) - 0x38000000) >> 13) & 0x7c00) | ((dwValue >> 13) & 0x03ff)));

	return (*(f16*)&wValue);
}

inline
f32
i16tof32(i16 wValue)
{
	f32 fValue = .0f;
	fValue = (float)wValue / 32768.0f;

	return fValue;
}

inline
i16
f32toi16(f32 fValue)
{
	i16 iValue = 0;

	iValue = maxmin(((i16)(fValue * 32768.0f)), -32768, 32767);

	return iValue;
}

inline
i24
f32toi24(f32 fValue)
{
	i32 iValue = 0;

	iValue = maxmin(((i32)(fValue * iPlus)), iMinus, iPlus);
	i32 Temp_Out = iValue >> 8;

	return *(i24*)((&(Temp_Out)));
}

inline
f32
i24tof32(i24 iValue)
{
	i32 dwValue = 8 << (*(i32*)(&iValue));
	f32 fValue = .0f;

	fValue = (float)dwValue / iPlus;

	return fValue;
}

#ifdef __cplusplus
class WMString
{
public:
	WMString(const WideChar* String)
	{
		DataSize = wcslen(String);
		DataSize++;
		Data = (WideChar*)malloc(DataSize * sizeof(WideChar));
		Wide = true;

		memset(Data, 0, DataSize * sizeof(WideChar));
		memcpy_s(Data, DataSize * sizeof(WideChar), String, (DataSize * sizeof(WideChar)) - 2);
	}

	WMString(const char* String)
	{
		DataSize = strlen(String);
		DataSize++;
		Data = (WideChar*)malloc(DataSize * sizeof(WideChar));
		Wide = false;

		memset(Data, 0, DataSize * sizeof(WideChar));
		
		for (size_t i = 0; i < DataSize; i++)
		{
			Data[i] = String[i];
		}
	}

	~WMString() 
	{ 
		free(Data);
	}

	WMString& operator= (const WideChar* Wc) 
	{
		DataSize = wcslen(Wc);
		DataSize++;
		Data = (WideChar*)malloc(DataSize * sizeof(WideChar));
		Wide = true;

		memset(Data, 0, DataSize * sizeof(WideChar));
		memcpy(Data, Wc, (DataSize * sizeof(WideChar)) - 2);

		return (WMString&)*this;
	}

	WMString& operator= (const char* Mb)
	{
		DataSize = strlen(Mb);
		DataSize++;
		Data = (WideChar*)malloc(DataSize * sizeof(WideChar));
		Wide = false;

		memset(Data, 0, DataSize * sizeof(WideChar));

		for (size_t i = 0; i < DataSize; i++)
		{
			Data[i] = Mb[i];
		}

		return (WMString&)*this;
	}

	const WideChar* c_wstr()
	{
		WideChar* WideString = (WideChar*)malloc(DataSize * sizeof(WideChar));

		memset(WideString, 0, DataSize * sizeof(WideChar));

		for (size_t i = 0; i < DataSize; i++)
		{
			WideString[i] = Data[i];
		}

		return WideString;
	}

	const char* c_str()
	{
		char* ByteString = (char*)malloc(DataSize * sizeof(char));

		memset(ByteString, 0, DataSize);

		if (!Wide)
		{
			for (size_t i = 0; i < DataSize; i++)
			{
				ByteString[i] = (char)Data[i];
			}
		}
		else
		{
			size_t Converted = 0;
			wcstombs_s(&Converted, ByteString, DataSize, Data, DataSize);
		}

		return ByteString;
	}

	size_t size() { return DataSize; }
	bool is_wide() { return Wide; }

private:
	bool Wide;
	size_t DataSize;
	WideChar* Data;
};
#endif
