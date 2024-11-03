//Copyright(C) 2024 Lost Empire Entertainment

#include <iostream>
#include <filesystem>
#include <string>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glfw3.h"

//bot
#include "gui.hpp"
#include "core.hpp"
#include "render.hpp"
#include "configfile.hpp"

using std::string;
using std::cout;

using Core::Bot;
using Graphics::Render;
using Core::ConfigFile;

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
			RenderWindowContent();
		}
	}

	void BotGUI::RenderWindowContent()
	{
		if (ImGui::Button("123"))
		{
			cout << "456";
		}

		ImGui::End();
	}

	void BotGUI::Shutdown()
	{
		isImguiInitialized = false;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}