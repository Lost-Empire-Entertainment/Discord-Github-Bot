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
#include "gui.hpp"

using std::unique_ptr;
using std::make_unique;
using std::cout;
using std::thread;

using Graphics::GUI::BotGUI;

namespace Core
{
	unique_ptr<dpp::cluster> bot;
    dpp::loglevel current_log_level = dpp::ll_trace;
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

        BotMessageEvents();

		//event handler for when the bot is ready
		bot->on_ready([&](const dpp::ready_t& event)
		{
			BotGUI::Print("Bot is ready and connected!");
		});

		//run the bot in a separate thread in non-blocking mode
		botThread = thread([]() 
		{
			bot->start(false);
		});
	}

	void BotMechanics::BotMessageEvents()
	{
        //
        // DPP NATIVE EVENTS
        //

        bot->on_guild_create([](const dpp::guild_create_t& event)
            {
                string cleanedMessage = "[GUILD] Guild data received. Bot has joined or connected to a guild.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_guild_delete([](const dpp::guild_delete_t& event)
            {
                string cleanedMessage = "[GUILD] Bot was removed from a guild or the guild was deleted.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_guild_update([](const dpp::guild_update_t& event)
            {
                string cleanedMessage = "[GUILD] Guild information updated.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_channel_create([](const dpp::channel_create_t& event)
            {
                string cleanedMessage = "[CHANNEL] New channel created in guild.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_channel_delete([](const dpp::channel_delete_t& event)
            {
                string cleanedMessage = "[CHANNEL] Channel deleted in guild.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_voice_server_update([](const dpp::voice_server_update_t& event)
            {
                string cleanedMessage = "[CHANNEL] Voice server updated in guild.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_voice_state_update([](const dpp::voice_state_update_t& event)
            {
                string cleanedMessage = "[CHANNEL] Voice state updated for a user in voice channel.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_message_create([](const dpp::message_create_t& event)
            {
                string cleanedMessage = "[EVENT] New message created in a channel.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_message_update([](const dpp::message_update_t& event)
            {
                string cleanedMessage = "[EVENT] Message edited in a channel.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_message_delete([](const dpp::message_delete_t& event)
            {
                string cleanedMessage = "[EVENT] Message deleted in a channel.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_message_reaction_add([](const dpp::message_reaction_add_t& event)
            {
                string cleanedMessage = "[EVENT] Reaction added to a message.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_message_reaction_remove([](const dpp::message_reaction_remove_t& event)
            {
                string cleanedMessage = "[EVENT] Reaction removed from a message.";
                BotGUI::Print(cleanedMessage);
            });

        bot->on_typing_start([](const dpp::typing_start_t& event)
            {
                string cleanedMessage = "[EVENT] User started typing in a channel.";
                BotGUI::Print(cleanedMessage);
            });


        //
        // DPP AND DISCORD OPCODES AND SESSION MESSAGES
        //

        bot->on_log([](const dpp::log_t& log)
            {
                string cleanedMessage;

                //opcodes
                if (log.message.find("\"op\":0") != string::npos)
                {
                    cleanedMessage = "[DISPATCH][OPCODE: 0] Dispatch event: Receiving an event from Discord.";
                }
                else if (log.message.find("\"op\":1") != string::npos)
                {
                    cleanedMessage = "[HEARTBEAT][OPCODE: 1] Heartbeat sent: Keeping connection alive with Discord.";
                }
                else if (log.message.find("\"op\":2") != string::npos)
                {
                    cleanedMessage = "[IDENTIFY][OPCODE: 2] Identify event: Sending bot properties to authenticate.";
                }
                else if (log.message.find("\"op\":3") != string::npos)
                {
                    if (log.message.find("online") != string::npos)
                    {
                        cleanedMessage = "[PRESENCE_UPDATE][OPCODE: 3] Presence update: Bot is now online.";
                    }
                    else if (log.message.find("idle") != string::npos)
                    {
                        cleanedMessage = "[PRESENCE_UPDATE][OPCODE: 3] Presence update: Bot is now idle.";
                    }
                    else
                    {
                        cleanedMessage = "[PRESENCE_UPDATE][OPCODE: 3] Presence update: Bot's presence has changed.";
                    }
                }
                else if (log.message.find("\"op\":4") != string::npos)
                {
                    cleanedMessage = "[VOICE_STATE_UPDATE][OPCODE: 4] Voice state update: Joining, leaving, or moving voice channels.";
                }
                else if (log.message.find("\"op\":6") != string::npos)
                {
                    cleanedMessage = "[RESUME][OPCODE: 6] Resume event: Attempting to resume a previous session.";
                }
                else if (log.message.find("\"op\":7") != string::npos)
                {
                    cleanedMessage = "[RECONNECT][OPCODE: 7] Reconnect requested: Discord is requesting the bot to reconnect.";
                }
                else if (log.message.find("\"op\":8") != string::npos)
                {
                    cleanedMessage = "[REQUEST_GUILD_MEMBERS][OPCODE: 8] Request guild members.";
                }
                else if (log.message.find("\"op\":9") != string::npos)
                {
                    cleanedMessage = "[INVALID_SESSION][OPCODE: 9] Invalid session: The session is invalid.";
                }
                else if (log.message.find("\"op\":10") != string::npos)
                {
                    cleanedMessage = "[HELLO][OPCODE: 10] Hello event: Connection established. Sending heartbeat interval.";
                }
                else if (log.message.find("\"op\":11") != string::npos)
                {
                    cleanedMessage = "[HEARTBEAT_ACK][OPCODE: 11] Heartbeat acknowledged: Connection is active.";
                }
                else if (log.message.find("\"op\":") != string::npos)
                {
                    cleanedMessage = "[UNKNOWN_OPCODE] Unhandled opcode detected: " + log.message;
                }

                //connection and session management
                if (cleanedMessage.empty() && log.message.find("READY") != string::npos)
                {
                    cleanedMessage = "[CONNECTION] Bot connection ready!";
                }
                else if (cleanedMessage.empty() && log.message.find("RESUMED") != string::npos)
                {
                    cleanedMessage = "[CONNECTION] Connection resumed successfully.";
                }
                else if (cleanedMessage.empty() && log.message.find("INVALIDATE_SESSION") != string::npos)
                {
                    cleanedMessage = "[SESSION] Session invalidated. Attempting to reauthenticate...";
                }
                else if (cleanedMessage.empty() && log.message.find("GATEWAY_RECONNECT") != string::npos)
                {
                    cleanedMessage = "[SESSION] Gateway requested reconnect. Reconnecting...";
                }
                else if (cleanedMessage.empty() && log.message.find("SESSION_LIMIT") != string::npos)
                {
                    cleanedMessage = "[SESSION] Session limit reached. Please wait before attempting to reconnect.";
                }

                //print the final cleaned message
                if (!cleanedMessage.empty()) BotGUI::Print(cleanedMessage);
            });
	}

	void BotMechanics::SendDiscordMessage(const string& channelID, const string& message)
	{
		dpp::snowflake convertedChannelID = static_cast<dpp::snowflake>(stoull(channelID));
		bot->message_create(dpp::message(convertedChannelID, message));

        BotGUI::Print("[BOT MESSAGE] Message '" + message + "' was sent to channel '" + channelID + "'.");
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