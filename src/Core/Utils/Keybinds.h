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
		KeybindEvent_Exit,
		KeybindEvent_RecompileNodes,
		KeybindEvent_COUNT
	};

	struct KeyEvent {
		bool alt;
		bool shift;
		bool ctrl;
		bool system;
		sf::Keyboard::Key code;

		KeyEvent(){}
		KeyEvent(bool alt_, bool shift_, bool ctrl_, bool system_, sf::Keyboard::Key code_) { alt = alt_; shift = shift_; ctrl = ctrl_; system = system_, code = code_; };
		KeyEvent(sf::Event::KeyEvent event) { alt = event.alt, shift = event.shift, ctrl = event.control, system = event.system; code = event.code; };

		bool operator==(const KeyEvent& other) const
		{
			return (alt == other.alt
				&& shift == other.shift
				&& ctrl == other.ctrl
				&& system == other.system
				&& code == other.code);
		}
	};

	// The specialized hash function for `unordered_map` keys
	struct hash_fn
	{
		std::size_t operator() (const KeyEvent& e) const
		{
			std::size_t h1 = std::hash<bool>()(e.alt);
			std::size_t h2 = std::hash<bool>()(e.shift);
			std::size_t h3 = std::hash<bool>()(e.ctrl);
			std::size_t h4 = std::hash<bool>()(e.system);
			std::size_t h5 = std::hash<int>()(e.code);

			return h1 ^ h2 ^ h3 ^ h4 ^ h5;
		}
	};

	static inline std::unordered_map<KeyEvent, KeybindEvent, hash_fn> KeybindMap = {
		{KeyEvent(false,false,false,false,sf::Keyboard::Key::F11), KeybindEvent_FullscreenToggle},
		{KeyEvent(false,false,true,false,sf::Keyboard::Key::Q), KeybindEvent_Exit},
		{KeyEvent(false,false,true,false,sf::Keyboard::Key::R), KeybindEvent_RecompileNodes}
	};

	static KeybindEvent ProcessEvent(sf::Event& event);
};