//Copyright(C) 2024 Lost Empire Entertainment

#include <iostream>
#include <filesystem>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glfw3.h"
#pragma warning(push, 0) //start dpp warning ignore
#include "dpp/include/dpp.h"
#pragma warning(pop) //end dpp warning ignore

//bot
#include "gui.hpp"
#include "core.hpp"
#include "render.hpp"
#include "configfile.hpp"
#include "bot.hpp"

using std::cout;
using std::to_string;

using Core::Bot;
using Graphics::Render;
using Core::ConfigFile;
using Core::BotMechanics;

namespace Graphics::GUI
{
	static ImVec4 bgrColor;

	void BotGUI::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = Bot::docsPath + "\\imgui.ini";
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Bot::filesPath + "\\fonts\\coda\\Coda-Regular.ttf").c_str(), 16.0f);

		bgrColor.x = Render::backgroundColor.x;
		bgrColor.y = Render::backgroundColor.y;
		bgrColor.z = Render::backgroundColor.z;
		bgrColor.w = 1.0f;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		io.FontGlobalScale = stof(ConfigFile::GetValue("fontScale"));

		isImguiInitialized = true;
	}

	void BotGUI::Run()
	{
		if (isImguiInitialized)
		{
			if (!Bot::IsUserIdle())
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				ImGuiDockNodeFlags dockFlags =
					ImGuiDockNodeFlags_PassthruCentralNode;

				ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

				RenderParentWindow();

				ImGui::Render();
			}

			ImDrawData* drawData = ImGui::GetDrawData();
			if (drawData != nullptr)
			{
				ImGui_ImplOpenGL3_RenderDrawData(drawData);
			}
		}
	}

	int BotGUI::GetScreenWidth()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return width;
	}

	int BotGUI::GetScreenHeight()
	{
		int width, height;
		glfwGetFramebufferSize(Render::window, &width, &height);
		return height;
	}

	void BotGUI::RenderParentWindow()
	{
		int width = GetScreenWidth();
		int height = GetScreenHeight();
		ImGui::SetNextWindowSize(ImVec2(
			static_cast<float>(width),
			static_cast<float>(height)),
			ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoSavedSettings;

		if (ImGui::Begin("##Main", NULL, windowFlags))
		{
			//left panel takes full height and 55% width
			ImVec2 leftChildSize = ImVec2(width * 0.55f, height - 20.0f);
			RenderConsole(leftChildSize);

			ImGui::SameLine();

			//top right panel takes 40% height and 45% width - 25.0f margin for right edge
			ImVec2 userListSize = ImVec2(width * 0.45f - 25.0f, height * 0.4f);
			ListUsers(userListSize);

			//bottom right panel takes 60% height and 45% width - 25.0f margin for right edge
			ImGui::SetCursorPos(ImVec2(width * 0.55f + 15.0f, userListSize.y + 15.0f));
			ImVec2 rightChildSize = ImVec2(width * 0.45f - 27.0f, height * 0.6f - 27.0f);
			RenderRightSideInteractions(rightChildSize);

			ImGui::End();
		}
	}

	void BotGUI::RenderConsole(ImVec2 windowSize)
	{
		ImVec2 scrollingRegionSize(
			windowSize.x,
			windowSize.y);
		if (ImGui::BeginChild("left", scrollingRegionSize, true))
		{
			float wrapWidth = windowSize.x - 10;
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);

			//display the content of the text buffer
			for (const auto& message : output)
			{
				ImGui::TextWrapped("%s", message.c_str());

				if (ImGui::IsItemClicked()
					&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					Print("---- Added '" + message + "' to clipboard.", MessageTarget::cmdOnly);
					ImGui::SetClipboardText(message.c_str());
				}
			}

			ImGui::PopTextWrapPos();

			//scrolls to the bottom if scrolling is allowed
			//and if user is close to the newest compilation message
			bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10.0f;
			if (isNearBottom
				|| (!firstScrollToBottom))
			{
				ImGui::SetScrollHereY(1.0f);
				firstScrollToBottom = true;
			}
		}

		ImGui::EndChild();
	}

	void BotGUI::RenderRightSideInteractions(ImVec2 windowSize)
	{
		ImVec2 scrollingRegionSize(
			windowSize.x,
			windowSize.y);
		if (ImGui::BeginChild("right", scrollingRegionSize, true))
		{
			if (ImGui::Button("linux sux"))
			{
				string channelID = "1264625286054412350";
				BotMechanics::SendDiscordMessage(channelID, "linux sux");
			}
		}

		ImGui::EndChild();
	}

	void BotGUI::ListUsers(ImVec2 windowSize)
	{
		ImVec2 scrollingRegionSize(windowSize.x, windowSize.y);
		if (ImGui::BeginChild("users", scrollingRegionSize, true))
		{
			if (ImGui::CollapsingHeader("Users", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (const auto& [userID, userName] : BotMechanics::userMap)
				{
					ImGui::BulletText("%s", userName.c_str());

					if (ImGui::BeginPopupContextItem(userID.c_str()))
					{
						if (ImGui::MenuItem("DM"))
						{
							BotGUI::Print("[ADMIN ACTION] DMed " + userName);
						}
						if (ImGui::MenuItem("Message in channel"))
						{
							BotGUI::Print("[ADMIN ACTION] Server-messaged " + userName);
						}
						if (ImGui::MenuItem("Mute"))
						{
							BotGUI::Print("[ADMIN ACTION] Muted " + userName);
						}
						if (ImGui::MenuItem("Kick"))
						{
							BotGUI::Print("[ADMIN ACTION] Kicked " + userName);
						}
						if (ImGui::MenuItem("Ban"))
						{
							BotGUI::Print("[ADMIN ACTION] Banned " + userName);
						}
						ImGui::EndPopup();
					}
				}
			}
		}
		ImGui::EndChild();
	}

	void BotGUI::Print(
		const string& message, 
		MessageTarget messageTarget, 
		int newLineCount,
		bool logInServer,
		string logChannelID)
	{
		//prints to external cmd console
		if (messageTarget == MessageTarget::both
			|| messageTarget == MessageTarget::cmdOnly)
		{
			cout << message << "\n";
		}

		//prints to executable gui console
		if (messageTarget == MessageTarget::both
			|| messageTarget == MessageTarget::consoleOnly)
		{
			output.emplace_back(message);
		}

		//prints new lines to cmd and console
		if (newLineCount != 0)
		{
			for (int i = 0; i < newLineCount; ++i)
			{
				if (messageTarget == MessageTarget::both
					|| messageTarget == MessageTarget::cmdOnly)
				{
					cout << "\n";
				}

				if (messageTarget == MessageTarget::both
					|| messageTarget == MessageTarget::consoleOnly)
				{
					output.emplace_back("");
				}
			}
		}

		//should this message also be printed in the server?
		if (logInServer)
		{
			if (logChannelID == "") Print("Error: Cannot log in channel if no valid channel ID has been assigned!");
			else BotMechanics::SendDiscordMessage(logChannelID, message);
		}
	}

	void BotGUI::Shutdown()
	{
		isImguiInitialized = false;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}