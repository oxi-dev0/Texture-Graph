#include "Keybinds.h"

Keybinds::KeybindEvent Keybinds::ProcessEvent(sf::Event& event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		auto key = event.key;
		if (KeybindMap.find(key.code) == KeybindMap.end()) {
			return KeybindEvent_None;
		}
		return KeybindMap[key.code];
	}
	return KeybindEvent_None;
}