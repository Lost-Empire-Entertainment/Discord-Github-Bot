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

		static void Initialize();
		static void Run();

		static int GetScreenHeight();
		static int GetScreenWidth();

		static void RenderParentWindow();
		static void RenderConsole(ImVec2 windowSize);

		enum class MessageTarget
		{
			both,
			cmdOnly,
			consoleOnly
		};
		static void Print(const string& message, MessageTarget messageTarget = MessageTarget::both, int newLineCount = 0);

		static void Shutdown();
	private:
		static inline bool firstScrollToBottom;
		static inline vector<string> output;
		static inline vector<string> storedLogs;
	};
}