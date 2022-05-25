#include "GraphNode.h"

#define TGNF_PARAM_ASSERT(keyword, expected, given, file)	if (expected != given) { LOG_CRITICAL("Too many parameters passed to keyword '{3}' in '{0}'. {1} expected but {2} were given.", file, expected, given, keyword); std::exit(134); }
#define TGNF_COLOR_ASSERT(colorString, file)				if (colorString.substr(0,2) != "0x") { LOG_CRITICAL("Invalid color given in '{0}'. Received '{1}'. (missing '0x')", file, colorString); std::exit(134); } if (colorString.size() != 10) { LOG_CRITICAL("Invalid color given in '{0}'. Received '{1}'. (incorrect length)", file, colorString); std::exit(134); } 
#define TGNF_STRING_ASSERT(string, file)					if (string[0] != '"' || string[string.size()-1] != '"') { LOG_CRITICAL("Invalid string given in {0}. Received '{1}'. (missing starting or ending '\"')", file, string); std::exit(134); }
#define TGNF_TYPE_ASSERT(keyword, type, file)				if (!Utility::String::includes({"tex", "int", "float", "color", "bool"}, Utility::String::toLower(type))) { LOG_CRITICAL("Unrecognised type '{0}' given to '{2}' in '{1}'.)", type, file, keyword); std::exit(134); }
#define TGNF_BOOL_ASSERT(keyword, bool, file)				if (!Utility::String::includes({"false", "true"}, Utility::String::toLower(bool))) { LOG_CRITICAL("Unrecognised bool '{0}' given to '{2}' in '{1}'.)", bool, file, keyword); std::exit(134); }

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
			continue;
		}
		if (keyword == "exec") {
			execprime = true;
			continue;
		}

		if (metadata) {
			// METADATA PROCESSING
			if (keyword == "name") { TGNF_PARAM_ASSERT("name", 1, data.size() - 1, classFile) TGNF_STRING_ASSERT(data[1], classFile) newNode.displayName = RemoveStringIndicators(data[1]); continue; }
			if (keyword == "color") { TGNF_PARAM_ASSERT("color", 1, data.size() - 1, classFile) TGNF_COLOR_ASSERT(data[1], classFile) newNode.displayColor = Utility::Color::ColorFromHex(data[1]); continue; }
			if (keyword == "varname") { TGNF_PARAM_ASSERT("varname", 2, data.size() - 1, classFile) TGNF_STRING_ASSERT(data[2], classFile) newNode.luaVarDisplayNames.insert({ data[1], RemoveStringIndicators(data[2]) }); continue; }
			if (keyword == "display") { TGNF_PARAM_ASSERT("color", 1, data.size() - 1, classFile) newNode.displayVar = data[1]; continue; }
			if (keyword == "default") { TGNF_PARAM_ASSERT("default", 2, data.size() - 1, classFile) pendingDefaults.insert({ data[1], data[2] }); continue; } // defaults have to be offset until all vars are loaded

			if (keyword == "show") { /* IMPLEMENT */ }
			continue;
		}
		if (execprime) {
			// EXEC PROCESSING
			if (!execdata) {
				if (keyword == "{") {
					execdata = true;
					continue;
				}
				else {
					LOG_CRITICAL("Missing exec brace on line after 'exec' in '{0}'", classFile); std::exit(134);
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
		if (keyword == "param") {
			TGNF_PARAM_ASSERT("param", 2, data.size() - 1, classFile);
			TGNF_TYPE_ASSERT("param", data[1], classFile);
			bool exists = false;
			for (auto var : newNode.paramLuaVars)
			{
				if (var.name == data[2]) {
					exists = true;
				}
			}
			for (std::map<int, LuaVar>::iterator it = newNode.pinLuaVars.begin(); it != newNode.pinLuaVars.end(); ++it) {
				if (it->second.name == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is being redefined.", data[2]); std::exit(134); }
			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			if (newNode.luaVarDisplayNames.find(data[2]) == newNode.luaVarDisplayNames.end()) {
				LOG_CRITICAL("Param variable '{0}' requires a varname defintion in the metadata block, before it's definition. (File '{1}')", data[2], classFile); std::exit(134);
				continue;
			}
			newNode.paramLuaVars.push_back(LuaVar(data[2], dataType));
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			continue;
		}
		if (keyword == "output") {
			TGNF_PARAM_ASSERT("output", 2, data.size() - 1, classFile);
			TGNF_TYPE_ASSERT("output", data[1], classFile);
			bool exists = false;
			for (auto var : newNode.paramLuaVars)
			{
				if (var.name == data[2]) {
					exists = true;
				}
			}
			for (std::map<int, LuaVar>::iterator it = newNode.pinLuaVars.begin(); it != newNode.pinLuaVars.end(); ++it) {
				if (it->second.name == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is being redefined.", data[2]); std::exit(134); }
			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			if (newNode.luaVarDisplayNames.find(data[2]) == newNode.luaVarDisplayNames.end()) {
				LOG_CRITICAL("Output variable '{0}' requires a varname defintion in the metadata block, before it's definition. (File '{1}')", data[2], classFile); std::exit(134);
				continue;
			}
			NodePin newPin = NodePin(newNode.pins.size(), newNode.nodeId, dataType, Direction::Out, newNode.luaVarDisplayNames[data[2]]);
			newNode.pins.push_back(newPin);
			newNode.pinLuaVars.insert({ newPin.pinId, LuaVar(data[2], dataType) });
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			newNode.outPins.insert({ newPin.pinId, newNode.pins.size() -1});
			continue;
		}
		if (keyword == "input") {
			TGNF_PARAM_ASSERT("input", 2, data.size() - 1, classFile);
			TGNF_TYPE_ASSERT("input", data[1], classFile);
			bool exists = false;
			for (auto var : newNode.paramLuaVars) 
			{
				if (var.name == data[2]) {
					exists = true;
				}
			}
			for (std::map<int, LuaVar>::iterator it = newNode.pinLuaVars.begin(); it != newNode.pinLuaVars.end(); ++it) {
				if (it->second.name == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is being redefined.", data[2]); std::exit(134); }
			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			if (newNode.luaVarDisplayNames.find(data[2]) == newNode.luaVarDisplayNames.end()) {
				LOG_CRITICAL("Input variable '{0}' requires a varname defintion in the metadata block, before it's definition. (File '{1}')", data[2], classFile); std::exit(134);
				continue;
			}
			NodePin newPin = NodePin(newNode.pins.size(), newNode.nodeId, dataType, Direction::In, newNode.luaVarDisplayNames[data[2]]);
			newNode.pins.push_back(newPin);
			newNode.pinLuaVars.insert({ newPin.pinId, LuaVar(data[2], dataType) });
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
		 
		auto expectedType = newNode.luaVarData[var].dataType;
		switch (expectedType) {
		case Types::DataType::DataType_Bool:
			TGNF_BOOL_ASSERT("default", data, classFile);
			newNode.luaVarData[var].boolVar = Utility::String::toLower(data) == "true";
			break;
		case Types::DataType::DataType_Color:
			TGNF_COLOR_ASSERT(data, classFile);
			newNode.luaVarData[var].colorVar = Utility::Color::ColorFromHex(data);
			break;
		case Types::DataType::DataType_Float:
			// MAKE ASSERT FOR FLOAT
			newNode.luaVarData[var].floatVar = std::stof(data);
			break;
		case Types::DataType::DataType_Int:
			// MAKE ASSERT FOR INT
			newNode.luaVarData[var].intVar = std::stoi(data);
			break;
		case Types::DataType::DataType_Tex:
			LOG_CRITICAL("Type 'tex' is not compatible with 'default' keyword in {0}", classFile);
			std::exit(134);
			break;
		}
	}
	 
	newNode.luaLines = luaLines; 

	return newNode;
}