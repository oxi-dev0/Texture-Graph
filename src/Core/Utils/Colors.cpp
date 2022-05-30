#include "Colors.h"

namespace Utility {
	namespace Color {
		sf::Color ColorFromHex(std::string hexString) {
			int r, g, b, a;
			int discard = sscanf_s(hexString.c_str(), "0x%02x%02x%02x%02x", &r, &g, &b, &a);
			return sf::Color(r, g, b, a);
		}
	}
}
