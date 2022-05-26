#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace Types {
	typedef std::vector<std::vector<sf::Color>> tex;

	enum DataType {
		DataType_Color, // {r=0,g=0,b=0,a=255}
		DataType_Int,   // 0
		DataType_Float, // 0.0
		DataType_Bool,  // false
		DataType_Tex,   // {{{r=0,g=0,b=0,a=255},{r=0,g=0,b=0,a=255}},{{r=0,g=0,b=0,a=255},{r=0,g=0,b=0,a=255}}} (2d array of colors)
		DataType_String
	};

	inline std::unordered_map<std::string, DataType> stringToType = {
		{"color", DataType::DataType_Color},
		{"int", DataType::DataType_Int},
		{"float", DataType::DataType_Float},
		{"bool", DataType::DataType_Bool},
		{"tex", DataType::DataType_Tex},
		{"string", DataType::DataType_String}
	};

	inline std::unordered_map<DataType, std::string> typeToString = {
		{DataType::DataType_Color, "color"},
		{DataType::DataType_Int, "int"},
		{DataType::DataType_Float, "float"},
		{DataType::DataType_Bool, "bool"},
		{DataType::DataType_Tex, "tex"},
		{DataType::DataType_String, "string"}
	};

	struct WildData {
	public:
		bool boolVar = false;
		int intVar = 0;
		float floatVar = 0.0f;
		sf::Color colorVar;
		std::string stringVar;
		tex texVar;
		DataType dataType;

		WildData(bool b) { boolVar = b; dataType = DataType::DataType_Bool; }
		WildData(int i) { intVar = i; dataType = DataType::DataType_Int; }
		WildData(float f) { floatVar = f; dataType = DataType::DataType_Float; }
		WildData(sf::Color c) { colorVar = c; dataType = DataType::DataType_Color; }
		WildData(tex t) { texVar = t; dataType = DataType::DataType_Tex; }
		WildData(std::string s) { stringVar = s; dataType = DataType::DataType_String; }
		WildData() {
			boolVar = false;
			intVar = 0;
			floatVar = 0.0f;
			colorVar = sf::Color::White;
			texVar = tex();
			dataType = DataType::DataType_Bool;
			stringVar = "";
		}
	};
}