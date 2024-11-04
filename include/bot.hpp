//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include <string>

namespace Core
{
	using std::string;

	class BotMechanics
	{
	public:
		static inline string logChannelID;
		static inline string botID;

		static void Initialize();
		static void BotMessageEvents();

		static string GetChannelName(const string& channelID);
		static string GetUsername(const string& userID);
		static string GetGuildName(const string& guildID);
		static string GetReactionName(const string& reactionID, const string& guildID);

		static void SendDiscordMessage(const string& channelID, const string& message);

		static void Shutdown();
	};
}