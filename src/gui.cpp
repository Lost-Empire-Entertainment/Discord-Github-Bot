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
#include "magic_enum.hpp"

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

	ImVec2 BotGUI::CenterWindow(ImVec2 size)
	{
		int intWidth, intHeight;
		glfwGetFramebufferSize(Render::window, &intWidth, &intHeight);

		float glfwWindowWidth = static_cast<float>(intWidth);
		float glfwWindowHeight = static_cast<float>(intHeight);

		float posX = (glfwWindowWidth - size.x) / 2.0f;
		float posY = (glfwWindowHeight - size.y) / 2.0f;

		return ImVec2(posX, posY);
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
			UserActions(userListSize);

			//bottom right panel takes 60% height and 45% width - 25.0f margin for right edge
			ImGui::SetCursorPos(ImVec2(width * 0.55f + 15.0f, userListSize.y + 15.0f));
			ImVec2 rightChildSize = ImVec2(width * 0.45f - 27.0f, height * 0.6f - 27.0f);
			RenderRightSideInteractions(rightChildSize);

			ImGui::End();
		}

		if (renderBotAdminActionWindow) BotAdminActionWindow();
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

	void BotGUI::UserActions(ImVec2 windowSize)
	{
		ImVec2 scrollingRegionSize(windowSize.x, windowSize.y);
		if (ImGui::BeginChild("userActions", scrollingRegionSize, true))
		{
			ImGui::Text("Search for user by ID");

			ImGui::InputText(
				"##username",
				usernameTextBuffer,
				sizeof(usernameTextBuffer));

			if (ImGui::Button("Search"))
			{
				if (usernameTextBuffer[0] == '\0')
				{
					Print("Error: User search field cannot be empty!");
				}
				else
				{
					if (BotMechanics::UserExists(usernameTextBuffer))
					{
						targetUserID = usernameTextBuffer;
					}
				}
			}

			ImGui::Separator();
			ImGui::Text("Actions");

			if (ImGui::Button("DM"))
			{
				botAction = BotAction::dm;
				renderBotAdminActionWindow = true;
			}
			if (ImGui::Button("Message in channel"))
			{
				botAction = BotAction::message;
				renderBotAdminActionWindow = true;
			}
			if (ImGui::Button("Mute"))
			{
				botAction = BotAction::mute;
				renderBotAdminActionWindow = true;
			}
			if (ImGui::Button("Kick"))
			{
				botAction = BotAction::kick;
				renderBotAdminActionWindow = true;
			}
			if (ImGui::Button("Ban"))
			{
				botAction = BotAction::ban;
				renderBotAdminActionWindow = true;
			}
		}
		ImGui::EndChild();
	}

	void BotGUI::BotAdminActionWindow()
	{
		ImVec2 windowSize = ImVec2(500.0f, 300.0f);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);

		ImVec2 windowPos = CenterWindow(windowSize);
		ImGui::SetNextWindowPos(ImVec2(windowPos), ImGuiCond_Appearing);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings;

		string action = string(magic_enum::enum_name(botAction));
		string title = "Bot action: " + action;
		if (ImGui::Begin(title.c_str()), nullptr, flags)
		{
			switch (botAction)
			{
			case BotAction::dm:
				ImGui::InputText(
					"Message",
					messageTextBuffer,
					sizeof(messageTextBuffer));

				break;
			case BotAction::message:
				break;
			case BotAction::mute:
				break;
			case BotAction::kick:
				break;
			case BotAction::ban:
				break;
			}

			if (ImGui::Button("Accept"))
			{
				switch (botAction)
				{
				case BotAction::dm:
					if (usernameTextBuffer[0] == '\0'
						|| messageTextBuffer[0] == '\0')
					{
						Print("Error: Username or message field is empty!");
					}
					else
					{
						BotMechanics::BotAction_DMUser(targetUserID, messageTextBuffer);
						BotGUI::Print("[ADMIN ACTION] DMed '" + targetUsername + "' with message '" + messageTextBuffer + "'.");

						messageTextBuffer[0] = '\0';

						renderBotAdminActionWindow = false;
					}
					break;
				case BotAction::message:
					if (usernameTextBuffer[0] == '\0'
						|| messageTextBuffer[0] == '\0')
					{
						Print("Error: Username or message field is empty!");
					}
					else
					{
						BotMechanics::BotAction_DMUser(targetUserID, messageTextBuffer);
						BotGUI::Print("[ADMIN ACTION] Server-messaged '" + targetUsername + "' with message '" + messageTextBuffer + "'.");

						messageTextBuffer[0] = '\0';

						renderBotAdminActionWindow = false;
					}
					break;
				case BotAction::mute:
					if (usernameTextBuffer[0] == '\0')
					{
						Print("Error: Username is empty!");
					}
					else
					{
						BotGUI::Print("[ADMIN ACTION] Muted " + targetUsername);

						renderBotAdminActionWindow = false;
					}
					break;
				case BotAction::kick:
					if (usernameTextBuffer[0] == '\0')
					{
						Print("Error: Username is empty!");
					}
					else
					{
						BotGUI::Print("[ADMIN ACTION] Kicked " + targetUsername);

						renderBotAdminActionWindow = false;
					}
					break;
				case BotAction::ban:
					if (usernameTextBuffer[0] == '\0')
					{
						Print("Error: Username is empty!");
					}
					else
					{
						BotGUI::Print("[ADMIN ACTION] Banned " + targetUsername);

						renderBotAdminActionWindow = false;
					}
					break;
				}
			}

			if (ImGui::Button("Close")) renderBotAdminActionWindow = false;

			ImGui::End();
		}
	}

	void BotGUI::Print(
		const string& message, 
		MessageTarget messageTarget,
		bool logInServer,
		string logChannelID)
	{
		string newMessage = message + "\n";

		//prints to external cmd console
		if (messageTarget == MessageTarget::both
			|| messageTarget == MessageTarget::cmdOnly)
		{
			cout << newMessage << "\n";
		}

		//prints to executable gui console
		if (messageTarget == MessageTarget::both
			|| messageTarget == MessageTarget::consoleOnly)
		{
			output.emplace_back(newMessage);
		}

		//should this message also be printed in the server?
		if (logInServer)
		{
			if (logChannelID == "") Print("Error: Cannot log in channel if no valid channel ID has been assigned!");
			else BotMechanics::SendDiscordMessage(logChannelID, newMessage);
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