#include "ImageNode.h"

void ImageNode::Execute(std::atomic<int>* threadCount)
{
	if (threadCount != nullptr) {
		// Limit to only 15 nodes executing at one time
		while (*threadCount > 15) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
		(*threadCount)++;
	}

	Utility::Timer fullTimr;

	sf::Image loaded;
	if (std::find(Bundle::Resources::resourceList.begin(), Bundle::Resources::resourceList.end(), resourceName) == Bundle::Resources::resourceList.end()) {
		LOG_ERROR("Invalid resource name {0}", resourceName);
		return;
	}
	if (!loaded.loadFromFile(resourceName)) {
		LOG_ERROR("Could not load the image file '{0}'", resourceName);
		return;
	}

	float scaleFactor = (float)texSize.x / (float)loaded.getSize().x;
	float offsetY = (texSize.y - ((float)loaded.getSize().y*scaleFactor))/2.f;
		
	// Fill var with loaded image (currently just crops)
	for (int x = 0; x < luaVarData["Out"].colortexVar.size(); x++) {
		for (int y = 0; y < luaVarData["Out"].colortexVar[0].size(); y++) {
			int xS = std::floor(x / scaleFactor);
			int yS = std::floor((y-offsetY) / scaleFactor);
			if (xS >= loaded.getSize().x || yS >= loaded.getSize().y) { luaVarData["Out"].colortexVar[x][y] = sf::Color::Black; continue; }
			luaVarData["Out"].colortexVar[x][y] = loaded.getPixel(xS, yS);
		}
	}

	// Display tex
	Types::colortex displayData = luaVarData[displayVar].colortexVar;
	Types::greytex displayDataGrey = luaVarData[displayVar].greytexVar;
	auto dataType = luaVarData[displayVar].dataType;

	for (unsigned int x = 0; x < displayTexture->getSize().x; x++) {
		for (unsigned int y = 0; y < displayTexture->getSize().y; y++) {
			if (dataType == Types::DataType_ColorTex) {
				displayImage->setPixel(x, y, displayData[x][y]);
			}
			else {
				displayImage->setPixel(x, y, sf::Color(displayDataGrey[x][y], displayDataGrey[x][y], displayDataGrey[x][y], 255));
			}
		}
	}
	displayTexture->update(*displayImage);

	prevEvalTime = (float)fullTimr.Elapsed();
	evaluated = true;

	LOG_TRACE("Processed image node in {0}s", fullTimr.Elapsed());

	if (threadCount != nullptr)
		(*threadCount)--;
}

ImageNode::ImageNode(const ImageNode& node) : ImageNode()
{
	nodeClass = node.nodeClass;
	nodeId = node.nodeId;
	displayName = node.displayName;
	category = node.category;
	displayColor = node.displayColor;
	pins = node.pins;
	luaVarDisplayNames = node.luaVarDisplayNames;
	luaVars = node.luaVars;
	pinLuaVars = node.pinLuaVars;
	paramLuaVars = node.paramLuaVars;
	luaVarData = node.luaVarData;
	luaVarPins = node.luaVarPins;
	pinPosCache = std::map<int, sf::Vector2f>();
	displayVar = node.displayVar;
	luaLines = node.luaLines;
	nodePos = node.nodePos;
	luaTempFile = node.luaTempFile;
	prevEvalTime = 0.0f;
	texSize = sf::Vector2i(512, 512);
	resourceName = node.resourceName;
	SetTextureSize(texSize);
}

ImageNode::ImageNode(GraphNode* node) {
	ImageNode* other = dynamic_cast<ImageNode*>(node);
	nodeClass = other->nodeClass;
	nodeId = other->nodeId;
	displayName = other->displayName;
	category = other->category;
	displayColor = other->displayColor;
	pins = other->pins;
	luaVarDisplayNames = other->luaVarDisplayNames;
	luaVars = other->luaVars;
	pinLuaVars = other->pinLuaVars;
	paramLuaVars = other->paramLuaVars;
	luaVarData = other->luaVarData;
	luaVarPins = other->luaVarPins;
	pinPosCache = std::map<int, sf::Vector2f>();
	displayVar = other->displayVar;
	luaLines = other->luaLines;
	nodePos = other->nodePos;
	luaTempFile = other->luaTempFile;
	prevEvalTime = 0.0f;
	texSize = sf::Vector2i(512, 512);
	resourceName = other->resourceName;
	SetTextureSize(texSize);
}