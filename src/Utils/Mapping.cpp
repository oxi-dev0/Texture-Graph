#include "Mapping.h"

namespace Utility {
	sf::Vector2f Mapping::pixelToWindowLoc(sf::Vector2i pixel, sf::RenderWindow& target) {
		sf::Vector2u size = target.getSize();
		sf::Vector2i pos = target.getPosition();

		int titleBarHeight = 45;

		float x = (pixel.x - pos.x) / (float)size.x;
		float y = (pixel.y - titleBarHeight - pos.y) / (float)size.y;

		x = std::clamp(x, 0.0f, 1.0f);
		y = std::clamp(y, 0.0f, 1.0f);

		return sf::Vector2f(x*size.x, y*size.y);
	}
}