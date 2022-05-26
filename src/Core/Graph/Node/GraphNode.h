#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <string>
#include <map>
#include <fstream>

#include "Pin/NodePin.h"

#include "../../Data/Types.h"
#include "../../Utils/Strings.h"
#include "../../Utils/Colors.h"
#include "../../Utils/Log.h"

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

	// Actual pin definition
	std::vector<NodePin> pins;

	// pin id, pin index (in pins vector)
	std::map<int, int> inPins;
	std::map<int, int> outPins;

	// lua var name, display name
	std::map<std::string, std::string> luaVarDisplayNames;
	// lua var
	std::vector<LuaVar> paramLuaVars;
	// pin id, lua var
	std::map<int, LuaVar> pinLuaVars;
	// lua var name, value
	std::map<std::string, Types::WildData> luaVarData;
	// lua var name
	std::string displayVar;


	std::vector<std::string> luaLines;

public:
	GraphNode() {
		nodeClass = "";
		nodeId = 0;
		displayName = "";
		displayColor = sf::Color(0, 0, 0, 255);
		pins = std::vector<NodePin>();
		inPins = std::map<int, int>();
		outPins = std::map<int, int>();
		luaVarDisplayNames = std::map<std::string, std::string>();
		paramLuaVars = std::vector<LuaVar>();
		pinLuaVars = std::map<int, LuaVar>();
		luaVarData = std::map<std::string, Types::WildData>();
		displayVar = "";
		luaLines = std::vector<std::string>();
	}

	static GraphNode LoadFromTGNF(std::string classFile); // Load from Texture Graph Node File
};

