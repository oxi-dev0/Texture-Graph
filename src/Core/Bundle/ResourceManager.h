#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "../Utils/Log.h"

#include <string>
#include <sstream>
#include <unordered_map>
#include <nfd.h>

namespace Bundle
{
	namespace Resources
	{
		extern std::vector<std::string> resourceList; // list of resource paths
		extern std::unordered_map<int, sf::Texture*> resourcePreviews;

		void Init();
		void Clear();

		void GeneratePreviews();
		void LoadFromBundle();

		void ClearPreviews();

		std::string ImportResourceFromAsk();

		std::string ResourceSelector(std::string currentResource = "");
	}
}