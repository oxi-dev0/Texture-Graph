#include "GraphNode.h"

#define TGNL_LINE_ASSERT(keyword, expected, given, file)	if (expected != given) { LOG_CRITICAL("Too many parameters passed to keyword '{3}'. {1} expected but {2} were given. (Node Class '{0}')", file, expected, given, keyword); }

#define TGNL_PARAM_ASSERT(type, file)						if (Utility::String::includes({"colortex", "greytex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type '{1}' is not allowed to be used with the keyword 'param'. (Node Class '{0}')", file, type); }
#define TGNL_PIN_ASSERT(keyword, type, file)				if (!Utility::String::includes({"colortex", "greytex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type '{2}' is not allowed to be used with the keyword '{1}'. (Node Class '{0}')", file, keyword, type); }
#define TGNL_DISPLAY_ASSERT(type, file)						if (!Utility::String::includes({"colortex", "greytex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Type '{0}' cannot be used with keyword 'display'. (Node Class {1})", type, file); }

#define TGNL_TYPE_ASSERT(keyword, type, file)				if (!Utility::String::includes({"greytex", "int", "float", "color", "bool", "colortex"}, Utility::String::toLower(type))) { LOG_CRITICAL("Unrecognised type '{0}' given to '{2}'. (Node Class '{1}')", type, file, keyword); }
#define TGNL_FLOAT_ASSERT(keyword, string, file)			if (!Utility::String::isFloat(string)) { LOG_CRITICAL("Invalid value passed to keyword '{0}'. Expected valid float, but got '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_INT_ASSERT(keyword, string, file)				if (!Utility::String::isInt(string)) { LOG_CRITICAL("Invalid value passed to keyword '{0}'. Expected valid int, but got '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_BOOL_ASSERT(keyword, bool, file)				if (!Utility::String::includes({"false", "true"}, Utility::String::toLower(bool))) { LOG_CRITICAL("Unrecognised bool '{0}' given to keyword '{2}'. (Node Class '{1}')", bool, file, keyword); }
#define TGNL_STRING_ASSERT(keyword, string, file)			if (string[0] != '"' || string[string.size()-1] != '"') { LOG_CRITICAL("Invalid string given to keyword '{0}'. Received '{1}'. (Node Class '{2}')", keyword, string, file); }
#define TGNL_COLOR_ASSERT(keyword, colorString, file)		if (colorString.substr(0,2) != "0x") { LOG_CRITICAL("Invalid color given to keyword '{0}'. Received '{1}', which is missing the '0x' prefix. (Node Class '{2}')", keyword, colorString, file); }												  if (colorString.size() != 10) { LOG_CRITICAL("Invalid color given to keyword '{0}'. Received '{1}', which has an incorrect length. (Node Class '{2}')", keyword, colorString, file); } 

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

GraphNode* GraphNode::LoadFromTGNF(std::string classFile) {
	if (!std::filesystem::exists(classFile)) { LOG_CRITICAL("Node Class '{0}' could not be found.", classFile); }
	std::ifstream f(classFile);
	GraphNode newNode;

	newNode.nodeClass = classFile;

	bool metadata = false;
	bool parsedMetadata = false;
	bool parsedVariables = false;
	bool execdata = false;
	bool execprime = false;

	bool setName = false;
	bool setColor = false;
	bool setCategory = false;

	std::vector<std::string> luaLines;

	std::map<std::string, std::string> pendingDefaults; // defaults are defined at the start of the file, but have to be processed after variables are specified.

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
			if (keyword == "name") { TGNL_LINE_ASSERT("name", 1, data.size() - 1, classFile) TGNL_STRING_ASSERT("name", data[1], classFile) newNode.displayName = RemoveStringIndicators(data[1]); setName = true; continue; }
			if (keyword == "color") { TGNL_LINE_ASSERT("color", 1, data.size() - 1, classFile) TGNL_COLOR_ASSERT("color", data[1], classFile) newNode.displayColor = Utility::Color::ColorFromHex(data[1]); setColor = true; continue; }
			if (keyword == "category") { TGNL_LINE_ASSERT("category", 1, data.size() - 1, classFile) TGNL_STRING_ASSERT("category", data[1], classFile) newNode.category = RemoveStringIndicators(data[1]); setCategory = true; continue; }
			if (keyword == "varname") { TGNL_LINE_ASSERT("varname", 2, data.size() - 1, classFile) TGNL_STRING_ASSERT("varname", data[2], classFile) newNode.luaVarDisplayNames.insert({data[1], RemoveStringIndicators(data[2])}); continue; }
			if (keyword == "display") { TGNL_LINE_ASSERT("color", 1, data.size() - 1, classFile) newNode.displayVar = data[1]; continue; }
			if (keyword == "default") { TGNL_LINE_ASSERT("default", 2, data.size() - 1, classFile) pendingDefaults.insert({ data[1], data[2] }); continue; } // defaults have to be cached until all vars are loaded

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
			newNode.paramLuaVars.insert({ newNode.luaVarDisplayNames.find(data[2])->second, data[2]});
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

			NodePin newPin = NodePin((int)newNode.pins.size(), &newNode.nodeId, dataType, Direction::Out, newNode.luaVarDisplayNames[data[2]]);
			newNode.pins.push_back(newPin);
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.pinLuaVars.insert({ newPin.pinIndex, data[2]});
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			newNode.luaVarPins.insert({ data[2], newPin.pinIndex });
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
			NodePin newPin = NodePin((int)newNode.pins.size(), &newNode.nodeId, dataType, Direction::In, newNode.luaVarDisplayNames[data[2]]);
			newNode.pins.push_back(newPin);
			newNode.luaVars.insert({ data[2], LuaVar(data[2], dataType) });
			newNode.pinLuaVars.insert({ newPin.pinIndex, data[2] });
			Types::WildData newData = Types::WildData();
			newData.dataType = dataType;
			newNode.luaVarData.insert({ data[2], newData });
			newNode.luaVarPins.insert({ data[2], newPin.pinIndex });
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
		case Types::DataType::DataType_ColorTex:
			LOG_CRITICAL("Type 'colortex' is not compatible with keyword 'default'. (Node Class '{0}')", classFile);
			break;
		case Types::DataType::DataType_GreyTex:
			LOG_CRITICAL("Type 'greytex' is not compatible with keyword 'default'. (Node Class '{0}')", classFile);
			break;
		}
	}

	if (newNode.displayVar != "") {
		if (newNode.luaVars.find(newNode.displayVar) != newNode.luaVars.end()) {
			TGNL_DISPLAY_ASSERT(Types::typeToString[newNode.luaVars.find(newNode.displayVar)->second.type], classFile);
			if (newNode.pins[newNode.luaVarPins[newNode.luaVars.find(newNode.displayVar)->second.name]].dir == Direction::In) {
				LOG_CRITICAL("Only 'Out' facing pins can be used for keyword 'display'. Variable '{0}' is facing in. (Node Class '{1}')", newNode.displayVar, classFile);
			}
		}
		else {
			LOG_CRITICAL("Variable '{0}' given to keyword 'display' is not defined. (Node Class '{1}')", newNode.displayVar, classFile);
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

	if (!setName) {
		LOG_CRITICAL("Missing metadata keyword 'name' (Node Class '{0}')", classFile);
	}
	if (!setColor) {
		LOG_CRITICAL("Missing metadata keyword 'color' (Node Class '{0}')", classFile);
	}
	if (!setCategory) {
		LOG_CRITICAL("Missing metadata keyword 'category' (Node Class '{0}')", classFile);
	}

	std::ofstream tempFile;
	std::stringstream fileName;
	auto splitClass = Utility::String::split(newNode.nodeClass, '/');
	auto classNameLast = splitClass[splitClass.size() - 1];

	classNameLast = Utility::String::split(classNameLast, '.')[0];
	fileName << "temp/node-exec/" << classNameLast << ".lua";
	tempFile.open(fileName.str());
	for (auto line : newNode.luaLines) {
		tempFile << line << "\n";
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
	nodeRect.setOutlineColor(!selected ? sf::Color(120, 120, 120) : sf::Color(246, 157, 190, transparency));
	nodeRect.setOutlineThickness(!selected ? 0.4f : 1.f);
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
		sf::Color badC = sf::Color(237, 159, 114, 255);
		sf::Color okC = sf::Color(237, 210, 114, 255);
		sf::Color goodC = sf::Color(178, 237, 114, 255);
		timingText.setFillColor(threeColorLerp(badC, okC, goodC, remapRange(prevEvalTime * 1000.0f, 30.f, 120.f, 1.f, 0.f)));
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


bool CheckLua(lua_State* L2, int r, std::string owner) {
	if (r != LUA_OK) {
		LOG_ERROR("{0}'s lua code did not execute successfully. Error: '{1}'", owner, lua_tostring(L2, -1));
		return false;
	}
	return true;
}

void GraphNode::Execute(std::atomic<int>* threadCount)
{
	if (threadCount != nullptr) {
		// Limit to only 15 nodes executing at one time
		while (*threadCount > 15) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
		(*threadCount)++;
	}

	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);

	// Globals
	lua_pushinteger(L, texSize.x);
	lua_setglobal(L, "sizeX");
	lua_pushinteger(L, texSize.y);
	lua_setglobal(L, "sizeY");

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
			lua_setglobal(L, varName.c_str());
		}
		LOG_TRACE("Var '{0}' took {1}ms to convert to lua", varName, specVarTmr.Elapsed() * 1000.f);
	}

	LOG_TRACE("Converted c++ vars to lua in {0}ms", varTmr.Elapsed()*1000.f);
	Utility::Timer execTmr; 
	 
	CheckLua(L, luaL_dofile(L, luaTempFile.c_str()), nodeClass);

	LOG_TRACE("Executed node lua in {0}ms", execTmr.Elapsed()*1000.f);
	Utility::Timer extractTmr;

	int i = 0;
	for (auto& pin : pins) {
		if (pin.dir == Direction::Out) {
			auto type = pin.type;
			auto var = pinLuaVars[i];

			lua_getglobal(L, var.c_str());
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

	if(threadCount != nullptr)
		(*threadCount)--;
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
	prevEvalTime = 0.0f;
	texSize = sf::Vector2i(512, 512);
	renderedPins = false;
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
	pinPosCache = std::map<int, sf::Vector2f>();
	displayVar = node->displayVar;
	luaLines = node->luaLines;
	nodePos = node->nodePos;
	luaTempFile = node->luaTempFile;
	prevEvalTime = 0.0f;
	texSize = sf::Vector2i(512, 512);
	renderedPins = false;
	SetTextureSize(texSize);
}