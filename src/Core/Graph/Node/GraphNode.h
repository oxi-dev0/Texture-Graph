#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <string>
#include <map>
#include <fstream>
#include <iostream>

#include "Pin/NodePin.h"

#include "../../Data/Types.h"

#include "../../Utils/Strings.h"
#include "../../Utils/Colors.h"
#include "../../Utils/Log.h"
#include "../../Utils/Timer.h"

#include "../../Rendering/RenderingGlobals.h"
#include "../../Rendering/CachedImages.h"

#include "../../Lua/LuaManager.h"

class GraphNode
{
public:
	struct LuaVar {
		std::string name;
		Types::DataType type;

		LuaVar() { name = ""; type = Types::DataType::DataType_Bool; }
		LuaVar(std::string name_, Types::DataType type_) { name = name_; type = type_; }
	};

	std::string nodeClass;
	int nodeId;

	std::string displayName;
	sf::Color displayColor;
	std::string category;

	std::vector<NodePin> pins; // Pin Definitions

	std::map<std::string, std::string> luaVarDisplayNames; // Lua Var Name -> Display Name
	std::map<std::string, LuaVar> luaVars; // Lua Var Name -> Lua Var
	std::map<int, std::string> pinLuaVars; // Pin Index -> Lua Var Name
	std::map<std::string, int> luaVarPins; // Lua Var Name -> Pin Index
	std::map<std::string, std::string> paramLuaVars; // Param Name -> Lua Var Name
	std::map<std::string, Types::WildData> luaVarData; // Lua Var Name -> Data
	std::map<int, sf::Vector2f> pinPosCache; // Pin Index -> Location

	std::string displayVar; // Lua Var Name

	std::vector<std::string> luaLines;
	std::string luaTempFile;

	sf::Texture& displayTexture;
	sf::Image& displayImage;
	float prevEvalTime;

	int debugEvalIndex;

	sf::Vector2f nodePos;
	sf::Vector2i texSize;

	bool evaluated;

public:
	GraphNode() : displayTexture(*(new sf::Texture)), displayImage(*(new sf::Image)) {
		nodeClass = "";
		nodeId = 0;
		displayName = "";
		category = "";
		displayColor = sf::Color(0, 0, 0, 255);
		pins = std::vector<NodePin>();
		luaVarDisplayNames = std::map<std::string, std::string>();
		luaVars = std::map<std::string, LuaVar>();
		pinLuaVars = std::map<int, std::string>();
		paramLuaVars = std::map<std::string, std::string>();
		luaVarData = std::map<std::string, Types::WildData>();
		luaVarPins = std::map<std::string, int>();
		pinPosCache = std::map<int, sf::Vector2f>();
		displayVar = "";
		luaLines = std::vector<std::string>();
		nodePos = sf::Vector2f(0, 0);
		prevEvalTime = 0.0f;
		texSize = sf::Vector2i(512, 512);
		debugEvalIndex = -1;
		evaluated = false;
	}
	GraphNode(const GraphNode& node);
	~GraphNode() {
		displayImage.~Image();
		displayTexture.~Texture();
	}

	static GraphNode LoadFromTGNF(std::string classFile); // Load from Texture Graph Node File

	void SFMLRender(sf::RenderTarget& target, float zoomLevel = 0.5f, bool selected=false, int transparency=255);
	void Execute();

	void SetDirty();

	void SetTextureSize(sf::Vector2i size);

	sf::FloatRect calcBounds(); // Calculate bounds in graph space
};

