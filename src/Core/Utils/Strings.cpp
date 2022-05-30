#include "Strings.h"

namespace Utility {
	namespace String {
		std::vector<std::string> split(const std::string& s, char seperator)
		{
			std::vector<std::string> output;
			std::string::size_type prev_pos = 0, pos = 0;
			while ((pos = s.find(seperator, pos)) != std::string::npos)
			{
				std::string substring(s.substr(prev_pos, pos - prev_pos));
				output.push_back(substring);
				prev_pos = ++pos;
			}
			output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

			return output;
		}

		std::string replace(std::string s, char target, char replace)
		{
			std::string ret;
			for (auto c : s) {
				if (c == target) {
					ret += replace;
				}
				else {
					ret += c;
				}
			}
			return ret;
		}

		std::string join(std::vector<std::string> lst, std::string delim, int startIndex)
		{
			std::string ret;
			int i = 0;
			for (const auto& s : lst) {
				if (i < startIndex) { i++; continue; }
				if (i > startIndex) { ret += delim; }
				ret += s;
				i++;
			}
			return ret;
		}

		// trim whitespace from start
		void ltrim(std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
				}));
		}

		// trim whitespace from end
		void rtrim(std::string& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
				return !std::isspace(ch);
				}).base(), s.end());
		}

		std::string toLower(std::string s) {
			std::string sc = s;
			std::transform(sc.begin(), sc.end(), sc.begin(),
				[](unsigned char c) { return std::tolower(c); });
			return sc;
		}

		bool includes(std::vector<std::string> checks, std::string s) {
			bool includes = false;
			for (auto check : checks) {
				if (check == s) {
					includes = true;
				}
			}
			return includes;
		}

		bool isInt(const std::string& s)
		{
			return !s.empty() && std::find_if(s.begin(),
				s.end(), [](unsigned char c) { return !(std::isdigit(c) || c=='-'); }) == s.end();
		}

		bool isFloat(const std::string& s)
		{
			bool hasPoint = false;
			return !s.empty() && std::find_if(s.begin(),
				s.end(), [&hasPoint](unsigned char c) { if (c == '.') { hasPoint = true; } return !(std::isdigit(c) || c == '-' || c == '.'); }) == s.end() && hasPoint;
		}
	}
}
