//Copyright(C) 2024 Lost Empire Entertainment

//external
#pragma warning(push, 0) //start dpp warning ignore
#include <dpp/include/dpp.h>
#pragma warning(pop) //end dpp warning ignore

//bot
#include "bot.hpp"
#include "core.hpp"

namespace Core
{
	void BotMechanics::Initialize()
	{
		string botToken = Bot::GetEnv("DISCORD_BOT_TOKEN");
		if (botToken == "")
		{
			Bot::CreateErrorPopup("Failed to get discord bot token from env file! Did you forget to add the DISCORD_BOT_TOKEN variable?");
		}
	}

	void BotMechanics::SendDiscordMessage()
	{

	}
}