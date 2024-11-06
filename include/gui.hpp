//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include <vector>
#include <string>

//external
#include "imgui.h"

namespace Graphics::GUI
{
	using std::vector;
	using std::string;

	class BotGUI
	{
	public:
		static inline bool isImguiInitialized;
		static inline bool renderBotAdminActionWindow;

		static void Initialize();
		static void Run();

		static int GetScreenHeight();
		static int GetScreenWidth();
		static ImVec2 CenterWindow(ImVec2 windowSize);

		static void RenderParentWindow();

		static void RenderConsole(ImVec2 windowSize);
		static void RenderDiscordContent(ImVec2 windowSize);
		static void RenderGithubContent(ImVec2 windowSize);

		static inline bool tagUser;
		static inline string targetUserID;
		static inline string targetUsername;

		static inline string targetChannelID;
		static inline string targetChannelName;

		static inline bool tagRole;
		static inline string targetRoleID;
		static inline string targetRoleName;

		enum class BotAction
		{
			dm,
			message,
			mute,
			unmute,
			kick,
			ban,
			unban
		};
		static inline BotAction botAction;
		static void RenderBotActionWindow();

		enum class MessageTarget
		{
			both,
			cmdOnly,
			consoleOnly,
			serverLogOnly
		};
		static void Print(
			const string& message, 
			MessageTarget messageTarget = MessageTarget::both,
			bool logInServer = false,
			string logChannelID = "");

		static void Shutdown();
	private:
		static inline bool firstScrollToBottom;

		static inline char usernameTextBuffer[64];
		static inline char channelTextBuffer[64];
		static inline char roleTextBuffer[64];
		static inline char messageTextBuffer[128];

		static inline vector<string> output;
		static inline vector<string> storedLogs;
	};
}