/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR entry-point
**********************************************************
* OSRMath.h
* Basic math implementation for using in OSR
*********************************************************/
#pragma once
#include <math.h>

#define M_E        2.71828182845904523536f   // e
#define M_LOG2E    1.44269504088896340736f   // log2(e)
#define M_LOG10E   0.434294481903251827651f  // log10(e)
#define M_LN2      0.693147180559945309417f  // ln(2)
#define M_LN10     2.30258509299404568402f   // ln(10)
#define M_PI       3.14159265358979323846f   // pi
#define M_PI_2     1.57079632679489661923f   // pi/2
#define M_PI_4     0.785398163397448309616f  // pi/4
#define M_1_PI     0.318309886183790671538f  // 1/pi
#define M_2_PI     0.636619772367581343076f  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390f   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880f   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401f  // 1/sqrt(2)

inline
float
BesselI0(
	float x
)
{
	static float denominator;
	static float numerator;
	static float z;

	if (x == 0.0) 
	{
		return 1.0;
	}
	else 
	{
		z = x * x;
		numerator = (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z * (z *
			(z * 0.210580722890567e-22f + 0.380715242345326e-19f) +
			0.479440257548300e-16f) + 0.435125971262668e-13f) +
			0.300931127112960e-10f) + 0.160224679395361e-7f) +
			0.654858370096785e-5f) + 0.202591084143397e-2f) +
			0.463076284721000e0f) + 0.754337328948189e2f) +
			0.830792541809429e4f) + 0.571661130563785e6f) +
			0.216415572361227e8f) + 0.356644482244025e9f) +
			0.144048298227235e10f);

		denominator = (z * (z * (z - 0.307646912682801e4f) +
			0.347626332405882e7f) - 0.144048298227235e10f);
	}

	return -numerator / denominator;
}

inline
void
KBDWindow(
	float* window,
	int size,
	float alpha
) 
{
	static float sumvalue = 0.0f;
	int i;

	for (i = 0; i < size / 2; i++)
	{
		sumvalue += BesselI0(M_PI * alpha * sqrt(1.0f - pow(4.0f*i / size - 1.0f, 2)));
		window[i] = sumvalue;
	}

	// need to add one more value to 
	// the nomalization factor at size / 2
	sumvalue += BesselI0(M_PI * alpha * sqrt(1.0f - pow(4.0f*(size / 2) / size - 1.0f, 2)));

	// normalize the window and fill in 
	// the righthand side of the window
	for (i = 0; i < size / 2; i++) 
	{
		window[i] = sqrt(window[i] / sumvalue);
		window[size - 1 - i] = window[i];
	}
}

enum WindowTypes
{
	HANN_WINDOW = 1,
	HAMMING_WINDOW,
	BLACKMAN_WINDOW,
	BLACKMANHARRIS_WINDOW,
	KEISER_ALPHA2,
	KEISER_ALPHA8,
	KEISER_ALPHA24
};

class Math
{
public:
	void BuildWindow(WindowTypes windowType, float* pData, float* pFFT, size_t DataSize);
	void GetVUValue(u8* pData, float* pVUValue, u32 dwSampleRate, u32 dwBlockSize, u8 cbChannels, u8 cbBits);
};
