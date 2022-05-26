#include "GraphNode.h"

#define TGNL_LINE_ASSERT(keyword, expected, given, file)	if (expected != given) { LOG_CRITICAL("Too many parameters passed to keyword '{3}'. {1} expected but {2} were given. (Node Class '{0}')", file, expected, given, keyword); }

#define TGNL_PARAM_ASSERT(type, file)						if (Utility::String::includes({"tex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type 'tex' is not allowed to be used with the keyword 'param'. (Node Class '{0}')", file); }
#define TGNL_PIN_ASSERT(keyword, type, file)				if (Utility::String::includes({"string"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type 'string' is not allowed to be used with the keyword '{1}'. (Node Class '{0}')", file, keyword); }
#define TGNL_DISPLAY_ASSERT(type, file)						if (!Utility::String::includes({"tex", "int", "float", "color", "bool"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type '{0}' cannot be used with keyword 'display'. (Node Class {1})", type, file); }

#define TGNL_TYPE_ASSERT(keyword, type, file)				if (!Utility::String::includes({"tex", "int", "float", "color", "bool", "string"}, Utility::String::toLower(type))) { LOG_CRITICAL("Unrecognised type '{0}' given to '{2}'. (Node Class '{1}')", type, file, keyword); }
#define TGNL_FLOAT_ASSERT(keyword, string, file)			if (!Utility::String::isFloat(string)) { LOG_CRITICAL("Invalid value passed to keyword '{0}'. Expected valid float, but got '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_INT_ASSERT(keyword, string, file)				if (!Utility::String::isInt(string)) { LOG_CRITICAL("Invalid value passed to keyword '{0}'. Expected valid int, but got '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_BOOL_ASSERT(keyword, bool, file)				if (!Utility::String::includes({"false", "true"}, Utility::String::toLower(bool))) { LOG_CRITICAL("Unrecognised bool '{0}' given to keyword '{2}'. (Node Class '{1}')", bool, file, keyword); }
#define TGNL_STRING_ASSERT(keyword, string, file)			if (string[0] != '"' || string[string.size()-1] != '"') { LOG_CRITICAL("Invalid string given to keyword '{0}'. Received '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_COLOR_ASSERT(keyword, colorString, file)		if (colorString.substr(0,2) != "0x") { LOG_CRITICAL("Invalid color given to keyword '{0}'. Received '{1}', which is missing the '0x' prefix. (Node Class '{2}')", keyword, colorString, file); } if (colorString.size() != 10) { LOG_CRITICAL("Invalid color given to keyword '{0}'. Received '{1}', which has an incorrect length. (Node Class '{2}')", keyword, colorString, file); } 

std::string RemoveStringIndicators(std::string orig) {
	return orig.substr(1, orig.size() - 2);
}

std::vector<std::string> SmartSplit(std::string s, char seperator, char stickChar) {
	std::vector<std::string> output;
	std::string temp;
	int pos = 0;
	bool stick = false;
	while (pos < s.size())
	{
		if (s[pos] == seperator && !stick) {
			output.push_back(temp);
			temp.clear();
			pos++;
			continue;
		}
		if (s[pos] == stickChar) {
			stick = !stick;
		}
		temp.push_back(s[pos]);
		pos++;
	}
	output.push_back(temp);

	return output;
}

GraphNode GraphNode::LoadFromTGNF(std::string classFile) {
	std::ifstream f(classFile);
	GraphNode newNode;

	newNode.nodeClass = classFile;

	bool metadata = false;
	bool parsedMetadata = false;
	bool parsedVariables = false;
	bool execdata = false;
	bool execprime = false;

	std::vector<std::string> luaLines;

	std::map<std::string, std::string> pendingDefaults;

	std::string line;
	while (std::getline(f, line)) {
		Utility::String::ltrim(line); // Trim Whitespace left
		Utility::String::rtrim(line); // Trim Whitespace right
		if (line[0] == '#') { continue; } // Ignore comment lines
		if (line == "") { continue; }	// Ignore empty

		auto data = SmartSplit(line, ' ', '"'); // Smart stick allows "stick" characters to toggle seperation, e.g, a string shouldnt be seperated

		auto keyword = data[0];
		if (keyword == "metadata") {
			metadata = !metadata;
			if (parsedMetadata) {
				LOG_CRITICAL("Multiple metadata blocks are not supported. (Node Class '{0}')", classFile);
			}
			if (!metadata) {
				parsedMetadata = true;
			}
			continue;
		}
		if (keyword == "exec") {
			execprime = true;
			continue;
		}

		if (metadata) {
			// METADATA PROCESSING
			if (keyword == "name") { TGNL_LINE_ASSERT("name", 1, data.size() - 1, classFile) TGNL_STRING_ASSERT("name", data[1], classFile) newNode.displayName = RemoveStringIndicators(data[1]); continue; }
			if (keyword == "color") { TGNL_LINE_ASSERT("color", 1, data.size() - 1, classFile) TGNL_COLOR_ASSERT("color", data[1], classFile) newNode.displayColor = Utility::Color::ColorFromHex(data[1]); continue; }
			if (keyword == "varname") { TGNL_LINE_ASSERT("varname", 2, data.size() - 1, classFile) TGNL_STRING_ASSERT("varname", data[2], classFile) newNode.luaVarDisplayNames.insert({data[1], RemoveStringIndicators(data[2])}); continue; }
			if (keyword == "display") { TGNL_LINE_ASSERT("color", 1, data.size() - 1, classFile) newNode.displayVar = data[1]; continue; }
			if (keyword == "default") { TGNL_LINE_ASSERT("default", 2, data.size() - 1, classFile) pendingDefaults.insert({ data[1], data[2] }); continue; } // defaults have to be offset until all vars are loaded

			if (keyword == "show") { /* IMPLEMENT */ }
			continue;
		}
		if (execprime) {
			// EXEC PROCESSING
			if (!execdata) {
				if (keyword == "{") {
					execdata = true;
					parsedVariables = true;
					continue;
				}
				else {
					LOG_CRITICAL("Missing exec brace on line after keyword 'exec'. (Node Class '{0}')", classFile);
				}
			}
			else {
				if (keyword == "}") {
					execdata = false;
					execprime = false;
					continue;
				}
				luaLines.push_back(line);
				continue;
			}
		}

		// DEF PROCESSING
		if (!parsedMetadata) {
			LOG_CRITICAL("Metadata definition block is required. (Node Class '{0}')", classFile);
		}
		if (parsedVariables) {
			LOG_CRITICAL("Execution definition block must be after variable definition block. (Node Class '{0}')", classFile);
		}
		if (keyword == "param") {
			TGNL_LINE_ASSERT("param", 2, data.size() - 1, classFile);
			TGNL_TYPE_ASSERT("param", data[1], classFile);
			TGNL_PARAM_ASSERT(data[1], classFile);
			bool exists = false;
			for (std::map<std::string, LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it)
			{
				if (it->first == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is being redefined. (Node Class '{1}')", data[2], classFile); }
			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			if (newNode.luaVarDisplayNames.find(data[2]) == newNode.luaVarDisplayNames.end()) {
				LOG_CRITICAL("Param variable '{0}' requires a varname defintion in the metadata block, before it's definition. (Node Class '{1}')", data[2], classFile);
				continue;
			}
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.paramLuaVars.insert({ newNode.luaVarDisplayNames.find(data[2])->first, newNode.luaVars.size()-1 });
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			continue;
		}
		if (keyword == "output") {
			TGNL_LINE_ASSERT("output", 2, data.size() - 1, classFile);
			TGNL_TYPE_ASSERT("output", data[1], classFile);
			TGNL_PIN_ASSERT("output", data[1], classFile);
			bool exists = false;
			for (std::map<std::string, LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it)
			{
				if (it->first == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is being redefined. (Node Class '{1}')", data[2], classFile); }
			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			if (newNode.luaVarDisplayNames.find(data[2]) == newNode.luaVarDisplayNames.end()) {
				LOG_CRITICAL("Output variable '{0}' requires a varname defintion in the metadata block, before it's definition. (Node Class '{1}')", data[2], classFile);
				continue;
			}
			NodePin newPin = NodePin(newNode.pins.size(), newNode.nodeId, dataType, Direction::Out, newNode.luaVarDisplayNames[data[2]]);
			newNode.pins.push_back(newPin);
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.pinLuaVars.insert({ newPin.pinId, newNode.luaVars.size() - 1 });
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			newNode.outPins.insert({ newPin.pinId, newNode.pins.size() -1});
			continue;
		}
		if (keyword == "input") {
			TGNL_LINE_ASSERT("input", 2, data.size() - 1, classFile);
			TGNL_TYPE_ASSERT("input", data[1], classFile);
			TGNL_PIN_ASSERT("input", data[1], classFile);
			bool exists = false;
			for (std::map<std::string, LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it)
			{
				if (it->first == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is being redefined. (Node Class '{1}')", data[2], classFile); }
			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			if (newNode.luaVarDisplayNames.find(data[2]) == newNode.luaVarDisplayNames.end()) {
				LOG_CRITICAL("Input variable '{0}' requires a varname defintion in the metadata block, before it's definition. (Node Class '{1}')", data[2], classFile);
				continue;
			}
			NodePin newPin = NodePin(newNode.pins.size(), newNode.nodeId, dataType, Direction::In, newNode.luaVarDisplayNames[data[2]]);
			newNode.pins.push_back(newPin);
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.pinLuaVars.insert({ newPin.pinId, newNode.luaVars.size()-1 });
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			newNode.inPins.insert({ newPin.pinId, newNode.pins.size() - 1 });
			continue;
		} 
	}

	for (std::map<std::string, std::string>::iterator it = pendingDefaults.begin(); it != pendingDefaults.end(); ++it) {
		auto var = it->first;
		auto data = it->second;

		if (newNode.luaVarData.find(var) == newNode.luaVarData.end()) {
			LOG_CRITICAL("Variable '{0}' given to keyword 'default' is not defined. (Node Class '{1}')", var, classFile);
		}
		 
		auto expectedType = newNode.luaVarData[var].dataType;
		switch (expectedType) {
		case Types::DataType::DataType_Bool:
			TGNL_BOOL_ASSERT("default", data, classFile);
			newNode.luaVarData[var].boolVar = Utility::String::toLower(data) == "true";
			break;
		case Types::DataType::DataType_Color:
			TGNL_COLOR_ASSERT("default", data, classFile);
			newNode.luaVarData[var].colorVar = Utility::Color::ColorFromHex(data);
			break;
		case Types::DataType::DataType_Float:
			TGNL_FLOAT_ASSERT("default", data, classFile);
			newNode.luaVarData[var].floatVar = std::stof(data);
			break;
		case Types::DataType::DataType_Int:
			TGNL_INT_ASSERT("default", data, classFile);
			newNode.luaVarData[var].intVar = std::stoi(data);
			break;
		case Types::DataType_String:
			TGNL_STRING_ASSERT("default", data, classFile);
			newNode.luaVarData[var].stringVar = data;
			break;
		case Types::DataType::DataType_Tex:
			LOG_CRITICAL("Type 'tex' is not compatible with keyword 'default'. (Node Class '{0}')", classFile);
			break;
		}
	}

	if (newNode.displayVar != "") {
		if (newNode.luaVars.find(newNode.displayVar) != newNode.luaVars.end()) {
			TGNL_DISPLAY_ASSERT(Types::typeToString[newNode.luaVars.find(newNode.displayVar)->second.type], classFile);
		}
		else {
			LOG_CRITICAL("Variable '{0}' given to keyword 'display' is not defined. (Node Class '{1}')", newNode.displayVar, classFile)
		}
	}

	for (std::map<std::string, std::string>::iterator it = newNode.luaVarDisplayNames.begin(); it != newNode.luaVarDisplayNames.end(); ++it) {
		if (newNode.luaVars.find(it->first) == newNode.luaVars.end()) {
			LOG_CRITICAL("Variable '{0}' given to keyword 'varname' is not defined. (Node Class '{1}')", it->first, classFile);
		}
	}
	 
	newNode.luaLines = luaLines; 
	if (newNode.luaLines.size() == 0 || newNode.luaLines[0] == "") {
		LOG_CRITICAL("Missing execution lua. (Node Class '{0}')", classFile);
	}

	return newNode;
}