//Copyright(C) 2024 Lost Empire Entertainment

#include <iostream>
#include <string>

//external
#include "stb_image.h"
#include "glad.h"

//bot
#include "render.hpp"
#include "core.hpp"
#include "gui.hpp"

using std::cout;
using std::string;

using Core::Bot;
using Graphics::GUI::BotGUI;

namespace Graphics
{
	void Render::RenderInitialize()
	{
		GLFWSetup();
		WindowSetup();
		GladSetup();

		BotGUI::Initialize();
	}

	void Render::GLFWSetup()
	{
		BotGUI::Print("Initializing GLFW...", BotGUI::MessageTarget::cmdOnly);

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		BotGUI::Print("GLFW initialized successfully!", BotGUI::MessageTarget::cmdOnly, 1);
	}

	void Render::WindowSetup()
	{
		BotGUI::Print("Creating window...", BotGUI::MessageTarget::cmdOnly);

		//create a window object holding all the windowing data
		string name = Bot::exeName + " " + Bot::version;
		window = glfwCreateWindow(
			1000,
			1000,
			name.c_str(),
			NULL,
			NULL);

		if (window == NULL) Bot::CreateErrorPopup("Failed to create GLFW window!");

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);
		glfwSetWindowSizeLimits(window, 750, 750, 1500, 1500);
		glfwSwapInterval(1);

		int width, height, channels;
		string iconpath = Bot::filesPath + "\\icon.png";
		unsigned char* iconData = stbi_load(iconpath.c_str(), &width, &height, &channels, 4);

		GLFWimage icon{};
		icon.width = width;
		icon.height = height;
		icon.pixels = iconData;

		glfwSetWindowIcon(window, 1, &icon);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { Bot::Shutdown(); });

		BotGUI::Print("Window initialized successfully!", BotGUI::MessageTarget::cmdOnly, 1);
	}

	void Render::GladSetup()
	{
		BotGUI::Print("Initializing GLAD...", BotGUI::MessageTarget::cmdOnly);

		//check if glad is initialized before continuing
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Bot::CreateErrorPopup("Failed to initialize GLAD!");
		}

		BotGUI::Print("GLAD initialized successfully!", BotGUI::MessageTarget::cmdOnly, 1);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		//Set the viewport based on the aspect ratio
		glViewport(0, 0, width, height);
	}

	void Render::Run()
	{
		glClearColor(
			backgroundColor.x,
			backgroundColor.y,
			backgroundColor.z,
			1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		BotGUI::Run();

		//swap the front and back buffers
		glfwSwapBuffers(window);
		if (!Bot::IsUserIdle())
		{
			glfwPollEvents();
		}
		else glfwWaitEvents();
	}
}