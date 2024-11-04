//Copyright(C) 2024 Lost Empire Entertainment

#include <memory>
#include <iostream>
#include <thread>
#include <future>

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
using std::stoull;
using std::future;
using std::promise;
using std::to_string;

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

		logChannelID = Bot::GetEnv("DISCORD_LOG_CHANNEL_ID");
		if (logChannelID == "")
		{
			Bot::CreateErrorPopup("Failed to get discord log channel id from env file! Did you forget to add the DISCORD_LOG_CHANNEL_ID variable?");
		}

        botID = Bot::GetEnv("DISCORD_BOT_ID");
        if (botID == "")
        {
            Bot::CreateErrorPopup("Failed to get discord bot id from env file! Did you forget to add the DISCORD_BOT_ID variable?");
        }

        //set up intents to include guilds, channels, members, and emojis
        uint32_t intents =
            dpp::i_default_intents
            | dpp::i_guild_members       //for guild member events
            | dpp::i_guilds              //for guild events
            | dpp::i_guild_emojis        //for emoji events
            | dpp::i_message_content     //for accessing message content
            | dpp::i_guild_voice_states  //for voice state events
            | dpp::i_guild_bans;         //for guild ban events

        bot = make_unique<dpp::cluster>(botToken, intents);

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

        //bot joined server
        bot->on_guild_create([](const dpp::guild_create_t& event)
            {
                GetGuildName(to_string(event.created->id), [event](const string& guildName)
                    {
                        string cleanedMessage = "[GUILD] Bot has joined the guild: " + guildName;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //bot left server
        bot->on_guild_delete([](const dpp::guild_delete_t& event)
            {
                GetGuildName(to_string(event.guild_id), [event](const string& guildName)
                    {
                        string cleanedMessage = "[GUILD] Bot was removed from the guild: " + guildName;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //server was updated
        bot->on_guild_update([](const dpp::guild_update_t& event)
            {
                GetGuildName(to_string(event.updated->id), [event](const string& guildName)
                    {
                        string cleanedMessage = "[GUILD] Guild information updated for: " + guildName;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //channel was created in server
        bot->on_channel_create([](const dpp::channel_create_t& event)
            {
                GetChannelName(to_string(event.created->id), [event](const string& channelName)
                    {
                        string cleanedMessage = "[CHANNEL] New channel created: " + channelName;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //channel was deleted in server
        bot->on_channel_delete([](const dpp::channel_delete_t& event)
            {
                GetChannelName(to_string(event.deleted.id), [event](const string& channelName)
                    {
                        string cleanedMessage = "[CHANNEL] Channel deleted: " + channelName;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //a voice channel was updated in server
        bot->on_voice_server_update([](const dpp::voice_server_update_t& event)
            {
                GetGuildName(to_string(event.guild_id), [event](const string& guildName)
                    {
                        string cleanedMessage = "[CHANNEL] Voice server updated in guild: " + guildName;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //a user's voice state was updated
        bot->on_voice_state_update([](const dpp::voice_state_update_t& event)
            {
                GetUsername(to_string(event.state.user_id), [event](const string& username)
                    {
                        GetChannelName(to_string(event.state.channel_id), [event, username](const string& channelName)
                            {
                                string updateDetails;

                                //user is self-mute or not
                                if (event.state.is_self_mute())
                                {
                                    updateDetails = username + " has self-muted.";
                                }
                                else
                                {
                                    updateDetails = username + " has self-unmuted.";
                                }

                                //user is self-deaf or not
                                if (event.state.is_self_deaf())
                                {
                                    updateDetails += " " + username + " has self-deafened.";
                                }
                                else
                                {
                                    updateDetails += " " + username + " has self-undeafened.";
                                }

                                //user is server-mute or not
                                if (event.state.is_mute())
                                {
                                    updateDetails += " " + username + " is muted by the server.";
                                }
                                else
                                {
                                    updateDetails += " " + username + " is not server-muted.";
                                }

                                //user is server-deaf or not
                                if (event.state.is_deaf())
                                {
                                    updateDetails += " " + username + " is deafened by the server.";
                                }
                                else
                                {
                                    updateDetails += " " + username + " is not server-deafened.";
                                }

                                //user joined or left
                                if (event.state.channel_id == 0)
                                {
                                    updateDetails += " " + username + " has left the voice channel.";
                                }
                                else
                                {
                                    updateDetails += " " + username + " is in the channel: " + channelName;
                                }

                                //user is showing or not showing video
                                if (event.state.self_video())
                                {
                                    updateDetails += " Video is enabled.";
                                }
                                else
                                {
                                    updateDetails += " Video is disabled.";
                                }

                                string cleanedMessage = "[CHANNEL] Voice state updated for user " + username + ". " + updateDetails;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user or bot wrote a message
        bot->on_message_create([](const dpp::message_create_t& event)
            {
                if (event.msg.author.id == stoull(botID)          //ignore this bot's messages
                    || event.msg.author.id == 235148962103951360  //ignore Carl-bot messages
                    || event.msg.author.id == 691713521007984681  //ignore Engauge bot messages
                    || event.msg.author.id == 620126394390675466) //ignore Taco bot messages
                {
                    return;
                }

                //extract necessary data
                string messageID = to_string(event.msg.id);
                string messageContent = event.msg.content;
                string channelID = to_string(event.msg.channel_id);
                string authorID = to_string(event.msg.author.id);

                //store the message ID, content and order
                messageCache[messageID] = messageContent;
                messageOrder.push_back(messageID);

                const size_t MAX_CACHE_SIZE = 10000;
                if (messageCache.size() > MAX_CACHE_SIZE)
                {
                    const string& oldestMessageID = messageOrder.front();
                    messageCache.erase(oldestMessageID);
                    messageOrder.pop_front();
                }

                GetChannelName(channelID, [authorID, messageContent](const string& channelName)
                    {
                        GetUsername(authorID, [channelName, messageContent](const string& username)
                            {
                                string cleanedMessage = "[EVENT] New message created in channel " + channelName + " by " + username +
                                    "\nContent: " + messageContent;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user or bot updated a message
        bot->on_message_update([](const dpp::message_update_t& event)
            {
                string messageID = to_string(event.msg.id);
                string newMessageContent = event.msg.content;
                string channelID = to_string(event.msg.channel_id);
                string authorID = to_string(event.msg.author.id);

                //retrieve the old message content from the cache
                string oldMessageContent = "Unknown (not cached)";
                auto it = messageCache.find(messageID);
                if (it != messageCache.end())
                {
                    oldMessageContent = it->second;
                    //update the cache with the new message content
                    it->second = newMessageContent;
                }
                else
                {
                    //if not in cache, store the new message content
                    messageCache[messageID] = newMessageContent;
                    messageOrder.push_back(messageID);
                    //manage cache size as before
                    const size_t MAX_CACHE_SIZE = 10000; // Adjust as needed
                    if (messageOrder.size() > MAX_CACHE_SIZE)
                    {
                        const string& oldestMessageID = messageOrder.front();
                        messageCache.erase(oldestMessageID);
                        messageOrder.pop_front();
                    }
                }

                GetChannelName(channelID, [authorID, oldMessageContent, newMessageContent](const string& channelName)
                    {
                        GetUsername(authorID, [channelName, oldMessageContent, newMessageContent](const string& username)
                            {
                                string cleanedMessage = "[EVENT] Message edited in channel " + channelName + " by " + username +
                                    "\nOld content: " + oldMessageContent +
                                    "\nNew content: " + newMessageContent;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user or bot deleted a message
        bot->on_message_delete([](const dpp::message_delete_t& event)
            {
                if (event.channel_id == stoull(logChannelID))
                {
                    return;
                }

                string messageID = to_string(event.id);
                string channelID = to_string(event.channel_id);

                //retrieve the message content from the cache
                string messageContent = "Unknown (not cached)";
                //remove from cache and order
                auto it = messageCache.find(messageID);
                if (it != messageCache.end())
                {
                    messageContent = it->second;
                    messageCache.erase(it);
                    messageOrder.remove(messageID);
                }

                GetChannelName(channelID, [messageContent](const string& channelName)
                    {
                        string cleanedMessage = "[EVENT] Message deleted in channel " + channelName +
                            "\nContent: " + messageContent;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //a user or bot added a reaction to a message
        bot->on_message_reaction_add([](const dpp::message_reaction_add_t& event)
            {
                if (event.reacting_user.id == stoull(botID))
                {
                    return;
                }

                GetChannelName(to_string(event.channel_id), [event](const string& channelName)
                    {
                        GetUsername(to_string(event.reacting_user.id), [event, channelName](const string& username)
                            {
                                if (event.reacting_emoji.id != 0) //custom emoji with an ID
                                {
                                    GetReactionName(to_string(event.reacting_emoji.id), to_string(event.reacting_guild->id), [event, channelName, username](const string& emojiName)
                                        {
                                            string cleanedMessage = "[EVENT] Reaction added to a message in channel " + channelName + " by " + username + " with emoji: " + emojiName;
                                            BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                                        });
                                }
                                else //standard Unicode emoji
                                {
                                    string emojiName = event.reacting_emoji.name;
                                    string cleanedMessage = "[EVENT] Reaction added to a message in channel " + channelName + " by " + username + " with emoji: " + emojiName;
                                    BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                                }
                            });
                    });
            });

        //a user or bot removed a reaction from a message
        bot->on_message_reaction_remove([](const dpp::message_reaction_remove_t& event)
            {
                GetChannelName(to_string(event.channel_id), [event](const string& channelName)
                    {
                        GetUsername(to_string(event.reacting_user_id), [event, channelName](const string& username)
                            {
                                if (event.reacting_emoji.id != 0) //custom emoji with an ID
                                {
                                    GetReactionName(to_string(event.reacting_emoji.id), to_string(event.reacting_guild->id), [event, channelName, username](const string& emojiName)
                                        {
                                            string cleanedMessage = "[EVENT] Reaction removed from a message in channel " + channelName + " by " + username + " with emoji: " + emojiName;
                                            BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                                        });
                                }
                                else //standard Unicode emoji
                                {
                                    string emojiName = event.reacting_emoji.name;
                                    string cleanedMessage = "[EVENT] Reaction removed from a message in channel " + channelName + " by " + username + " with emoji: " + emojiName;
                                    BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                                }
                            });
                    });
            });

        //a user or bot started typing
        bot->on_typing_start([](const dpp::typing_start_t& event)
            {
                GetChannelName(to_string(event.typing_channel->id), [event](const string& channelName)
                    {
                        GetUsername(to_string(event.user_id), [event, channelName](const string& username)
                            {
                                string cleanedMessage = "[EVENT] " + username + " started typing in channel " + channelName;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user or bot joined a server
        bot->on_guild_member_add([](const dpp::guild_member_add_t& event)
            {
                GetUsername(to_string(event.added.user_id), [event](const string& username)
                    {
                        GetGuildName(to_string(event.adding_guild->id), [event, username](const string& guildName)
                            {
                                string cleanedMessage = "[MEMBER] " + username + " joined the guild: " + guildName;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user or bot left a server
        bot->on_guild_member_remove([](const dpp::guild_member_remove_t& event)
            {
                GetUsername(to_string(event.removed.id), [event](const string& username)
                    {
                        GetGuildName(to_string(event.removing_guild->id), [event, username](const string& guildName)
                            {
                                string cleanedMessage = "[MEMBER] " + username + " left or was removed from the guild: " + guildName;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user or bot server profile was updated
        bot->on_guild_member_update([](const dpp::guild_member_update_t& event)
            {
                GetUsername(to_string(event.updated.user_id), [event](const string& username)
                    {
                        GetGuildName(to_string(event.updating_guild->id), [event, username](const string& guildName)
                            {
                                string cleanedMessage = "[MEMBER] Server profile updated for user " + username + " in guild: " + guildName;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user or bot global profile was updated
        bot->on_user_update([](const dpp::user_update_t& event)
            {
                GetUsername(to_string(event.updated.id), [event](const string& username)
                    {
                        string cleanedMessage = "[USER] Global profile updated for user " + username;
                        BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                    });
            });

        //a user was banned
        bot->on_guild_ban_add([](const dpp::guild_ban_add_t& event)
            {
                GetUsername(to_string(event.banned.id), [event](const string& username)
                    {
                        GetGuildName(to_string(event.banning_guild->id), [event, username](const string& guildName)
                            {
                                string cleanedMessage = "[BAN] User " + username + " was banned from guild: " + guildName;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
            });

        //a user was unbanned
        bot->on_guild_ban_remove([](const dpp::guild_ban_remove_t& event)
            {
                GetUsername(to_string(event.unbanned.id), [event](const string& username)
                    {
                        GetGuildName(to_string(event.unbanning_guild->id), [event, username](const string& guildName)
                            {
                                string cleanedMessage = "[BAN] User " + username + " was unbanned from guild: " + guildName;
                                BotGUI::Print(cleanedMessage, BotGUI::MessageTarget::serverLogOnly, 0, true, logChannelID);
                            });
                    });
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

    void BotMechanics::GetGuildName(const string& guildID, function<void(const string&)> callback)
    {
        dpp::snowflake dppGuildID = stoull(guildID);
        dpp::guild* guild = dpp::find_guild(dppGuildID);
        if (guild)
        {
            //guild is in cache, invoke callback immediately
            callback(guild->name);
        }
        else
        {
            //fetch guild from API asynchronously
            bot->guild_get(dppGuildID, [callback](const dpp::confirmation_callback_t& event)
                {
                    if (!event.is_error())
                    {
                        dpp::guild guild = get<dpp::guild>(event.value);
                        //the guild should now be in the cache
                        callback(guild.name);
                    }
                    else
                    {
                        callback("Unknown Guild");
                    }
                });
        }
    }

    void BotMechanics::GetChannelName(const string& channelID, function<void(const string&)> callback)
    {
        dpp::snowflake dppChannelID = stoull(channelID);
        dpp::channel* channel = dpp::find_channel(dppChannelID);
        if (channel)
        {
            //channel is in cache, invoke callback immediately
            callback(channel->name);
        }
        else
        {
            //fetch channel from API asynchronously
            bot->channel_get(dppChannelID, [callback](const dpp::confirmation_callback_t& event)
                {
                    if (!event.is_error())
                    {
                        dpp::channel channel = get<dpp::channel>(event.value);
                        //the channel should now be in the cache
                        callback(channel.name);
                    }
                    else
                    {
                        callback("Unknown Channel");
                    }
                });
        }
    }

    void BotMechanics::GetUsername(const string& userID, function<void(const string&)> callback)
    {
        dpp::snowflake dppUserID = stoull(userID);
        dpp::user* user = dpp::find_user(dppUserID);
        if (user)
        {
            //user is in cache, invoke callback immediately
            callback(user->username);
        }
        else
        {
            //fetch user from API asynchronously
            bot->user_get(dppUserID, [callback](const dpp::confirmation_callback_t& event)
                {
                    //inside the lambda, 'callback' is captured and can be used
                    if (!event.is_error())
                    {
                        dpp::user_identified user = get<dpp::user_identified>(event.value);
                        //the user should now be in the cache
                        callback(user.username);
                    }
                    else
                    {
                        callback("Unknown User");
                    }
                });
        }
    }

    void BotMechanics::GetReactionName(const string& reactionID, const string& guildID, function<void(const string&)> callback)
    {
        dpp::snowflake dppReactionID = stoull(reactionID);
        dpp::snowflake dppGuildID = stoull(guildID);

        dpp::guild* guild = dpp::find_guild(dppGuildID);
        if (guild)
        {
            //iterate over the emoji IDs in the guild
            for (const auto& emoji_id : guild->emojis)
            {
                if (emoji_id == dppReactionID)
                {
                    //fetch the emoji object using the ID
                    dpp::emoji* emoji = dpp::find_emoji(emoji_id);
                    if (emoji)
                    {
                        callback(emoji->name);
                    }
                    else
                    {
                        //fetch from API if not in cache
                        bot->guild_emoji_get(dppGuildID, emoji_id, [callback](const dpp::confirmation_callback_t& event)
                            {
                                if (!event.is_error())
                                {
                                    dpp::emoji emoji = get<dpp::emoji>(event.value);
                                    callback(emoji.name);
                                }
                                else
                                {
                                    callback("Unknown Reaction");
                                }
                            });
                    }
                    return;
                }
            }
        }

        //if emoji not found in guild emojis, fetch it from the API
        bot->guild_emoji_get(dppGuildID, dppReactionID, [callback](const dpp::confirmation_callback_t& event)
            {
                if (!event.is_error())
                {
                    dpp::emoji emoji = get<dpp::emoji>(event.value);
                    callback(emoji.name);
                }
                else
                {
                    callback("Unknown Reaction");
                }
            });
    }

	void BotMechanics::SendDiscordMessage(const string& channelID, const string& message)
	{
		dpp::snowflake convertedChannelID = static_cast<dpp::snowflake>(stoull(channelID));
		bot->message_create(dpp::message(convertedChannelID, message));

        BotGUI::Print(message);
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