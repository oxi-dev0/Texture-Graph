#include "Types.h"


namespace Types
{
	std::string WildData::AsString()
	{
		std::stringstream stream;
		switch (dataType)
		{
		case Types::DataType_Color:
			stream << std::to_string(colorVar.r) << "," << std::to_string(colorVar.g) << "," << std::to_string(colorVar.b) << "," << std::to_string(colorVar.a);
			break;
		case Types::DataType_Int:
			stream << std::to_string(intVar);
			break;
		case Types::DataType_Float:
			stream << std::to_string(floatVar);
			break;
		case Types::DataType_Bool:
			stream << std::to_string(boolVar);
			break;
		case Types::DataType_ColorTex:
			break;
		case Types::DataType_GreyTex:
			break;
		default:
			break;
		}
		return stream.str();
	}

	void WildData::FromString(std::string s)
	{
		switch (dataType)
		{
		case Types::DataType_Color:
		{
			auto data = Utility::String::split(s, ',');
			int r = std::stoi(data[0]);
			int g = std::stoi(data[1]);
			int b = std::stoi(data[2]);
			int a = std::stoi(data[3]);
			colorVar = sf::Color(r, g, b, a);
		}	break;
		case Types::DataType_Int:
			intVar = std::stoi(s);
			break;
		case Types::DataType_Float:
			floatVar = std::stof(s);
			break;
		case Types::DataType_Bool:
			boolVar = s == "1";
			break;
		case Types::DataType_ColorTex:
			break;
		case Types::DataType_GreyTex:
			break;
		default:
			break;
		}
	}
}