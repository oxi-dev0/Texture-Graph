#include "CachedImages.h"

namespace ImageCache
{
	std::unordered_map<std::string, sf::Texture*> images;

	std::unordered_map<std::string, std::string> imageDefs({
		{"transparencyPreview", "resources/Images/transparency.png"}
		});

	void Init() {
		for (auto& def : imageDefs) {
			images.insert({ def.first, new sf::Texture() });
			if (!images[def.first]->loadFromFile(def.second)) { LOG_CRITICAL("Could not load image '{0}' at '{1}'", def.first, def.second); };
		}
	}

	void Exit() {
		for (auto& image : images) {
			delete images[image.first]; 
		}
	}
}