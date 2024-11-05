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

		static void Initialize();

		static bool UserExists(const string& userID, bool forceCurrentServer = true);
		static bool ChannelExists(const string& channelID);
		static bool RoleExists(const string& roleID);

		static void BotAction_DMUser(const string& userID, const string& message);
		static void BotAction_MessageChannel(
			const string& channelID, 
			const string& message, 
			const string& userID = "",
			const string& roleID = "");
		static void BotAction_MuteUser(const string& userID, int time, const string& reason = "");
		static void BotAction_UnmuteUser(const string& userID, const string& reason = "");
		static void BotAction_KickUser(const string& userID, const string& reason = "");
		static void BotAction_BanUser(const string& userID, int time, const string& reason = "");
		static void BotAction_UnbanUser(const string& userID, const string& reason = "");

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