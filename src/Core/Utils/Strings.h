#pragma once
#include <string>
#include <vector>
#include <algorithm>

namespace Utility {
	namespace String {
		std::vector<std::string> split(const std::string& s, char seperator);
		std::string join(std::vector<std::string> lst, std::string delim, int startIndex);
		void ltrim(std::string& s);
		void rtrim(std::string& s);
		std::string toLower(std::string s);
		bool includes(std::vector<std::string> checks, std::string s);
	}
}
