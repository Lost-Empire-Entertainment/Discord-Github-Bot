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

        bot->on_log([](const dpp::log_t& log)
            {
                string op0 = "\"op\":0";
                string op1 = "\"op\":1";
                string op2 = "\"op\":2";
                string op3 = "\"op\":3";
                string op4 = "\"op\":4";
                string op6 = "\"op\":6";
                string op7 = "\"op\":7";
                string op8 = "\"op\":8";
                string op9 = "\"op\":9";
                string op10 = "\"op\":10";
                string op11 = "\"op\":11";

                string cleanedMessage;

                //opcodes
                if (log.message.find(op0) != string::npos)
                {
                    cleanedMessage = "[DISPATCH][OPCODE: 0] Dispatch event: Receiving an event from Discord (e.g., message creation, guild update).";
                }
                else if (log.message.find(op1) != string::npos)
                {
                    cleanedMessage = "[HEARTBEAT][OPCODE: 1] Heartbeat sent: Keeping connection alive with Discord.";
                }
                else if (log.message.find(op2) != string::npos)
                {
                    cleanedMessage = "[IDENTIFY][OPCODE: 2] Identify event: Sending bot properties to authenticate and start a new session.";
                }
                else if (log.message.find(op3) != string::npos)
                {
                    if (log.message.find("online") != string::npos)
                        cleanedMessage = "[PRESENCE_UPDATE][OPCODE: 3] Presence update: Bot is now online.";
                    else if (log.message.find("idle") != string::npos)
                        cleanedMessage = "[PRESENCE_UPDATE][OPCODE: 3] Presence update: Bot is now idle.";
                    else
                        cleanedMessage = "[PRESENCE_UPDATE][OPCODE: 3] Presence update: Bot's presence has changed.";
                }
                else if (log.message.find(op4) != string::npos)
                {
                    cleanedMessage = "[VOICE_STATE_UPDATE][OPCODE: 4] Voice state update: Joining, leaving, or moving voice channels.";
                }
                else if (log.message.find(op6) != string::npos)
                {
                    cleanedMessage = "[RESUME][OPCODE: 6] Resume event: Attempting to resume a previous session after disconnection.";
                }
                else if (log.message.find(op7) != string::npos)
                {
                    cleanedMessage = "[RECONNECT][OPCODE: 7] Reconnect requested: Discord is requesting the bot to reconnect.";
                }
                else if (log.message.find(op8) != string::npos)
                {
                    cleanedMessage = "[REQUEST_GUILD_MEMBERS][OPCODE: 8] Request guild members: Requesting information about guild members (e.g., offline users).";
                }
                else if (log.message.find(op9) != string::npos)
                {
                    cleanedMessage = "[INVALID_SESSION][OPCODE: 9] Invalid session: The session is invalid. Bot needs to reauthenticate or start a new session.";
                }
                else if (log.message.find(op10) != string::npos)
                {
                    cleanedMessage = "[HELLO][OPCODE: 10] Hello event: Connection established. Discord is sending the heartbeat interval.";
                }
                else if (log.message.find(op11) != string::npos)
                {
                    cleanedMessage = "[HEARTBEAT_ACK][OPCODE: 11] Heartbeat acknowledged: Discord has received the bot's heartbeat, connection is active.";
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

                //guild and channel events
                else if (cleanedMessage.empty() && log.message.find("GUILD_CREATE") != string::npos)
                {
                    cleanedMessage = "[GUILD] Guild data received. Bot has joined or connected to a guild.";
                }
                else if (cleanedMessage.empty() && log.message.find("GUILD_DELETE") != string::npos)
                {
                    cleanedMessage = "[GUILD] Bot was removed from a guild or the guild was deleted.";
                }
                else if (cleanedMessage.empty() && log.message.find("GUILD_UPDATE") != string::npos)
                {
                    cleanedMessage = "[GUILD] Guild information updated.";
                }
                else if (cleanedMessage.empty() && log.message.find("CHANNEL_CREATE") != string::npos)
                {
                    cleanedMessage = "[CHANNEL] New channel created in guild.";
                }
                else if (cleanedMessage.empty() && log.message.find("CHANNEL_DELETE") != string::npos)
                {
                    cleanedMessage = "[CHANNEL] Channel deleted in guild.";
                }
                else if (cleanedMessage.empty() && log.message.find("VOICE_SERVER_UPDATE") != string::npos)
                {
                    cleanedMessage = "[CHANNEL] Voice server updated in guild.";
                }
                else if (cleanedMessage.empty() && log.message.find("VOICE_STATE_UPDATE") != string::npos)
                {
                    cleanedMessage = "[CHANNEL] Voice state updated for a user in voice channel.";
                }

                //rate limiting and errors
                else if (cleanedMessage.empty() && log.message.find("429") != string::npos)
                {
                    cleanedMessage = "[RATE_LIMIT] Rate limit warning! Requests are being throttled by Discord.";
                }
                else if (cleanedMessage.empty() && log.message.find("error") != string::npos)
                {
                    cleanedMessage = "[ERROR] Error detected: " + log.message;
                }

                //miscellaneous events
                else if (cleanedMessage.empty() && log.message.find("TYPING_START") != string::npos)
                {
                    cleanedMessage = "[EVENT] User started typing in a channel.";
                }
                else if (cleanedMessage.empty() && log.message.find("MESSAGE_CREATE") != string::npos)
                {
                    cleanedMessage = "[EVENT] New message created in a channel.";
                }
                else if (cleanedMessage.empty() && log.message.find("MESSAGE_UPDATE") != string::npos)
                {
                    cleanedMessage = "[EVENT] Message edited in a channel.";
                }
                else if (cleanedMessage.empty() && log.message.find("MESSAGE_DELETE") != string::npos)
                {
                    cleanedMessage = "[EVENT] Message deleted in a channel.";
                }
                else if (cleanedMessage.empty() && log.message.find("MESSAGE_REACTION_ADD") != string::npos)
                {
                    cleanedMessage = "[EVENT] Reaction added to a message.";
                }
                else if (cleanedMessage.empty() && log.message.find("MESSAGE_REACTION_REMOVE") != string::npos)
                {
                    cleanedMessage = "[EVENT] Reaction removed from a message.";
                }

                if (cleanedMessage.empty())
                {
                    //vor other messages, trim JSON-like characters
                    cleanedMessage = log.message;
                    cleanedMessage.erase(remove_if(cleanedMessage.begin(), cleanedMessage.end(), [](char c)
                        {
                            return c == '{'
                                || c == '}'
                                || c == '\\'
                                || c == '['
                                || c == ']';
                        }), cleanedMessage.end());
                }

                BotGUI::Print(cleanedMessage);
            });

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