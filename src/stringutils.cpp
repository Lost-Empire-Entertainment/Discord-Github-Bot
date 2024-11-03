//Copyright(C) 2024 Lost Empire Entertainment

#include <sstream>

#include "stringutils.hpp"

using std::istringstream;

namespace Utils
{
	string String::CharReplace(const string& input, const char& search, const char& replace)
	{
		string result = input;
		size_t pos = 0;
		while ((pos = result.find(search, pos)) != string::npos)
		{
			result[pos] = replace;
			pos += 1;
		}
		return result;
	}

	vector<string> String::Split(const string& input, char delimiter)
	{
		vector<string> tokens;
		string token;
		istringstream tokenStream(input);
		while (getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}
}