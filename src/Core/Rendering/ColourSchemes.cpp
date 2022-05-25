#include "ColourSchemes.h"

void EditorColourScheme::SetColors(int backGroundColor, int textColor, int mainColor, int mainAccentColor, int highlightColor)
{
	BackGroundColor = backGroundColor;
	TextColor = textColor;
	MainColor = mainColor;
	MainAccentColor = mainAccentColor;
	HighlightColor = highlightColor;
}

void EditorColourScheme::ApplyTheme()
{
    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_Text] = GetColor(TextColor);
    colors[ImGuiCol_TextDisabled] = Disabled(colors[ImGuiCol_Text]);
    colors[ImGuiCol_WindowBg] = GetColor(BackGroundColor);
    colors[ImGuiCol_ChildBg] = GetColor(Black, Alpha20);
    colors[ImGuiCol_PopupBg] = GetColor(BackGroundColor, Alpha90);
    colors[ImGuiCol_Border] = Lighten(GetColor(BackGroundColor), 0.4f);
    colors[ImGuiCol_BorderShadow] = GetColor(Black);
    colors[ImGuiCol_FrameBg] = GetColor(MainAccentColor, Alpha40);
    colors[ImGuiCol_FrameBgHovered] = Hovered(colors[ImGuiCol_FrameBg]);
    colors[ImGuiCol_FrameBgActive] = Active(colors[ImGuiCol_FrameBg]);
    colors[ImGuiCol_TitleBg] = GetColor(BackGroundColor, Alpha80);
    colors[ImGuiCol_TitleBgActive] = Active(colors[ImGuiCol_TitleBg]);
    colors[ImGuiCol_TitleBgCollapsed] = Collapsed(colors[ImGuiCol_TitleBg]);
    colors[ImGuiCol_MenuBarBg] = Darken(GetColor(BackGroundColor), 0.2f);
    colors[ImGuiCol_ScrollbarBg] = Lighten(GetColor(BackGroundColor, Alpha50), 0.4f);
    colors[ImGuiCol_ScrollbarGrab] = Lighten(GetColor(BackGroundColor), 0.3f);
    colors[ImGuiCol_ScrollbarGrabHovered] = Hovered(colors[ImGuiCol_ScrollbarGrab]);
    colors[ImGuiCol_ScrollbarGrabActive] = Active(colors[ImGuiCol_ScrollbarGrab]);
    colors[ImGuiCol_CheckMark] = GetColor(HighlightColor);
    colors[ImGuiCol_SliderGrab] = GetColor(HighlightColor);
    colors[ImGuiCol_SliderGrabActive] = Active(colors[ImGuiCol_SliderGrab]);
    colors[ImGuiCol_Button] = GetColor(MainColor, Alpha80);
    colors[ImGuiCol_ButtonHovered] = Hovered(colors[ImGuiCol_Button]);
    colors[ImGuiCol_ButtonActive] = Active(colors[ImGuiCol_Button]);
    colors[ImGuiCol_Header] = GetColor(MainAccentColor, Alpha80);
    colors[ImGuiCol_HeaderHovered] = Hovered(colors[ImGuiCol_Header]);
    colors[ImGuiCol_HeaderActive] = Active(colors[ImGuiCol_Header]);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = Hovered(colors[ImGuiCol_Separator]);
    colors[ImGuiCol_SeparatorActive] = Active(colors[ImGuiCol_Separator]);
    colors[ImGuiCol_ResizeGrip] = GetColor(MainColor, Alpha20);
    colors[ImGuiCol_ResizeGripHovered] = Hovered(colors[ImGuiCol_ResizeGrip]);
    colors[ImGuiCol_ResizeGripActive] = Active(colors[ImGuiCol_ResizeGrip]);
    colors[ImGuiCol_Tab] = GetColor(MainColor, Alpha60);
    colors[ImGuiCol_TabHovered] = Hovered(colors[ImGuiCol_Tab]);
    colors[ImGuiCol_TabActive] = Active(colors[ImGuiCol_Tab]);
    colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
    colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
    colors[ImGuiCol_DockingPreview] = Darken(colors[ImGuiCol_HeaderActive], 0.2f);
    colors[ImGuiCol_DockingEmptyBg] = Darken(colors[ImGuiCol_HeaderActive], 0.6f);
    colors[ImGuiCol_PlotLines] = GetColor(HighlightColor);
    colors[ImGuiCol_PlotLinesHovered] = Hovered(colors[ImGuiCol_PlotLines]);
    colors[ImGuiCol_PlotHistogram] = GetColor(HighlightColor);
    colors[ImGuiCol_PlotHistogramHovered] = Hovered(colors[ImGuiCol_PlotHistogram]);
    colors[ImGuiCol_TextSelectedBg] = GetColor(HighlightColor, Alpha40);
    colors[ImGuiCol_DragDropTarget] = GetColor(HighlightColor, Alpha80);;
    colors[ImGuiCol_NavHighlight] = GetColor(White);
    colors[ImGuiCol_NavWindowingHighlight] = GetColor(White, Alpha80);
    colors[ImGuiCol_NavWindowingDimBg] = GetColor(White, Alpha20);
    colors[ImGuiCol_ModalWindowDimBg] = GetColor(Black, Alpha60);

    ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_Right;
}