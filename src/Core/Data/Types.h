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
		DataType_Tex    // {{{r=0,g=0,b=0,a=255},{r=0,g=0,b=0,a=255}},{{r=0,g=0,b=0,a=255},{r=0,g=0,b=0,a=255}}} (2d array of colors)
	};

	inline std::unordered_map<std::string, DataType> stringToType = {
		{"color", DataType::DataType_Color},
		{"int", DataType::DataType_Int},
		{"float", DataType::DataType_Float},
		{"bool", DataType::DataType_Bool},
		{"tex", DataType::DataType_Tex}
	};

	struct WildData {
	public:
		bool boolVar = false;
		int intVar = 0;
		float floatVar = 0.0f;
		sf::Color colorVar;
		tex texVar;
		DataType dataType;

		WildData(bool b) { boolVar = b; dataType = DataType::DataType_Bool; }
		WildData(int i) { intVar = i; dataType = DataType::DataType_Int; }
		WildData(float f) { floatVar = f; dataType = DataType::DataType_Float; }
		WildData(sf::Color c) { colorVar = c; dataType = DataType::DataType_Color; }
		WildData(tex t) { texVar = t; dataType = DataType::DataType_Tex; }
		WildData() {
			boolVar = false;
			intVar = 0;
			floatVar = 0.0f;
			colorVar = sf::Color::White;
			texVar = tex();
			dataType = DataType::DataType_Bool;
		}
	};
}