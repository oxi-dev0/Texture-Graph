#pragma once

#include "GraphNode.h"
#include "../ResourceManager.h"

class ImageNode : public GraphNode
{
public:
	std::string resourceName;

public:
	ImageNode() : GraphNode()
	{
		displayName = "Image";
		category = "Core/Constants";
		nodeClass = "CUSTOM_IMAGENODE";
		displayColor = Utility::Color::ColorFromHex("8AE9C1FF");

		luaVarDisplayNames = { {"Out", "Out"} };
		luaVars = { {"Out", LuaVar("Out", Types::DataType_ColorTex)} };
		pinLuaVars = { {0, "Out"} };
		luaVarPins = { {"Out", 0} };
		luaVarData = { {"Out", Types::WildData(Types::DataType_ColorTex)} };

		displayVar = "Out";

		resourceName = "";

		NodePin newPin(0, &nodeId, Types::DataType_ColorTex, Direction::Out, "Out");
		pins.push_back(newPin);
	}

	ImageNode(const ImageNode& node);

	//Conversion function
	ImageNode(GraphNode* node);

	void Execute(std::atomic<int>* threadCount) override;
};

