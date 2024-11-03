//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include <string>

namespace Core
{
	using std::string;

	class BotMechanics
	{
	public:
		static inline string channelID;

		static void Initialize();
		static void SendDiscordMessage(const string& channelID, const string& message);
		static void Shutdown();
	};
}