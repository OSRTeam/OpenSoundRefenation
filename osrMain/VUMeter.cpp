/*********************************************************
* Copyright (C) VERTVER & FROWRIK, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR DirectX
**********************************************************
* VUMeter.cpp
* Main VU meter implementation
*********************************************************/
#include "stdafx.h"
#include "VUMeter.h"

VOID
VUMeter::DrawLevels(
	float Width,
	float Height, 
	float LinearLevelL,
	float LinearLevelR, 
	float PeaklevelL, 
	float PeakLevelR,
	bool PeakL, 
	bool PeakR
)
{
	Width = max(Width, IMGUI_LEVELS_WIDTH_MIN);
	Height = max(Height, IMGUI_LEVELS_HEIGHT_MIN);

	ImGui::BeginGroup();
	ImDrawList* DrawList = ImGui::GetWindowDrawList();

	ImVec2 PosStart = ImGui::GetCursorScreenPos();

	ImVec2 PosEnd = PosStart + ImVec2(Width, Height);
	float     LevelsWidth = Width / 2.0f;
	float     LevelsWidthDivide = LevelsWidth / 2.0f;
	float     LevelsHeight = Height - Width / 8.0f;
	float     LevelsX = PosStart.x + Width / 8.0f;
	float     LevelsY = PosStart.y + Width / 4.0f;
	float     LevelsX2 = LevelsX + LevelsWidth;
	float     LevelsY2 = LevelsY + LevelsHeight - Width / 4.0f;
	float     PeakInvHeight = Width / 8.0f;

	// draw background
	DrawList->AddRectFilled(PosStart, PosEnd, IMGUI_LEVELS_BG_COLOR);

	// add pading
	PosStart = PosStart + ImVec2(1, 1);
	PosEnd = PosEnd - ImVec2(1, 1);

	// draw background
	DrawList->AddRect(PosStart, PosEnd, IMGUI_LEVELS_BORDURE_COLOR);

	///////////////////////////////
	// draw level L R
	ImColor ColorMin, ColorMax;
	ImVec2 LevelLStart, LevelLEnd, LevelRStart, LevelREnd;

	float Part0ScaleL = min(1.0f, max(0.0f, LinearLevelL)*(1.0f / IMGUI_LEVELS_PART1_PERCENT));
	float Part1ScaleL = min(1.0f, max(0.0f, LinearLevelL - IMGUI_LEVELS_PART1_PERCENT)*(1.0f / (IMGUI_LEVELS_PART2_PERCENT - IMGUI_LEVELS_PART1_PERCENT)));
	float Part2ScaleL = min(1.0f, max(0.0f, LinearLevelL - IMGUI_LEVELS_PART2_PERCENT)*(1.0f / (1.00f - IMGUI_LEVELS_PART2_PERCENT)));

	float Part0ScaleR = min(1.0f, max(0.0f, LinearLevelR)*(1.0f / IMGUI_LEVELS_PART1_PERCENT));
	float Part1ScaleR = min(1.0f, max(0.0f, LinearLevelR - IMGUI_LEVELS_PART1_PERCENT)*(1.0f / (IMGUI_LEVELS_PART2_PERCENT - IMGUI_LEVELS_PART1_PERCENT)));
	float Part2ScaleR = min(1.0f, max(0.0f, LinearLevelR - IMGUI_LEVELS_PART2_PERCENT)*(1.0f / (1.00f - IMGUI_LEVELS_PART2_PERCENT)));

	float Part0SizeScaleL = 1.0 - min(IMGUI_LEVELS_PART1_PERCENT, LinearLevelL);
	float Part1SizeScaleL = 1.0 - min(IMGUI_LEVELS_PART2_PERCENT, LinearLevelL);
	float Part2SizeScaleL = 1.0 - LinearLevelL;

	float Part0SizeScaleR = 1.0 - min(IMGUI_LEVELS_PART1_PERCENT, LinearLevelR);
	float Part1SizeScaleR = 1.0 - min(IMGUI_LEVELS_PART2_PERCENT, LinearLevelR);
	float Part2SizeScaleR = 1.0 - LinearLevelR;

	// 1
	ColorMin = ImColor(55, 140, 70);
	ColorMax = LevelsColorInterp(55, 140, 70, 55, 233, 70, Part0ScaleL);

	LevelLStart = ImVec2(LevelsX, LevelsY2);
	LevelLEnd = ImVec2(LevelsX + LevelsWidthDivide - 1.0f, LevelsY + (LevelsY2 - LevelsY)*Part0SizeScaleL);

	DrawList->AddRectFilledMultiColor(LevelLStart, LevelLEnd, ColorMin, ColorMin, ColorMax, ColorMax);

	// 2
	if (LinearLevelL > 0.70) {
		ColorMin = ImColor(55, 233, 70);
		ColorMax = LevelsColorInterp(55, 233, 70, 245, 212, 70, Part1ScaleL);

		LevelLStart = ImVec2(LevelsX, LevelsY + (LevelsY2 - LevelsY)*Part0SizeScaleL);
		LevelLEnd = ImVec2(LevelsX + LevelsWidthDivide - 1.0f, LevelsY + (LevelsY2 - LevelsY)*Part1SizeScaleL);

		DrawList->AddRectFilledMultiColor(LevelLStart, LevelLEnd, ColorMin, ColorMin, ColorMax, ColorMax);
	}

	// 3
	if (LinearLevelL > 0.90) {
		ColorMin = ImColor(245, 212, 70);
		ColorMax = LevelsColorInterp(245, 212, 70, 245, 70, 70, Part2ScaleL);

		LevelLStart = ImVec2(LevelsX, LevelsY + (LevelsY2 - LevelsY)*Part1SizeScaleL);
		LevelLEnd = ImVec2(LevelsX + LevelsWidthDivide - 1.0f, LevelsY + (LevelsY2 - LevelsY)*Part2SizeScaleL);

		DrawList->AddRectFilledMultiColor(LevelLStart, LevelLEnd, ColorMin, ColorMin, ColorMax, ColorMax);
	}

	ColorMin = ImColor(55, 140, 70);
	ColorMax = LevelsColorInterp(55, 140, 70, 55, 233, 70, Part0ScaleR);

	LevelLStart = ImVec2(LevelsX + LevelsWidthDivide, LevelsY2);
	LevelLEnd = ImVec2(LevelsX2, LevelsY + (LevelsY2 - LevelsY)*Part0SizeScaleR);

	DrawList->AddRectFilledMultiColor(LevelLStart, LevelLEnd, ColorMin, ColorMin, ColorMax, ColorMax);

	// 2
	if (LinearLevelR > 0.70) {
		ColorMin = ImColor(55, 233, 70);
		ColorMax = LevelsColorInterp(55, 233, 70, 245, 212, 70, Part1ScaleR);

		LevelLStart = ImVec2(LevelsX + LevelsWidthDivide, LevelsY + (LevelsY2 - LevelsY)*Part0SizeScaleR);
		LevelLEnd = ImVec2(LevelsX2, LevelsY + (LevelsY2 - LevelsY)*Part1SizeScaleR);

		DrawList->AddRectFilledMultiColor(LevelLStart, LevelLEnd, ColorMin, ColorMin, ColorMax, ColorMax);
	}

	// 3
	if (LinearLevelR > 0.90) {
		ColorMin = ImColor(245, 212, 70);
		ColorMax = LevelsColorInterp(245, 212, 70, 245, 70, 70, Part2ScaleR);

		LevelLStart = ImVec2(LevelsX + LevelsWidthDivide, LevelsY + (LevelsY2 - LevelsY)*Part1SizeScaleR);
		LevelLEnd = ImVec2(LevelsX2, LevelsY + (LevelsY2 - LevelsY)*Part2SizeScaleR);

		DrawList->AddRectFilledMultiColor(LevelLStart, LevelLEnd, ColorMin, ColorMin, ColorMax, ColorMax);
	}

	DrawList->AddLine(
		ImVec2(LevelsX, LevelsY + (LevelsY2 - LevelsY)*(1.0 - PeaklevelL)),
		ImVec2(LevelsX + LevelsWidthDivide - 1.0f, LevelsY + (LevelsY2 - LevelsY)*(1.0 - PeaklevelL)),
		ImColor(255, 255, 70)
	);

	DrawList->AddLine(
		ImVec2(LevelsX + LevelsWidthDivide, LevelsY + (LevelsY2 - LevelsY)*(1.0 - PeakLevelR)),
		ImVec2(LevelsX2, LevelsY + (LevelsY2 - LevelsY)*(1.0 - PeakLevelR)),
		ImColor(255, 255, 70)
	);


	// draw levels rect
	DrawList->AddRect(ImVec2(LevelsX, LevelsY), ImVec2(LevelsX2, LevelsY2), IMGUI_LEVELS_BORDURE_COLOR);

	// draw peek
	if (PeakL) DrawList->AddRectFilled(ImVec2(LevelsX, LevelsY - PeakInvHeight), ImVec2(LevelsX + LevelsWidthDivide, LevelsY), ImColor(245, 70, 70));
	if (PeakR)  DrawList->AddRectFilled(ImVec2(LevelsX + LevelsWidthDivide - 1.0f, LevelsY - PeakInvHeight), ImVec2(LevelsX2, LevelsY), ImColor(245, 70, 70));

	DrawList->AddRect(ImVec2(LevelsX, LevelsY - PeakInvHeight), ImVec2(LevelsX + LevelsWidthDivide, LevelsY), IMGUI_LEVELS_BORDURE_COLOR);
	DrawList->AddRect(ImVec2(LevelsX + LevelsWidthDivide - 1.0f, LevelsY - PeakInvHeight), ImVec2(LevelsX2, LevelsY), IMGUI_LEVELS_BORDURE_COLOR);

	// draw grid
	float GridLineTextLen = 12.0f;
	float GridLineLen = 8.0f;
	float GridLineMiniLen = 4.0f;
	float GridLineHeightPart = (LevelsY2 - LevelsY) / 60.0f;
	float TextSize = DrawList->_Data->FontSize * 0.8f;
	float TextCenter = TextSize / 2.0f;      // ImGui::GetTextLineHeight() / 2.0f;   // Next Fix it

	DWORD AdaptiveNum = 3 + 10.0 / GridLineHeightPart; 
	if (!AdaptiveNum) { AdaptiveNum = 1; }
	DWORD AdaptiveNum2 = AdaptiveNum / 3;                              

	for (DWORD i = 0; i < 60; i++) 
	{
		float AddY = GridLineHeightPart * (float)i;

		if ((i % AdaptiveNum) == 0) {
			DrawList->AddLine(ImVec2(LevelsX2, LevelsY + AddY), ImVec2(LevelsX2 + GridLineLen, LevelsY + AddY), ImColor(138, 138, 138));

			char StrTemp[128];
			__int32 value = 0 - i;

			snprintf(StrTemp, sizeof(STRING128), "%d", value);

			DrawList->AddText(NULL, TextSize,
				ImVec2(LevelsX2 + GridLineTextLen, LevelsY + AddY - TextCenter),
				ImColor(138, 138, 138),
				StrTemp, NULL);
		}
		else 
		{
			if ((i % AdaptiveNum2) == 0) DrawList->AddLine(ImVec2(LevelsX2, LevelsY + AddY), ImVec2(LevelsX2 + GridLineMiniLen, LevelsY + AddY), ImColor(138, 138, 138));
		}

	}

	float AddY = GridLineHeightPart * 60.0f;
	DrawList->AddLine(ImVec2(LevelsX2, LevelsY + AddY), ImVec2(LevelsX2 + GridLineLen, LevelsY + AddY), ImColor(138, 138, 138));

	DrawList->AddText(NULL, TextSize,
		ImVec2(LevelsX2 + GridLineTextLen, LevelsY + AddY - TextCenter),
		ImColor(138, 138, 138),
		"dB", NULL);

	ImGui::EndGroup();
}
