//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

namespace Graphics::GUI
{
	class BotGUI
	{
	public:
		static inline bool isImguiInitialized;

		static void Initialize();
		static void Run();

		static int GetScreenHeight();
		static int GetScreenWidth();

		static void RenderParentWindow();
		static void RenderWindowContent();

		static void Shutdown();
	};
}