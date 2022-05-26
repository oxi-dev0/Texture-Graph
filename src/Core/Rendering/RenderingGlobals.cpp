#include "RenderingGlobals.h"

namespace RenderingGlobals {
	sf::Font& font = *(new sf::Font());
	void Init() {
		font = sf::Font();
		assert(font.loadFromFile("resources/NotoSans/NotoSans-Regular.ttf"));
	}
}