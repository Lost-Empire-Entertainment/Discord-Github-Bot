//Copyright(C) 2024 Lost Empire Entertainment

#pragma once

#include "glm.hpp"
#include "glfw3.h"

namespace Graphics
{
	using glm::vec3;

	class Render
	{
	public:
		static inline GLFWwindow* window;
		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

		static void RenderInitialize();
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();

		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);

		static void Run();
	};
}