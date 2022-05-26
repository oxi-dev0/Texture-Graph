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

	std::vector<NodePin> pins; // Pin Definitions

	std::map<int, int> inPins; // Pin Id -> Pin Index
	std::map<int, int> outPins;

	std::map<std::string, std::string> luaVarDisplayNames; // Lua Var Name -> Display Name
	std::map<std::string, LuaVar> luaVars; // Lua Var Name -> Lua Var
	std::map<int, int> pinLuaVars; // Pin Id -> Lua Var Index
	std::map<std::string, int> paramLuaVars; // Param Name -> Lua Var Index
	std::map<std::string, Types::WildData> luaVarData; // Lua Var Name -> Data
	std::string displayVar; // Lua Var Name

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
		luaVars = std::map<std::string, LuaVar>();
		pinLuaVars = std::map<int, int>();
		paramLuaVars = std::map<std::string, int>();
		luaVarData = std::map<std::string, Types::WildData>();
		displayVar = "";
		luaLines = std::vector<std::string>();
	}

	static GraphNode LoadFromTGNF(std::string classFile); // Load from Texture Graph Node File
};

