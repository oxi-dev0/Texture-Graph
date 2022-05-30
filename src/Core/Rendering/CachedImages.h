#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "../Utils/Log.h"

#include <string>
#include <unordered_map>

namespace ImageCache
{
	extern std::unordered_map<std::string, sf::Texture*> images;

	extern std::unordered_map<std::string, std::string> imageDefs;

	extern void Init();
	extern void Exit();
}
