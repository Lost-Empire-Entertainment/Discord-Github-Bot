//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include <string>

namespace Utils
{
	using std::string;

	class File
	{
	public:
		static void CreateNewFolder(const string& folderPath);
		static void DeleteFileOrfolder(const string& sourcePath);
		static string SetPath();
	};
}