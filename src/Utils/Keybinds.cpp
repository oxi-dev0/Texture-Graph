#include "Keybinds.h"

Keybinds::KeybindEvent Keybinds::ProcessEvent(sf::Event& event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		auto key = event.key;
		if (KeybindMap.find(Keybinds::KeyEvent(key)) == KeybindMap.end()) {
			return KeybindEvent_None;
		}
		return KeybindMap[Keybinds::KeyEvent(key)];
	}
	return KeybindEvent_None;
}