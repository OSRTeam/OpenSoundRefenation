#include "stdafx.h"
#include "osrMath.h"
#pragma hdrstop

VOID
Math::BuildWindow(
	WindowTypes windowType,
	float* pData,
	float* pFFT,
	size_t DataSize
)
{
	ASSERT2((DataSize != 1), L"Division on zero is impossible");
	ASSERT2(DataSize, L"Division on zero is impossible");

	if (!pData) { return; };
	if (!pFFT) { return; };

	float alpha = 0;
	float alpha1 = 0;
	float alpha2 = 0;
	float alpha3 = 0;

	// get process our window
	switch (windowType)
	{
	case HANN_WINDOW:
		for (int i = 0; i < DataSize; i++)
		{
			pData[i] = .5f * (1 - cos(2 * M_PI * i / (DataSize - 1.0f)));
		}
		break;
	case HAMMING_WINDOW:
		for (int i = 0; i < DataSize; i++)
		{
			pData[i] = .54f - .46f * cos(2 * M_PI * i / (float)DataSize);
		}
		break;
	case BLACKMAN_WINDOW:
		alpha = 0.16f;
		alpha1 = (1 - alpha) / 2.0f;
		alpha2 = 0.5f;
		alpha3 = alpha / 2;

		for (int i = 0; i < DataSize; i++)
		{
			pData[i] = alpha1 - alpha2 * cos((2 * M_PI * i) / (DataSize - 1.0f)) + alpha3 * cos((4 * M_PI * i) / (DataSize - 1.0f));
		}
		break;
	case BLACKMANHARRIS_WINDOW:
		alpha = 0.35875f;
		alpha1 = 0.48829f;
		alpha2 = 0.14128f;
		alpha3 = 0.01168f;

		for (int i = 0; i < DataSize; i++)
		{
			pData[i] = alpha - alpha1 * cos((2 * M_PI * i) / (DataSize - 1.0f))
				+ alpha2 * cos((4 * M_PI * i) / (DataSize - 1.0f))
				- alpha3 * cos((6 * M_PI * i) / (DataSize - 1.0f));
		}
		break;
	case KEISER_ALPHA2:
		KBDWindow(pData, (int)DataSize, 2);
		break;
	case KEISER_ALPHA8:
		KBDWindow(pData, (int)DataSize, 8);
		break;
	case KEISER_ALPHA24:
		KBDWindow(pData, (int)DataSize, 24);
		break;
	default:
		break;
	}

	// apply our window to data
	for (int i = 0; i < DataSize; i++)
	{
		pFFT[i] *= pData[i];
	}
}

VOID
Math::GetVUValue(
	u8* pData, 
	float* pVUValue,
	u32 dwSampleRate,
	u32 dwBlockSize,
	u8 cbChannels,
	u8 cbBits
)
{
	//float* fChannels = (float*)FastAlloc(sizeof(float) * cbChannels);

	//for (size_t i = 0; i < dwBlockSize; i++)
	//{

	//}
}
