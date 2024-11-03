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
		cout << "Initializing GLFW...\n";

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		cout << "GLFW initialized successfully!\n\n";
	}

	void Render::WindowSetup()
	{
		cout << "Creating window...\n";

		//create a window object holding all the windowing data
		string name = "Compiler | Elypso hub";
		window = glfwCreateWindow(
			1000,
			1000,
			name.c_str(),
			NULL,
			NULL);

		if (window == NULL)
		{
			cout << "Failed to create GLFW window!\n\n";
			return;
		}

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

		cout << "Window initialized successfully!\n\n";
	}

	void Render::GladSetup()
	{
		cout << "Initializing GLAD...\n";

		//check if glad is initialized before continuing
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << "Failed to initialize GLAD!\n\n";
			return;
		}

		cout << "GLAD initialized successfully!\n\n";
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