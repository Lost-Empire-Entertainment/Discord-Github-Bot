//Copyright(C) 2024 Lost Empire Entertainment

#include <memory>
#include <iostream>
#include <thread>

//external
#pragma warning(push, 0) //start dpp warning ignore
#include "dpp/include/dpp.h"
#pragma warning(pop) //end dpp warning ignore

//bot
#include "bot.hpp"
#include "core.hpp"

using std::unique_ptr;
using std::make_unique;
using std::cout;
using std::thread;

namespace Core
{
	unique_ptr<dpp::cluster> bot;
	thread botThread;

	void BotMechanics::Initialize()
	{
		string botToken = Bot::GetEnv("DISCORD_BOT_TOKEN");
		if (botToken == "")
		{
			Bot::CreateErrorPopup("Failed to get discord bot token from env file! Did you forget to add the DISCORD_BOT_TOKEN variable?");
		}

		channelID = Bot::GetEnv("DISCORD_CHANNEL_ID");
		if (channelID == "")
		{
			Bot::CreateErrorPopup("Failed to get discord channel id from env file! Did you forget to add the DISCORD_CHANNEL_ID variable?");
		}

		bot = make_unique<dpp::cluster>(botToken);

		//log events to console for debugging
		bot->on_log(dpp::utility::cout_logger());

		//event handler for when the bot is ready
		bot->on_ready([&](const dpp::ready_t& event)
		{
			cout << "Bot is ready and conencted!\n";
		});

		//run the bot in a separate thread in non-blocking mode
		botThread = thread([]() 
		{
			bot->start(false);
		});
	}

	void BotMechanics::SendDiscordMessage(const string& channelID, const string& message)
	{
		dpp::snowflake convertedChannelID = static_cast<dpp::snowflake>(stoull(channelID));
		bot->message_create(dpp::message(convertedChannelID, message));
	}

	void BotMechanics::Shutdown()
	{
		bot->shutdown();

		if (botThread.joinable()) 
		{
			botThread.join();
		}
	}
}