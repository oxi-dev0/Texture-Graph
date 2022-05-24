#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace Utility {
	class Mapping
	{
	public:
		static sf::Vector2f pixelToWindowLoc(sf::Vector2i pixel, sf::RenderWindow& target);
	};
}