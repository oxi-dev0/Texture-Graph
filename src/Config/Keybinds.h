#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

class Keybinds
{
public:
	enum KeybindEvent {
		KeybindEvent_None,
		KeybindEvent_FullscreenToggle,
		KeybindEvent_COUNT
	};

	static inline std::unordered_map<sf::Keyboard::Key, KeybindEvent> KeybindMap = {
		{sf::Keyboard::F11, KeybindEvent_FullscreenToggle}
	};

	static KeybindEvent ProcessEvent(sf::Event& event);
};