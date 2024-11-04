//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <list>

namespace Core
{
	using std::string;
	using std::function;
	using std::unordered_map;
	using std::list;

	class BotMechanics
	{
	public:
		static inline string guildID;
		static inline string logChannelID;
		static inline string botID;

		static inline unordered_map<string, string> userMap;

		static void Initialize();
		static void SendDiscordMessage(const string& channelID, const string& message);
		static void Shutdown();
	private:
		static inline unordered_map<string, string> messageCache;
		static inline list<string> messageOrder;

		static void BotMessageEvents();

		static void GetChannelName(const string& channelID, function<void(const string&)> callback);
		static void GetUsername(const string& userID, function<void(const string&)> callback);
		static void GetGuildName(const string& guildID, function<void(const string&)> callback);
		static void GetReactionName(const string& reactionID, const string& guildID, function<void(const string&)> callback);
	};
}