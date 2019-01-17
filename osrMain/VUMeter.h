/*********************************************************
* Copyright (C) VERTVER & FROWRIK, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR DirectX
**********************************************************
* VUMeter.h
* Main VU meter implementation
*********************************************************/
#pragma once
#include "stdafx.h"
#include "imgui_internal.h"

#define     IMGUI_LEVELS_WIDTH_MIN          90.0
#define     IMGUI_LEVELS_HEIGHT_MIN         100.0
#define     IMGUI_LEVELS_BG_COLOR           ImColor(19, 19, 19)
#define     IMGUI_LEVELS_BORDURE_COLOR      ImColor(0, 0, 0)

#define     IMGUI_LEVELS_PART1_PERCENT      0.70f
#define     IMGUI_LEVELS_PART2_PERCENT      0.90f


class VUMeter
{
public:
	VOID DrawLevels(float Width, float Height, float LinearLevelL, float LinearLevelR, float PeaklevelL, float PeakLevelR, bool PeakL, bool PeakR);

private:
	ImColor LevelsColorInterp(__int32 R1, __int32 G1, __int32 B1, __int32 R2, __int32 G2, __int32 B2, float Scale) 
	{
		return ImColor((__int32)(R1 + (R2 - R1)*Scale), (__int32)(G1 + (G2 - G1)*Scale), (__int32)(B1 + ((B2 - B1)*Scale)));
	}
};