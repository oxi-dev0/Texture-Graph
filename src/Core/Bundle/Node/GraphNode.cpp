#include "GraphNode.h"

#define TGNL_LINE_ASSERT(keyword, expected, given, file)	if (expected != given) { LOG_CRITICAL("Too many parameters passed to keyword '{3}'. {1} expected but {2} were given. (Node Class '{0}')", file, expected, given, keyword); }
#define TGNL_RANGE_LINE_ASSERT(keyword, expectedMin, expectedMax, given, file)	if (given < expectedMin || given > expectedMax) { LOG_CRITICAL("Incorrect amount of parameters passed to keyword '{4}'. {1} - {2} expected but {3} were given. (Node Class '{0}')", file, expectedMin, expectedMax, given, keyword); }

#define TGNL_PARAM_ASSERT(type, file)						if (Utility::String::includes({"colortex", "greytex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type '{1}' is not allowed to be used with the keyword 'param'. (Node Class '{0}')", file, type); }
#define TGNL_PIN_ASSERT(keyword, type, file)				if (!Utility::String::includes({"colortex", "greytex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type '{2}' is not allowed to be used with the keyword '{1}'. (Node Class '{0}')", file, keyword, type); }
#define TGNL_DISPLAY_ASSERT(type, file)						if (!Utility::String::includes({"colortex", "greytex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type '{0}' cannot be used with keyword 'display'. (Node Class {1})", type, file); }

#define TGNL_IO_TYPE_ASSERT(keyword, type, file)				if (!Utility::String::includes({"greytex", "colortex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Unrecognised type '{0}' given to '{2}'. (Node Class '{1}')", type, file, keyword); }
#define TGNL_PARAM_TYPE_ASSERT(keyword, type, file)				if (!Utility::String::includes({"int", "float", "color", "bool", "vec2", "enum"}, Utility::String::toLower(type))) { LOG_CRITICAL("Unrecognised type '{0}' given to '{2}'. (Node Class '{1}')", type, file, keyword); }
#define TGNL_FLOAT_ASSERT(keyword, string, file)			if (!Utility::String::isFloat(string)) { LOG_CRITICAL("Invalid value passed to keyword '{0}'. Expected valid float, but got '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_INT_ASSERT(keyword, string, file)				if (!Utility::String::isInt(string)) { LOG_CRITICAL("Invalid value passed to keyword '{0}'. Expected valid int, but got '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_BOOL_ASSERT(keyword, bool, file)				if (!Utility::String::includes({"false", "true"}, Utility::String::toLower(bool))) { LOG_CRITICAL("Unrecognised bool '{0}' given to keyword '{2}'. (Node Class '{1}')", bool, file, keyword); }
#define TGNL_STRING_ASSERT(keyword, string, file)			if (string[0] != '"' || string[string.size()-1] != '"') { LOG_CRITICAL("Invalid string given to keyword '{0}'. Received '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_COLOR_ASSERT(keyword, colorString, file)		if (colorString.substr(0,2) != "0x") { LOG_CRITICAL("Invalid color given to keyword '{0}'. Received '{1}', which is missing the '0x' prefix. (Node Class '{2}')", keyword, colorString, file); }												  if (colorString.size() != 10) { LOG_CRITICAL("Invalid color given to keyword '{0}'. Received '{1}', which has an incorrect length. (Node Class '{2}')", keyword, colorString, file); } 
#define TGNL_VEC2_ASSERT(keyword, vec2String, file)			{std::vector<std::string> parts = Utility::String::split(vec2String, ','); if (parts.size() != 2 || (!Utility::String::isInt(parts[0]) && !Utility::String::isFloat(parts[0])) || (!Utility::String::isFloat(parts[1]) && !Utility::String::isInt(parts[1]))) { LOG_CRITICAL("Invalid vec2 given to keyword '{0}'. Received '{1}', which does not meet the format 'x,y'. (Node Class '{2}')", keyword, vec2String, file); }}

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

GraphNode* GraphNode::LoadFromTGNF(std::string classFolder, std::function<bool(const std::string&, const std::string&, ConditionParser::ExpressionBase*)> registerShowExpressionCallback) {
	if (!std::filesystem::exists(classFolder)) { LOG_CRITICAL("Node Class '{0}' could not be found.", classFolder); }
	if (!std::filesystem::exists(classFolder + "/.metadata")) { LOG_CRITICAL("Node Class '{0}' is missing the .metadata file.", classFolder); }
	if (!std::filesystem::exists(classFolder + "/program.lua")) { LOG_CRITICAL("Node Class '{0}' is missing the program.lua file.", classFolder); }

	std::fstream metadataFile(classFolder + "/.metadata");
	std::fstream programFile(classFolder + "/program.lua");

	GraphNode newNode;
	newNode.nodeClass = classFolder;

	bool nameSet = false;
	bool colorSet = false;
	bool categorySet = false;
	bool displaySet = false;

	bool hasOutput = false;

	std::vector<std::string> luaLines;
	std::string lLine;
	while (std::getline(programFile, lLine)) {
		luaLines.push_back(lLine);
	}
	newNode.luaLines = luaLines;
	programFile.close();

	std::string line;
	int lIndex = -1;
	while (std::getline(metadataFile, line)) {
		lIndex += 1;
		Utility::String::ltrim(line); // Trim Whitespace left
		Utility::String::rtrim(line); // Trim Whitespace right
		if (line[0] == '#') { continue; } // Ignore comment lines
		if (line == "") { continue; }	// Ignore empty

		auto data = SmartSplit(line, ' ', '"'); // Smart stick allows "stick" characters to toggle seperation, e.g, a string shouldnt be seperated

		if (data[0] == "name") { TGNL_LINE_ASSERT("name",1,data.size()-1,classFolder); TGNL_STRING_ASSERT("name",data[1],classFolder); newNode.displayName=RemoveStringIndicators(data[1]); nameSet=true; continue; }
		if (data[0] == "category") { TGNL_LINE_ASSERT("category",1,data.size()-1,classFolder); TGNL_STRING_ASSERT("category",data[1],classFolder); newNode.category=RemoveStringIndicators(data[1]); categorySet=true; continue; }
		if (data[0] == "color") { TGNL_LINE_ASSERT("color",1,data.size()-1,classFolder); TGNL_COLOR_ASSERT("color",data[1],classFolder); newNode.displayColor=Utility::Color::ColorFromHex(data[1]); colorSet=true; continue; }
		if (data[0] == "display") { TGNL_LINE_ASSERT("color",1,data.size()-1,classFolder); newNode.displayVar=data[1]; displaySet=true; continue; }

		if (data[0] == "show") {
			if (data.size() < 3) {
				LOG_CRITICAL("Too little parameters passed to keyword 'show'. (Node Class '{0}')", classFolder)
			}

			bool exists = false;
			for (std::map<std::string, LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it)
			{
				if (it->first == data[1]) {
					exists = true;
				}
			}
			if (!exists) { LOG_CRITICAL("Variable '{0}' is not defined before the 'show' keyword. (Node Class '{1}')", data[1], classFolder); continue; }

			std::stringstream exprStr;
			for (int i = 2; i < data.size(); i++) {
				exprStr << data[i];
				if (i != data.size() - 1) {
					exprStr << " ";
				}
			}
			std::string exprStrF = exprStr.str();
			exprStrF.erase(std::remove(exprStrF.begin(), exprStrF.end(), ' '), exprStrF.end());

			Utility::Timer tmr;
			ConditionParser::ExpressionBase* expr = ConditionParser::ExpressionPair::Parse(exprStrF);
			LOG_TRACE("Compiled show expression for '{1}':'{2}' in {0}s", tmr.Elapsed(), classFolder, data[1]);

			if (!registerShowExpressionCallback(newNode.nodeClass, data[1], expr)) {
				LOG_CRITICAL("Variable '{0}' has 'show' condition defined multiple times. (Node Class '{1}')", data[1], classFolder);
			}
		}

		if (data[0] == "input") {
			TGNL_LINE_ASSERT("input", 3, data.size() - 1, classFolder);
			TGNL_IO_TYPE_ASSERT("input", data[1], classFolder);
			TGNL_PIN_ASSERT("input", data[1], classFolder);
			TGNL_STRING_ASSERT("input", data[3], classFolder);

			bool exists = false;
			for (std::map<std::string, LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it)
			{
				if (it->first == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is defined multiple times. (Node Class '{1}')", data[2], classFolder); continue; }

			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			NodePin newPin = NodePin((int)newNode.pins.size(), &newNode.nodeId, dataType, Direction::In, RemoveStringIndicators(data[3]));
			newNode.pins.push_back(newPin);
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.pinLuaVars.insert({ newPin.pinIndex, data[2] });
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			newNode.luaVarPins.insert({ data[2], newPin.pinIndex });

			newNode.luaVarDisplayNames.insert({ data[2], RemoveStringIndicators(data[3]) });
			continue;
		}

		if (data[0] == "output") {
			TGNL_LINE_ASSERT("output", 3, data.size() - 1, classFolder);
			TGNL_IO_TYPE_ASSERT("output", data[1], classFolder);
			TGNL_PIN_ASSERT("output", data[1], classFolder);
			TGNL_STRING_ASSERT("output", data[3], classFolder);

			bool exists = false;
			for (std::map<std::string, LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it)
			{
				if (it->first == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is defined multiple times. (Node Class '{1}')", data[2], classFolder); }

			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			NodePin newPin = NodePin((int)newNode.pins.size(), &newNode.nodeId, dataType, Direction::Out, RemoveStringIndicators(data[3]));
			newNode.pins.push_back(newPin);
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.pinLuaVars.insert({ newPin.pinIndex, data[2] });
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			newNode.luaVarPins.insert({ data[2], newPin.pinIndex });

			newNode.luaVarDisplayNames.insert({ data[2], RemoveStringIndicators(data[3]) });

			hasOutput = true;
			continue;
		}

		if (data[0] == "param") {
			TGNL_RANGE_LINE_ASSERT("param", 3, 5, data.size() - 1, classFolder);
			TGNL_PARAM_TYPE_ASSERT("param", data[1], classFolder);
			TGNL_STRING_ASSERT("param", data[3], classFolder);

			bool exists = false;
			for (std::map<std::string, LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it)
			{
				if (it->first == data[2]) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable '{0}' is defined multiple times. (Node Class '{1}')", data[2], classFolder); }

			exists = false;
			for (std::map<std::string, std::string>::iterator it = newNode.paramLuaVars.begin(); it != newNode.paramLuaVars.end(); ++it)
			{
				if (it->first == RemoveStringIndicators(data[3])) {
					exists = true;
				}
			}
			if (exists) { LOG_CRITICAL("Variable Display Name '{0}' cannot be used more than once. (Node Class '{1}')", RemoveStringIndicators(data[3]), classFolder); }

			Types::DataType dataType = Types::stringToType[Utility::String::toLower(data[1])];
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.paramLuaVars.insert({ RemoveStringIndicators(data[3]), data[2]});
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });

			newNode.luaVarDisplayNames.insert({ data[2], RemoveStringIndicators(data[3]) });

			if (data.size() > 4) {
				switch (dataType) {
				case Types::DataType::DataType_Bool:
					TGNL_BOOL_ASSERT("default", data[4], classFolder);
					newNode.luaVarData[data[2]].boolVar = Utility::String::toLower(data[4]) == "true";
					break;
				case Types::DataType::DataType_Color:
					TGNL_COLOR_ASSERT("default", data[4], classFolder);
					newNode.luaVarData[data[2]].colorVar = Utility::Color::ColorFromHex(data[4]);
					break;
				case Types::DataType::DataType_Float:
					TGNL_FLOAT_ASSERT("default", data[4], classFolder);
					newNode.luaVarData[data[2]].floatVar = std::stof(data[4]);
					break;
				case Types::DataType::DataType_Int:
					TGNL_INT_ASSERT("default", data[4], classFolder);
					newNode.luaVarData[data[2]].intVar = std::stoi(data[4]);
					break;
				case Types::DataType::DataType_Vec2:
				{
					TGNL_VEC2_ASSERT("default", data[4], classFolder);
					std::vector<std::string> parts = Utility::String::split(data[4], ',');
					newNode.luaVarData[data[2]].vec2Var = Types::Vec2(std::stof(parts[0]), std::stof(parts[1]));
				}	break;
				case Types::DataType::DataType_Enum:
				{
					auto parts = SmartSplit(data[4], ',', '"');
					TGNL_STRING_ASSERT("default", parts[0], classFolder);
					newNode.luaVarData[data[2]].enumVar = RemoveStringIndicators(parts[0]);
				}	break;
				}
			}

			if (data[1] == "enum") {
				if (data.size() == 4) {
					LOG_CRITICAL("Variable '{0}' is missing it's enum definition. (Node Class '{1}')", data[2], classFolder);
				}

				auto parts = SmartSplit(data[data.size() - 1], ',', '"');
				std::vector<std::string> enumParts = {};
				for (auto part : parts) {
					TGNL_STRING_ASSERT("enum", part, classFolder);
					enumParts.push_back(RemoveStringIndicators(part));
				}
				newNode.luaVarEnumSets.insert({ data[2], enumParts });
			}

			continue;
		}
	}

	if (newNode.displayVar != "") {
		if (newNode.luaVars.find(newNode.displayVar) != newNode.luaVars.end()) {
			TGNL_DISPLAY_ASSERT(Types::typeToString[newNode.luaVars.find(newNode.displayVar)->second.type], classFolder);
			if (newNode.pins[newNode.luaVarPins[newNode.luaVars.find(newNode.displayVar)->second.name]].dir == Direction::In) {
				LOG_CRITICAL("Only 'Out' facing pins can be used for keyword 'display'. Variable '{0}' is facing in. (Node Class '{1}')", newNode.displayVar, classFolder);
			}
		}
		else {
			LOG_CRITICAL("Variable '{0}' given to keyword 'display' is not defined. (Node Class '{1}')", newNode.displayVar, classFolder);
		}
	}

	if (!nameSet) {
		LOG_CRITICAL("Missing metadata keyword 'name' (Node Class '{0}')", classFolder);
	}
	if (!colorSet) {
		LOG_CRITICAL("Missing metadata keyword 'color' (Node Class '{0}')", classFolder);
	}
	if (!categorySet) {
		LOG_CRITICAL("Missing metadata keyword 'category' (Node Class '{0}')", classFolder);
	}
	if (!displaySet) {
		LOG_CRITICAL("Missing metadata keyword 'display' (Node Class '{0}')", classFolder);
	}
	if (!hasOutput) {
		LOG_CRITICAL("Missing at least one output pin. (Node Class '{0}')", classFolder);
	}

	std::ifstream coreFile("library/Nodes/core.lua");

	std::ofstream tempFile;
	std::stringstream fileName;
	auto splitClass = Utility::String::split(newNode.nodeClass, '/');
	auto classNameLast = splitClass[splitClass.size() - 1];
	fileName << "temp/node-exec/" << classNameLast << ".lua";

	tempFile.open(fileName.str());  

	int lineCount = 0;
	
	tempFile << "-- [PREGENERATED CORE] --\n";
	lineCount += 1;

	std::string coreLine;
	while (std::getline(coreFile, coreLine))
	{
		Utility::String::rtrim(coreLine);
		tempFile << coreLine << "\n";
		lineCount += 1;
	}
	coreFile.close();

	tempFile << "\n-- [VARIABLE COPY (IMPROVES PERF)] --\n";
	
	tempFile << "local sizeX = g_sizeX\n";
	tempFile << "local sizeY = g_sizeY\n";
	lineCount += 2;
	for (std::map<std::string, GraphNode::LuaVar>::iterator it = newNode.luaVars.begin(); it != newNode.luaVars.end(); ++it) {
		tempFile << "local " << it->first << " = g_" << it->first << "\n";
		lineCount += 1;
	}
	
	tempFile << "-- [NODE EXEC] --\n";
	lineCount += 1;
	newNode.luaTempCoreOffset = lineCount;

	for (auto line : newNode.luaLines) {
		tempFile << line << "\n";
	}

	tempFile << "-- [RESOLVE GLOBALS] --\n";
	for (std::map<std::string, int>::iterator it = newNode.luaVarPins.begin(); it != newNode.luaVarPins.end(); ++it) {
		tempFile << "g_" << it->first << " = " << it->first << "\n";
	}

	tempFile.close();

	newNode.luaTempFile = fileName.str();

	return new GraphNode(newNode);
}

sf::Text CenteredText(std::string s, sf::Vector2f centerPos, unsigned int textSize, sf::Font& font) {
	sf::Text text;
	text.setFont(font);
	text.setString(s);
	text.setCharacterSize(textSize);

	//CENTER
	size_t CharacterSize = text.getCharacterSize();
	size_t MaxHeight = 0;

	for (size_t x = 0; x < s.size(); ++x)
	{
		sf::Uint32 Character = s.at(x);

		const sf::Glyph& CurrentGlyph = font.getGlyph(Character, (unsigned int)CharacterSize, false);

		size_t Height = (size_t)CurrentGlyph.bounds.height;

		if (MaxHeight < Height)
			MaxHeight = Height;
	}

	sf::FloatRect rect = text.getLocalBounds();

	rect.left = (float)((centerPos.x) - (rect.width / 2.0f));
	rect.top = (float)((centerPos.y) - (MaxHeight / 2.0f) - (rect.height - MaxHeight) + ((rect.height - CharacterSize) / 2.0f));

	text.setPosition(sf::Vector2f(rect.left, rect.top));
	return text;
}

sf::Text ScaleCentered(sf::Text t, float factor) {
	auto bBefore = t.getGlobalBounds();
	t.setScale(sf::Vector2f(factor, factor));
	auto bAfter = t.getGlobalBounds();
	auto rBefore = bBefore.left + bBefore.width;
	auto rAfter = bAfter.left + bAfter.width;
	auto hBefore = bBefore.top + bBefore.height;
	auto hAfter = bAfter.top + bAfter.height;
	auto rdiff = rBefore - rAfter;
	auto hdiff = hBefore - hAfter;
	t.setPosition(t.getPosition() + sf::Vector2f(rdiff / 2, hdiff / 2));
	return t;
}


const float NODE_MINWIDTH = 100.0f;
const float NODE_MINHEIGHT = 130.0f;

const float NODE_TITLE_HEIGHT = 15.0f;
const float NODE_COLOR_THICKNESS = 1.0f;

const float PIN_COMPONENTHEIGHT = 20.0f;
const float PIN_PADDING = 0.0f;
const float PIN_RADIUS = 6.5f;


sf::FloatRect GraphNode::calcBounds() {
	std::vector<float> pinHeights;
	float phSumIn = 0.0f;
	float phSumOut = 0.0f;
	int pCountIn = 0;
	int pCountOut = 0;
	std::vector<int> inPinIndexes;
	std::vector<int> outPinIndexes;
	int i = 0;
	for (auto& pin : pins) {
		auto type = pin.type;
		pinHeights.push_back(PIN_COMPONENTHEIGHT);
		if (pin.dir == Direction::In) {
			phSumIn += PIN_COMPONENTHEIGHT;
			pCountIn += 1;
			inPinIndexes.push_back(i);
		}
		else {
			phSumOut += PIN_COMPONENTHEIGHT;
			pCountOut += 1;
			outPinIndexes.push_back(i);
		}
		i++;
	}

	float neededPinHeight = std::max(phSumIn, phSumOut) + (PIN_PADDING * 2) + NODE_TITLE_HEIGHT;

	float nodeHeight = std::max(NODE_MINHEIGHT, neededPinHeight);
	float nodeWidth = NODE_MINWIDTH;

	return sf::FloatRect(nodePos, sf::Vector2f(nodeWidth, nodeHeight));
}

float remapRange(float value, float low1, float high1, float low2, float high2) {
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

sf::Color threeColorLerp(sf::Color a, sf::Color b, sf::Color c, float t) {
	sf::Color r = sf::Color::White;
	if (t <= 0.5) {
		r.r = (int)std::lerp(a.r, b.r, t * 2.f);
		r.g = (int)std::lerp(a.g, b.g, t * 2.f);
		r.b = (int)std::lerp(a.b, b.b, t * 2.f);
		r.a = (int)std::lerp(a.a, b.a, t * 2.f);
	}
	else {
		r.r = (int)std::lerp(b.r, c.r, (t-0.5f) * 2.f);
		r.g = (int)std::lerp(b.g, c.g, (t-0.5f) * 2.f);
		r.b = (int)std::lerp(b.b, c.b, (t-0.5f) * 2.f);
		r.a = (int)std::lerp(b.a, c.a, (t-0.5f) * 2.f);
	}
	return r;
}

void GraphNode::SFMLRender(sf::RenderTarget& target, float zoomLevel, bool selected, int transparency) {
	pinPosCache.clear();

	// Calculate sizes
	std::vector<float> pinHeights;
	float phSumIn = 0.0f;
	float phSumOut = 0.0f;
	int pCountIn = 0;
	int pCountOut = 0;
	std::vector<int> inPinIndexes;
	std::vector<int> outPinIndexes;
	int i = 0;
	for (auto& pin : pins) {
		auto type = pin.type;
		pinHeights.push_back(PIN_COMPONENTHEIGHT);
		if (pin.dir == Direction::In) {
			phSumIn += PIN_COMPONENTHEIGHT;
			pCountIn += 1;
			inPinIndexes.push_back(i);
		}
		else {
			phSumOut += PIN_COMPONENTHEIGHT;
			pCountOut += 1;
			outPinIndexes.push_back(i);
		}
		i++;
	}

	float neededPinHeight = std::max(phSumIn, phSumOut) + (PIN_PADDING * 2) + NODE_TITLE_HEIGHT;

	float nodeHeight = std::max(NODE_MINHEIGHT, neededPinHeight);
	float nodeWidth = NODE_MINWIDTH;

	// Draw node
	sf::RectangleShape nodeRect;
	nodeRect.setPosition(nodePos);
	nodeRect.setSize(sf::Vector2f(nodeWidth, nodeHeight));
	nodeRect.setFillColor(sf::Color(70, 70, 70, transparency));

	sf::Color outlineColor = sf::Color(120, 120, 120);
	float outlineThickness = 0.4f;
	if (isEvaluating) {
		outlineColor = sf::Color(109, 171, 247, transparency);
		outlineThickness = 1.f;
	}
	if (selected) {
		outlineColor = sf::Color(246, 157, 190, transparency);
		outlineThickness = 1.f;
	}

	float outlineMultiplier = remapRange(zoomLevel, .2f, 4.f, 1.f, 4.f);

	nodeRect.setOutlineColor(outlineColor);
	nodeRect.setOutlineThickness(outlineThickness * outlineMultiplier);

	target.draw(nodeRect);

	if (zoomLevel < 2.f) {
		sf::RectangleShape titleRect;
		titleRect.setPosition(nodePos);
		titleRect.setSize(sf::Vector2f(nodeWidth, NODE_TITLE_HEIGHT));
		titleRect.setFillColor(sf::Color(50, 50, 50, transparency));
		target.draw(titleRect);

		sf::RectangleShape colorRect;
		colorRect.setPosition(sf::Vector2f(nodePos.x, (nodePos.y + NODE_TITLE_HEIGHT) - (NODE_COLOR_THICKNESS / 2)));
		colorRect.setSize(sf::Vector2f(nodeWidth, NODE_COLOR_THICKNESS));
		colorRect.setFillColor(sf::Color(displayColor.r, displayColor.g, displayColor.b, transparency));
		target.draw(colorRect);

		const float baseTextSize = 9.6f;
		const float DPIScaleMax = 5.0f;
		const float DPIScaleMin = 0.2f;
		float textDPIScale = remapRange(zoomLevel, 0.5f, 2.0f, DPIScaleMax, DPIScaleMin);
		textDPIScale = std::clamp(textDPIScale, DPIScaleMin, DPIScaleMax);

		sf::Text titleText = CenteredText(displayName, sf::Vector2f(nodePos.x + nodeWidth / 2, nodePos.y + (NODE_TITLE_HEIGHT / 2) - 2.0f), (unsigned int)(baseTextSize * textDPIScale), RenderingGlobals::font);
		titleText = ScaleCentered(titleText, 1 / textDPIScale);
		titleText.setFillColor(sf::Color(255, 255, 255, transparency));
		target.draw(titleText);

		sf::Text timingText = CenteredText(std::format("{:.2f}", prevEvalTime * 1000.0f) + "ms", sf::Vector2f(nodePos.x + nodeWidth / 2, nodePos.y + NODE_TITLE_HEIGHT + nodeHeight), (unsigned int)(baseTextSize * textDPIScale), RenderingGlobals::font);
		timingText = ScaleCentered(timingText, 1 / textDPIScale);
		sf::Color badC = sf::Color(255, 74, 74, 255);
		sf::Color okC = sf::Color(237, 210, 114, 255);
		sf::Color goodC = sf::Color(178, 237, 114, 255);
		timingText.setFillColor(threeColorLerp(badC, okC, goodC, std::clamp(remapRange(prevEvalTime * 1000.0f, 50.f, 500.f, 1.f, 0.f), 0.f, 1.f)));
		target.draw(timingText);
	}

	float tWidth = nodeWidth;
	float tHeight = tWidth;
	float displayPosY = (NODE_TITLE_HEIGHT / 2) + nodePos.y + (nodeHeight / 2) - (tHeight / 2);

	// Draw transparency preview texture
	sf::Sprite transparentSprite;
	transparentSprite.setTexture(*ImageCache::images["transparencyPreview"]);
	sf::Vector2u trSize = ImageCache::images["transparencyPreview"]->getSize();
	transparentSprite.setScale(sf::Vector2f(tWidth / trSize.x, tHeight / trSize.y));
	transparentSprite.setPosition(sf::Vector2f(nodePos.x, displayPosY));
	transparentSprite.setColor(sf::Color(255, 255, 255, transparency));
	target.draw(transparentSprite);

	if (!evaluated) {
		// Draw unevaluated texture
		float ms = lifeTmr.Elapsed();

		sf::Sprite transparentSprite;
		transparentSprite.setTexture(*ImageCache::images["unevaluated"]);
		sf::Vector2u trSize = ImageCache::images["unevaluated"]->getSize();
		transparentSprite.setTextureRect(sf::IntRect(sf::Vector2i(0.f, 0.f), sf::Vector2i(trSize.x, trSize.y)));
		transparentSprite.setScale(sf::Vector2f(tWidth / trSize.x, tHeight / trSize.y));
		transparentSprite.setPosition(sf::Vector2f(nodePos.x, displayPosY));
		transparentSprite.setColor(sf::Color(255, 255, 255, transparency));
		target.draw(transparentSprite);
	}
	else {
		// Draw display texture
		sf::Sprite displaySprite;
		displaySprite.setTexture(*displayTexture);
		sf::Vector2u tSize = displayTexture->getSize();
		displaySprite.setScale(sf::Vector2f(tWidth / tSize.x, tHeight / tSize.y));
		displaySprite.setPosition(sf::Vector2f(nodePos.x, displayPosY));
		displaySprite.setColor(sf::Color(255, 255, 255, transparency));
		target.draw(displaySprite);
	}

	// Calculate pin spacing
	float lPinSpace = 0;
	float rPinSpace = 0;
	if (pCountIn != 0) {
		lPinSpace = (nodeHeight - (PIN_PADDING * 2) - NODE_TITLE_HEIGHT) / (pCountIn+1);
	}
	if (pCountOut != 0) {
		rPinSpace = (nodeHeight - (PIN_PADDING * 2) - NODE_TITLE_HEIGHT) / (pCountOut+1);
	}

	// Draw in pins
	sf::VertexArray pinVerts(sf::Triangles, (pCountIn + pCountOut) * 3 * 2);
	for (int p = 0; p < pCountIn; p++) {
		float y = nodePos.y + PIN_PADDING + ((p + 1)* lPinSpace) + NODE_TITLE_HEIGHT;
		float x = nodePos.x;
		sf::Color color = Types::typeToColor[pins[inPinIndexes[p]].type];
		color.a = transparency;

		sf::Vector2f verts[4];
		verts[0] = sf::Vector2f(x - PIN_RADIUS, y);
		verts[1] = sf::Vector2f(x, y - PIN_RADIUS);
		verts[2] = sf::Vector2f(x + PIN_RADIUS, y);
		verts[3] = sf::Vector2f(x, y + PIN_RADIUS);

		pinVerts[(p * 6) + 0].position = verts[0];
		pinVerts[(p * 6) + 1].position = verts[1];
		pinVerts[(p * 6) + 2].position = verts[2];
		pinVerts[(p * 6) + 3].position = verts[2];
		pinVerts[(p * 6) + 4].position = verts[3];
		pinVerts[(p * 6) + 5].position = verts[0];

		for (int j = p * 6; j < (p * 6) + 6; j++) {
			pinVerts[j].color = color;
		}

		pinPosCache.insert({ pins[inPinIndexes[p]].pinIndex, sf::Vector2f(x,y) });

		if (selected && zoomLevel < 2.f) {
			const float baseTextSize = 9.6f;
			const float DPIScaleMax = 5.0f;
			const float DPIScaleMin = 0.2f;
			float textDPIScale = remapRange(zoomLevel, 0.5f, 2.0f, DPIScaleMax, DPIScaleMin);
			textDPIScale = std::clamp(textDPIScale, DPIScaleMin, DPIScaleMax);

			sf::Text pinText;
			pinText.setFont(RenderingGlobals::font);
			pinText.setString(pins[inPinIndexes[p]].displayName);
			pinText.setCharacterSize((unsigned int)(baseTextSize * textDPIScale));

			size_t CharacterSize = pinText.getCharacterSize();
			size_t MaxHeight = 0;
			for (size_t x = 0; x < pins[inPinIndexes[p]].displayName.size(); ++x)
			{
				sf::Uint32 Character = pins[inPinIndexes[p]].displayName.at(x);
				const sf::Glyph& CurrentGlyph = RenderingGlobals::font.getGlyph(Character, (unsigned int)CharacterSize, false);
				size_t Height = (size_t)CurrentGlyph.bounds.height;
				if (MaxHeight < Height)
					MaxHeight = Height;
			}

			sf::FloatRect rect = pinText.getLocalBounds();
			rect.left = (float)(x-15) - rect.width;
			rect.top = (float)(y)-(MaxHeight / 2.0f) - (rect.height - MaxHeight) + ((rect.height - CharacterSize) / 2.0f);
			pinText.setPosition(sf::Vector2f(rect.left, rect.top));

			auto bBefore = pinText.getGlobalBounds();
			pinText.setScale(sf::Vector2f(1 / textDPIScale, 1 / textDPIScale));
			auto bAfter = pinText.getGlobalBounds();
			auto rBefore = bBefore.left + bBefore.width;
			auto rAfter = bAfter.left + bAfter.width;
			auto hBefore = bBefore.top + bBefore.height;
			auto hAfter = bAfter.top + bAfter.height;
			auto rdiff = rBefore - rAfter;
			auto hdiff = hBefore - hAfter;
			pinText.setPosition(pinText.getPosition() + sf::Vector2f(rdiff, hdiff / 2));

			pinText.setFillColor(sf::Color(255, 255, 255, transparency));
			target.draw(pinText);
		}
	}

	// Draw out pins
	for (int p = pCountIn; p < pCountOut + pCountIn; p++) {
		float y = nodePos.y + PIN_PADDING + (((p- pCountIn) + 1) * rPinSpace) + NODE_TITLE_HEIGHT;
		float x = nodePos.x + nodeWidth;
		sf::Color color = Types::typeToColor[pins[outPinIndexes[p-pCountIn]].type];
		color.a = transparency;

		sf::Vector2f verts[4];
		verts[0] = sf::Vector2f(x - PIN_RADIUS, y);
		verts[1] = sf::Vector2f(x, y - PIN_RADIUS);
		verts[2] = sf::Vector2f(x + PIN_RADIUS, y);
		verts[3] = sf::Vector2f(x, y + PIN_RADIUS);

		pinVerts[(p * 6) + 0].position = verts[0];
		pinVerts[(p * 6) + 1].position = verts[1];
		pinVerts[(p * 6) + 2].position = verts[2];
		pinVerts[(p * 6) + 3].position = verts[2];
		pinVerts[(p * 6) + 4].position = verts[3];
		pinVerts[(p * 6) + 5].position = verts[0];

		for (int j = p * 6; j < (p * 6) + 6; j++) {
			pinVerts[j].color = color;
		}
		pinPosCache.insert({ pins[outPinIndexes[p - pCountIn]].pinIndex, sf::Vector2f(x,y) });

		if (selected && zoomLevel < 2.f) {
			const float baseTextSize = 9.6f;
			const float DPIScaleMax = 5.0f;
			const float DPIScaleMin = 0.2f;
			float textDPIScale = remapRange(zoomLevel, 0.5f, 2.0f, DPIScaleMax, DPIScaleMin);
			textDPIScale = std::clamp(textDPIScale, DPIScaleMin, DPIScaleMax);

			sf::Text pinText;
			pinText.setFont(RenderingGlobals::font);
			pinText.setString(pins[outPinIndexes[p - pCountIn]].displayName);
			pinText.setCharacterSize((unsigned int)(baseTextSize * textDPIScale));

			size_t CharacterSize = pinText.getCharacterSize();
			size_t MaxHeight = 0;
			for (size_t x = 0; x < pins[outPinIndexes[p - pCountIn]].displayName.size(); ++x)
			{
				sf::Uint32 Character = pins[outPinIndexes[p - pCountIn]].displayName.at(x);
				const sf::Glyph& CurrentGlyph = RenderingGlobals::font.getGlyph(Character, (unsigned int)CharacterSize, false);
				size_t Height = (size_t)CurrentGlyph.bounds.height;
				if (MaxHeight < Height)
					MaxHeight = Height;
			}

			sf::FloatRect rect = pinText.getLocalBounds();
			rect.left = (float)(x+15);
			rect.top = (float)(y) - (MaxHeight / 2.0f) - (rect.height - MaxHeight) + ((rect.height - CharacterSize) / 2.0f);
			pinText.setPosition(sf::Vector2f(rect.left, rect.top));

			auto bBefore = pinText.getGlobalBounds();
			pinText.setScale(sf::Vector2f(1 / textDPIScale, 1 / textDPIScale));
			auto bAfter = pinText.getGlobalBounds();
			auto rBefore = bBefore.left + bBefore.width;
			auto rAfter = bAfter.left + bAfter.width;
			auto hBefore = bBefore.top + bBefore.height;
			auto hAfter = bAfter.top + bAfter.height;
			auto rdiff = rBefore - rAfter;
			auto hdiff = hBefore - hAfter;
			pinText.setPosition(pinText.getPosition() + sf::Vector2f(0, hdiff / 2));

			pinText.setFillColor(sf::Color(255, 255, 255, transparency));
			target.draw(pinText);
		}
	}

	renderedPins = true;

	if (zoomLevel < 2.f) {
		target.draw(pinVerts);
	}

	// DEBUG: DRAW TOPOLOGICAL SORT ORDER
	//if (debugEvalIndex == -1) { return; }
	//sf::Text orderText = CenteredText(std::to_string(debugEvalIndex), sf::Vector2f(nodePos.x - 10, nodePos.y - 10), (unsigned int)(18.12f * textDPIScale), RenderingGlobals::font);
	//orderText = ScaleCentered(orderText, 1 / textDPIScale);
	//target.draw(orderText);
}

void GraphNode::SetDirty()
{
	evaluated = false;
}

static void dumpstack(lua_State* L) {
	int top = lua_gettop(L);
	for (int i = 1; i <= top; i++) {
		printf("%d\t%s\t", i, luaL_typename(L, i));
		switch (lua_type(L, i)) {
		case LUA_TNUMBER:
			printf("%g\n", lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("%s\n", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
			break;
		case LUA_TNIL:
			printf("%s\n", "nil");
			break;
		default:
			printf("%p\n", lua_topointer(L, i));
			break;
		}
	}
}

bool CheckLua(lua_State* L2, int r, std::string owner, int coreOffset) {
	if (r != LUA_OK) {
		//dumpstack(L2); 
		const char* msg = lua_tostring(L2, -1); // temp-file:temp-line: error msg
		std::vector<std::string> parts = Utility::String::split(std::string(msg), ':');
		if (parts.size() != 3) { LOG_ERROR("{0}'s program did not execute successfully. RAW Error: \"{1}\"", owner, msg); return false; }

		std::string actualError = parts[2];
		Utility::String::ltrim(actualError);
		int lineNum = std::stoi(parts[1]);

		LOG_ERROR("{0}'s program did not execute successfully. Error: \"{1}\". TGNode Line: {2}, Temp Line: {3}", owner, actualError, std::to_string((lineNum-coreOffset)), std::to_string(lineNum));
		return false;
	} 
	return true;
}

bool GraphNode::AreDependenciesEvaluated(const std::vector<GraphNode*>* nodes)
{
	bool evaluated = true;
	for (auto& pin : pins) {
		auto type = pin.type;
		if (pin.dir == Direction::In) {
			if (pin.inNodeId == -1) { continue; }
			if (nodes->operator[](pin.inNodeId)->evaluated == false) {
				evaluated = false;
			}
		}
	}
	return evaluated;
}

void GraphNode::Evaluate()
{
	isEvaluating = true;

	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);

	// Globals
	lua_pushinteger(L, texSize.x);
	lua_setglobal(L, "g_sizeX");
	lua_pushinteger(L, texSize.y);
	lua_setglobal(L, "g_sizeY");

	Utility::Timer fullTimr;
	Utility::Timer varTmr;

	for (auto& var : luaVars) {
		Utility::Timer specVarTmr;
		auto& varName = var.first;
		auto& data = luaVarData[varName];

		bool ranSet = false;

		switch (data.dataType)
		{
		case Types::DataType_Bool:
			lua_pushboolean(L, data.boolVar);
			break;
		case Types::DataType_Float:
			lua_pushnumber(L, lua_Number((double)data.floatVar));
			break;
		case Types::DataType_Int: 
			lua_pushinteger(L, data.intVar);
			break;
		case Types::DataType_Enum:
			lua_pushstring(L, data.enumVar.c_str());
			break;
		case Types::DataType_Vec2:
		{
			lua_createtable(L, 0, 2);
			lua_pushnumber(L, lua_Number((double)data.vec2Var.x));
			lua_setfield(L, -2, "x");
			
			lua_pushnumber(L, lua_Number((double)data.vec2Var.y));
			lua_setfield(L, -2, "y");
		}	break;
		case Types::DataType_Color:
		{
			Utility::Timer colorTmr;
			lua_createtable(L, 0, 4);
			lua_pushnumber(L, data.colorVar.r);
			lua_setfield(L, -2, "r");

			lua_pushnumber(L, data.colorVar.g);
			lua_setfield(L, -2, "g");

			lua_pushnumber(L, data.colorVar.b);
			lua_setfield(L, -2, "b");

			lua_pushnumber(L, data.colorVar.a);
			lua_setfield(L, -2, "a");
		} break;
		case Types::DataType_GreyTex:
		{
			lua_createtable(L, (int)data.greytexVar.size(), 0); // stack = -1: table

			int xI = 1;
			for (auto& xRow : data.greytexVar) {
				lua_pushinteger(L, xI); // stack = -1: index, -2: table
				lua_createtable(L, (int)xRow.size(), 0); // stack = -1: subtable, -2: index, -3: table

				int yI = 1;
				for (auto& y : xRow)
				{
					// stack = -1: subtable, -2: index, -3: table
					lua_pushinteger(L, yI); // stack = -1: index, -2: subtable, -3: index, -4: table
					lua_pushinteger(L, data.greytexVar[xI-1][yI-1]); // stack = -1: val, -2: index, -3: subtable, -4: index, -5: table

					lua_rawset(L, -3);
					// stack = -1: subtable, -2: index, -3: table
					yI++;
				}

				// stack = -1: subtable, -2: index, -3: table
				lua_rawset(L, -3);

				xI++;
			}
		}	break;
		case Types::DataType_ColorTex:
		{
			lua_createtable(L, (int)data.colortexVar.size(), 0); // stack = -1: table
			
			int xI = 1;
			for (auto& xRow : data.colortexVar) {
				lua_pushinteger(L, xI); // stack = -1: index, -2: table
				lua_createtable(L, (int)xRow.size(), 0); // stack = -1: subtable, -2: index, -3: table

				int yI = 1;
				for (auto& y : xRow) 
				{
					// stack = -1: subtable, -2: index, -3: table
					lua_pushinteger(L, yI); // stack = -1: index, -2: subtable, -3: index, -4: table
					lua_createtable(L, 0, 4); // stack = -1: colortable, -2: index, -3: subtable, -4: index, -5: table

					auto& color = data.colortexVar[xI-1][yI-1];

					lua_pushnumber(L, color.r); // stack = -1: value, -2: colortable, -3: index, -4: subtable, -5: index, -6: table
					lua_setfield(L, -2, "r"); // stack = -1: colortable, -2: index, -3: subtable, -4: index, -5: table

					lua_pushnumber(L, color.g);
					lua_setfield(L, -2, "g");

					lua_pushnumber(L, color.b);
					lua_setfield(L, -2, "b");

					lua_pushnumber(L, color.a);
					lua_setfield(L, -2, "a");

					lua_rawset(L, -3); // // stack = -1: colortable, -2: index, -3: subtable, -4: index, -5: table => subtable[index] = colortable
					// stack = -1: subtable, -2: index, -3: table
					yI++;
				}

				// stack = -1: subtable, -2: index, -3: table
				lua_rawset(L, -3);

				xI++;
			}
		}	break;
		default:
			break;
		}

		if (!ranSet) {
			lua_setglobal(L, std::string("g_" + varName).c_str());
		}
		LOG_TRACE("Var '{0}' took {1}ms to convert to lua", varName, specVarTmr.Elapsed() * 1000.f);
	}

	LOG_TRACE("Converted c++ vars to lua in {0}ms", varTmr.Elapsed()*1000.f);
	Utility::Timer execTmr; 
	 
	CheckLua(L, luaL_dofile(L, luaTempFile.c_str()), nodeClass, luaTempCoreOffset);

	LOG_TRACE("Executed node lua in {0}ms", execTmr.Elapsed()*1000.f);
	Utility::Timer extractTmr;

	int i = 0;
	for (auto& pin : pins) {
		if (pin.dir == Direction::Out) {
			auto type = pin.type;
			auto var = pinLuaVars[i];

			lua_getglobal(L, std::string("g_" + var).c_str());
			switch (type)
			{
			case Types::DataType_ColorTex:
			{
				if (!lua_istable(L, -1)) { LOG_ERROR("Node Class '{0}' lua return value of '{1}' is not a table.", nodeClass, var); }
				size_t xLen = lua_rawlen(L, -1);
				if (xLen != texSize.x) {
					LOG_ERROR("Node Class '{0}' lua return value of '{1}' is not the correct size.", nodeClass, var);
				}

				Types::colortex colorTex;
				lua_pushnil(L); // stack = -1: nil, -2: table
				while (lua_next(L, -2) != 0) {
					std::vector<sf::Color> yList;
					// stack = -1: value, -2: key, -3: table
					size_t yLen = lua_rawlen(L, -1);
					if (yLen != texSize.y) {
						LOG_ERROR("Node Class '{0}' lua return value of '{1}' is not the correct size.", nodeClass, var);
					}

					lua_pushnil(L); // stack = -1: nil, -2: value/table, -3: key, -4: table
					while (lua_next(L, -2)) {
						// stack = -1: value/color table, -2: key, -3: table, -4: key, -5: table
						lua_getfield(L, -1, "r");
						int r = std::clamp((int)lua_tointeger(L, -1), 0, 255); 	// stack = -1: val, -2: color table, -3: key, -4: table, -5: key, -6: table
						lua_pop(L, 1);
						lua_getfield(L, -1, "g");
						int g = std::clamp((int)lua_tointeger(L, -1), 0, 255); 	// stack = -1: val, -2: color table, -3: key, -4: table, -5: key, -6: table
						lua_pop(L, 1);
						lua_getfield(L, -1, "b");
						int b = std::clamp((int)lua_tointeger(L, -1), 0, 255); 	// stack = -1: val, -2: color table, -3: key, -4: table, -5: key, -6: table
						lua_pop(L, 1);
						lua_getfield(L, -1, "a");
						int a = std::clamp((int)lua_tointeger(L, -1), 0, 255); 	// stack = -1: val, -2: color table, -3: key, -4: table, -5: key, -6: table
						lua_pop(L, 1);
						// stack = -1: value/color table, -2: key, -3: table, -4: key, -5: table
						yList.push_back(sf::Color(r, g, b, a));
						lua_pop(L, 1); // Remove value
					}

					colorTex.push_back(yList);
					lua_pop(L, 1); // Remove variable
				}
				lua_pop(L, 1); // Remove table

				luaVarData[var].colortexVar = colorTex;
			}	break;
			case Types::DataType_GreyTex:
			{
				if (!lua_istable(L, -1)) { LOG_ERROR("Node Class '{0}' lua return value of '{1}' is not a table.", nodeClass, var); }
				size_t xLen = lua_rawlen(L, -1);
				if (xLen != texSize.x) {
					LOG_ERROR("Node Class '{0}' lua return value of '{1}' is not the correct size.", nodeClass, var);
				}

				Types::greytex greyTex;
				lua_pushnil(L); // stack = -1: nil, -2: table
				while (lua_next(L, -2) != 0) {
					std::vector<int> yList;
					// stack = -1: value, -2: key, -3: table
					size_t yLen = lua_rawlen(L, -1);
					if (yLen != texSize.y) {
						LOG_ERROR("Node Class '{0}' lua return value of '{1}' is not the correct size.", nodeClass, var);
					}

					lua_pushnil(L); // stack = -1: nil, -2: value/table, -3: key, -4: table
					while (lua_next(L, -2)) {
						// stack = -1: int, -2: key, -3: table, -4: key, -5: table
						int g = std::clamp((int)lua_tointeger(L, -1), 0, 255);
						lua_pop(L, 1);
						yList.push_back(g);
					}

					greyTex.push_back(yList);
					lua_pop(L, 1); // Remove variable
				}
				lua_pop(L, 1); // Remove table

				luaVarData[var].greytexVar = greyTex;
			}	break;
			default:
				break;
			}
		}
		i++;
	}

	LOG_TRACE("Extracted c++ vars from lua in {0}ms", extractTmr.Elapsed()*1000.f);

	// Render display
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
	lua_close(L);

	evaluated = true;
	isEvaluating = false;
}

void GraphNode::SetTextureSize(sf::Vector2i size)
{
	texSize = size;
	if (!displayTexture->create(texSize.x, texSize.y)) { LOG_ERROR("Error updating display texture"); };
	displayImage->create(displayTexture->getSize().x, displayTexture->getSize().y, sf::Color::White);
	displayTexture->update(*displayImage);

	for (auto& luaData : luaVarData) {
		if (luaData.second.dataType == Types::DataType_ColorTex) {
			luaData.second.colortexVar = Types::colortex(texSize.x, std::vector<sf::Color>(texSize.y, sf::Color::White));
		}
		else {
			if (luaData.second.dataType == Types::DataType_GreyTex) {
				luaData.second.greytexVar = Types::greytex(texSize.x, std::vector<int>(texSize.y, 0));
			}
		}
	}
}

GraphNode::GraphNode(const GraphNode& node) : GraphNode()
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
	luaVarEnumSets = node.luaVarEnumSets;
	prevEvalTime = 0.0f;
	texSize = sf::Vector2i(512, 512);
	renderedPins = false;
	luaTempCoreOffset = node.luaTempCoreOffset;
	SetTextureSize(texSize);
}

GraphNode::GraphNode(GraphNode* node) : GraphNode() {
	nodeClass = node->nodeClass;
	nodeId = node->nodeId;
	displayName = node->displayName;
	category = node->category;
	displayColor = node->displayColor;
	pins = node->pins;
	luaVarDisplayNames = node->luaVarDisplayNames;
	luaVars = node->luaVars;
	pinLuaVars = node->pinLuaVars;
	paramLuaVars = node->paramLuaVars;
	luaVarData = node->luaVarData;
	luaVarPins = node->luaVarPins;
	luaTempCoreOffset = node->luaTempCoreOffset; 
	pinPosCache = std::map<int, sf::Vector2f>();
	displayVar = node->displayVar;
	luaLines = node->luaLines;
	nodePos = node->nodePos;
	luaTempFile = node->luaTempFile;
	luaVarEnumSets = node->luaVarEnumSets;
	prevEvalTime = 0.0f;
	texSize = sf::Vector2i(512, 512);
	renderedPins = false;
	SetTextureSize(texSize);
}