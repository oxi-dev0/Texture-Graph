#include "ImGuiExtra.h"

namespace Utility {
	namespace ImGuiExtra {
		CollapsableSelectorData ThickSelectable(const char* label, const char* id, CollapsableSelectorData data)
		{
			ImGui::PushID(id);
			ImGui::BeginGroup();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0, 0.5));
			float height = 40.0f;
			float width = ImGui::GetContentRegionAvail().x;
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, ImVec2(0, 0));
			bool selectableHovered = data.hovered;
			float selectIndicatorWidth = 4.0f;

			bool oldSelected = data.selected;
			if (ImGui::Selectable(label, data.selected, 0, ImVec2(width - (data.selected ? selectIndicatorWidth : 0), height), true, &selectableHovered, true)) {
				data.selected = !data.selected;
			}
			ImGui::SameLine();
			if (oldSelected) {
				// select bar
				ImGui::PushID("Indicator"); 
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.968f, 0.619f, 0.749f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.968f, 0.619f, 0.749f, 1.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
				ImGui::Button("", ImVec2(selectIndicatorWidth, height));
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor(2);
				ImGui::PopID();
			}

			data.hovered = selectableHovered;

			ImGui::EndGroup();

			// seperator
			ImGui::PushID("Seperator");
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2);
			ImGui::Button("", ImVec2(width, 2));
			ImGui::PopItemFlag();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(2);
			ImGui::PopID();

			ImGui::PopStyleVar(6);
			ImGui::PopID();

			return data;
		}

		CollapsableSelectorData CollapsingSelectable(const char* label, const char* id, CollapsableSelectorData data) {
			ImGui::PushID(id);
			ImGui::BeginGroup();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0, 0.5));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
			float fullWidth = ImGui::GetContentRegionAvail().x;
			bool arrowButtonHovered = data.hovered;
			if (ImGui::ArrowButton("dropButton", data.open ? ImGuiDir_Down : ImGuiDir_Right, &arrowButtonHovered, &data.selected, true)) {
				data.open = !data.open;
			}
			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();
			float height = max.y - min.y;
			ImGui::PopStyleVar();
			ImGui::SameLine();
			float width = ImGui::GetContentRegionAvail().x;
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, ImVec2(0, 0));
			bool selectableHovered = data.hovered;
			float selectIndicatorWidth = 4.0f;

			bool oldSelected = data.selected;
			if (ImGui::Selectable(label, data.selected, 0, ImVec2(width-(data.selected ? selectIndicatorWidth : 0), height), true, &selectableHovered, true)) {
				data.selected = !data.selected;
			}
			if (oldSelected) {
				ImGui::SameLine();
				// select bar
				ImGui::PushID("Indicator");
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.968f, 0.619f, 0.749f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.968f, 0.619f, 0.749f, 1.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::Button("", ImVec2(selectIndicatorWidth, height));
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor(2);
				ImGui::PopID();
			}

			ImGui::EndGroup();

			// seperator
			ImGui::PushID("Seperator");
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2);
			ImGui::Button("", ImVec2(fullWidth, 2));
			ImGui::PopItemFlag();
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(2);
			ImGui::PopID();

			data.hovered = arrowButtonHovered || selectableHovered;

			ImGui::PopStyleVar(6);
			ImGui::PopID();

			return data;
		}
	}
}
