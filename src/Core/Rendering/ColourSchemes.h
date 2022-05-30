#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

class EditorColourScheme
{
	// 0xRRGGBBAA
	inline static int BackGroundColor = 0x25213100;
	inline static int TextColor = 0xF4F1DE00;
	inline static int MainColor = 0xDA115E00;
	inline static int MainAccentColor = 0x79235900;
	inline static int HighlightColor = 0xC7EF0000;

	inline static int Black = 0x00000000;
	inline static int White = 0xFFFFFF00;

	inline static int AlphaTransparent = 0x00;
	inline static int Alpha20 = 0x33;
	inline static int Alpha40 = 0x66;
	inline static int Alpha50 = 0x80;
	inline static int Alpha60 = 0x99;
	inline static int Alpha80 = 0xCC;
	inline static int Alpha90 = 0xE6;
	inline static int AlphaFull = 0xFF;

	static float GetR(int colorCode) { return (float)((colorCode & 0xFF000000) >> 24) / (float)(0xFF); }
	static float GetG(int colorCode) { return (float)((colorCode & 0x00FF0000) >> 16) / (float)(0xFF); }
	static float GetB(int colorCode) { return (float)((colorCode & 0x0000FF00) >> 8) / (float)(0xFF); }
	static float GetA(int alphaCode) { return ((float)alphaCode / (float)0xFF); }

	static ImVec4 GetColor(int c, int a = Alpha80) { return ImVec4(GetR(c), GetG(c), GetB(c), GetA(a)); }
	static ImVec4 Darken(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x - 1.0f * p), fmax(0.f, c.y - 1.0f * p), fmax(0.f, c.z - 1.0f * p), c.w); }
	static ImVec4 Lighten(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x + 1.0f * p), fmax(0.f, c.y + 1.0f * p), fmax(0.f, c.z + 1.0f * p), c.w); }

	static ImVec4 Disabled(ImVec4 c) { return Darken(c, 0.6f); }
	static ImVec4 Hovered(ImVec4 c) { return Lighten(c, 0.2f); }
	static ImVec4 Active(ImVec4 c) { return Lighten(ImVec4(c.x, c.y, c.z, 1.0f), 0.08f); }
	static ImVec4 ActiveD(ImVec4 c) { return Darken(ImVec4(c.x, c.y, c.z, 1.0f), 0.12f); }
	static ImVec4 HoveredD(ImVec4 c) { return Darken(ImVec4(c.x, c.y, c.z, 1.0f), 0.14f); }
	static ImVec4 Collapsed(ImVec4 c) { return Darken(c, 0.2f); }

public:
	static void SetColors(int backGroundColor, int textColor, int mainColor, int mainAccentColor, int highlightColor);
	static void ApplyTheme();
};