#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include <imgui_internal.h>

namespace Utility {
	namespace ImGuiExtra {
		struct CollapsableSelectorData
		{
			bool open;
			bool selected;
			bool hovered;

			CollapsableSelectorData() {
				open = false;
				selected = false;
				hovered = false;
			}
		};

		CollapsableSelectorData CollapsingSelectable(const char* label, const char* id, CollapsableSelectorData data);
		CollapsableSelectorData ThickSelectable(const char* label, const char* id, CollapsableSelectorData data);
	}
}

