#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "Utils/Strings.h"

namespace Types {
	typedef std::vector<std::vector<sf::Color>> colortex;
	typedef std::vector<std::vector<int>> greytex;

	enum DataType {
		DataType_Color, // {r=0,g=0,b=0,a=255}
		DataType_Int,   // 0
		DataType_Float, // 0.0
		DataType_Bool,  // false
		DataType_ColorTex,   // {{{r=0,g=0,b=0,a=255},{r=0,g=0,b=0,a=255}},{{r=0,g=0,b=0,a=255},{r=0,g=0,b=0,a=255}}} (2d array of colors)
		DataType_GreyTex,
	};

	inline std::unordered_map<std::string, DataType> stringToType = {
		{"color", DataType::DataType_Color},
		{"int", DataType::DataType_Int},
		{"float", DataType::DataType_Float},
		{"bool", DataType::DataType_Bool},
		{"colortex", DataType::DataType_ColorTex},
		{"greytex", DataType::DataType_GreyTex}
	};

	inline std::unordered_map<DataType, std::string> typeToString = {
		{DataType::DataType_Color, "color"},
		{DataType::DataType_Int, "int"},
		{DataType::DataType_Float, "float"},
		{DataType::DataType_Bool, "bool"},
		{DataType::DataType_ColorTex, "colortex"},
		{DataType::DataType_GreyTex, "greytex"}
	};

	inline std::unordered_map<DataType, sf::Color> typeToColor = {
		{DataType::DataType_ColorTex, sf::Color(213,87,59)},
		{DataType::DataType_GreyTex, sf::Color(119,125,167)}
	};

	struct WildData {
	public:
		bool boolVar = false;
		int intVar = 0;
		float floatVar = 0.0f;
		sf::Color colorVar;
		std::string stringVar;
		colortex colortexVar;
		greytex greytexVar;
		DataType dataType;

		WildData(bool b) { boolVar = b; dataType = DataType::DataType_Bool; }
		WildData(int i) { intVar = i; dataType = DataType::DataType_Int; }
		WildData(float f) { floatVar = f; dataType = DataType::DataType_Float; }
		WildData(sf::Color c) { colorVar = c; dataType = DataType::DataType_Color; }
		WildData(colortex t) { colortexVar = t; dataType = DataType::DataType_ColorTex; }
		WildData(greytex t) { greytexVar = t; dataType = DataType::DataType_GreyTex; }
		WildData(DataType t) { dataType = t; }
		WildData() {
			boolVar = false;
			intVar = 0;
			floatVar = 0.0f;
			colorVar = sf::Color::White;
			colortexVar = colortex(256, std::vector<sf::Color>(256, sf::Color::White));
			greytexVar = greytex(256, std::vector<int>(256, 0));
			dataType = DataType::DataType_Bool;
			stringVar = "";
		}

		std::string AsString();
		void FromString(std::string s);
	};
}