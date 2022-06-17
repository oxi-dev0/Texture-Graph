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
	if (Graph::Resources::resourceList.size() <= resourceIndex) {
		LOG_ERROR("Invalid resource index {0}", resourceIndex);
		return;
	}
	if (!loaded.loadFromFile(Graph::Resources::resourceList[resourceIndex])) {
		LOG_ERROR("Could not load the image file '{0}'", Graph::Resources::resourceList[resourceIndex]);
		return;
	}

	// Fill var with loaded image (currently just crops)
	for (int x = 0; x < luaVarData["Out"].colortexVar.size(); x++) {
		for (int y = 0; y < luaVarData["Out"].colortexVar[0].size(); y++) {
			if (x >= loaded.getSize().x || y >= loaded.getSize().y) { luaVarData["Out"].colortexVar[x][y] = sf::Color::Black; continue; }
			luaVarData["Out"].colortexVar[x][y] = loaded.getPixel(x, y);
		}
	}

	// Display tex
	Types::colortex displayData = luaVarData[displayVar].colortexVar;
	Types::greytex displayDataGrey = luaVarData[displayVar].greytexVar;
	auto dataType = luaVarData[displayVar].dataType;

	for (unsigned int x = 0; x < displayTexture.getSize().x; x++) {
		for (unsigned int y = 0; y < displayTexture.getSize().y; y++) {
			if (dataType == Types::DataType_ColorTex) {
				displayImage.setPixel(x, y, displayData[x][y]);
			}
			else {
				displayImage.setPixel(x, y, sf::Color(displayDataGrey[x][y], displayDataGrey[x][y], displayDataGrey[x][y], 255));
			}
		}
	}
	displayTexture.update(displayImage);

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
	resourceIndex = node.resourceIndex;
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
	resourceIndex = other->resourceIndex;
	SetTextureSize(texSize);
}