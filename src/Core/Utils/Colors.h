#pragma once

#include "../Types.h"
#include <string>

namespace Utility {
	namespace Color {
		sf::Color ColorFromHex(std::string hexString); // Convert to 255 RGBA from '0x......'
	}
}

