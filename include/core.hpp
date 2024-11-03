//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include <string>

namespace Core
{
	using std::string;

	class Bot
	{
	public:
		static inline string exeName;
		static inline string version;

		static inline string filesPath;
		static inline string docsPath;
		static inline string configFilePath;

		static inline bool isBotRunning;

		static string GetEnv(const string& envVariable);

		static void Initialize();
		static void Run();

		static void CreateErrorPopup(const char* errorMessage = "MISSING ERROR MESSAGE");
		static bool IsThisProcessAlreadyRunning(const string& processName);

		static bool IsUserIdle();

		static void Shutdown();
	};
}