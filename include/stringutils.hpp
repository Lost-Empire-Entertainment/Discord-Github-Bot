//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include <string>
#include <vector>

namespace Utils
{
	using std::string;
	using std::vector;

	class String
	{
	public:
		static string CharReplace(const string& input, const char& search, const char& replace);
		static vector<string> Split(const string& input, char delimiter);
	};
}