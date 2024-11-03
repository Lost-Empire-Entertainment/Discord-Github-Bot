//Copyright(C) 2024 Lost Empire Entertainment

//used for dotenv.h, safe functions are still used in Bot::GetEnv function
#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <Windows.h>
#include <TlHelp32.h>
#include <ShlObj.h>
#include <iostream>
#include <filesystem>

//external
#include "dotenv.h"
#include "glfw3.h"

//bot
#include "core.hpp"
#include "render.hpp"
#include "gui.hpp"
#include "console.hpp"
#include "fileutils.hpp"
#include "stringutils.hpp"
#include "configfile.hpp"

using std::cout;
using std::wstring;
using std::filesystem::exists;
using std::filesystem::current_path;

using Graphics::Render;
using Graphics::GUI::BotGUI;
using Graphics::GUI::Console;
using Utils::String;
using Utils::File;

namespace Core
{
	void Bot::Initialize()
	{
		//initialize dotenv
		dotenv::init();

		exeName = GetEnv("BOT_EXE_NAME");

		if (IsThisProcessAlreadyRunning(exeName + ".exe"))
		{
			CreateErrorPopup((exeName + " is already running!").c_str());
		}

		cout << "Copyright (C) Lost Empire Entertainment 2024\n\n";

		cout << "Initializing Bot...\n";

		//
		// SET DOCUMENTS PATH
		//

		PWSTR docsFolderWidePath;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &docsFolderWidePath);
		if (SUCCEEDED(result))
		{
			wstring wPath(docsFolderWidePath);
			CoTaskMemFree(docsFolderWidePath); //free the allocated memory

			//get the required buffer size
			int size_needed = WideCharToMultiByte(
				CP_UTF8,
				0,
				wPath.c_str(),
				static_cast<int>(wPath.length()),
				NULL,
				0,
				NULL,
				NULL);

			//convert wide string to utf-8 encoded narrow string
			string narrowPath(size_needed, 0);
			WideCharToMultiByte(
				CP_UTF8,
				0,
				wPath.c_str(),
				static_cast<int>(wPath.length()),
				&narrowPath[0],
				size_needed,
				NULL,
				NULL);

			docsPath = String::CharReplace(
				string(narrowPath.begin(), narrowPath.end()), '/', '\\') +
				"\\Compiler";

			if (!exists(docsPath)) File::CreateNewFolder(docsPath);

			cout << "Compiler documents path: " << docsPath << "\n";

			filesPath = current_path().generic_string() + "\\files";
			filesPath = String::CharReplace(filesPath, '/', '\\');
			if (!exists(filesPath))
			{
				CreateErrorPopup("Couldn't find files folder!");
				return;
			}

			cout << "Files path: " << filesPath << "\n";

			configFilePath = docsPath + "\\config.txt";

			ConfigFile::LoadData();
		}
		else
		{
			CreateErrorPopup("Couldn't find engine documents folder!");
		}

		Render::RenderInitialize();
	}

	string Bot::GetEnv(const string& envVariable)
	{
		char* envValue{};
		size_t size{};

		if (_dupenv_s(&envValue, &size, envVariable.c_str()) == 0) 
		{
			if (envValue) 
			{
				string result(envValue);
				free(envValue);
				return result;
			}
			else 
			{
				cout << "Environment variable not found." << std::endl;
				return "";
			}
		}
		else 
		{
			cout << "Error retrieving environment variable." << std::endl;
			return "";
		}
	}

	bool Bot::IsThisProcessAlreadyRunning(const string& processName)
	{
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32{};
		bool processFound = false;
		DWORD currentProcessId = GetCurrentProcessId();

		//take a snapshot of all processes
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			cout << "Error: CreateToolhelp32Snapshot failed!\n";
			return false;
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32))
		{
			cout << "Error: Process32First failed!\n";
			CloseHandle(hProcessSnap);
			return false;
		}

		do
		{
			//compare the current process name with the one to check
			if (strcmp(pe32.szExeFile, processName.c_str()) == 0)
			{
				//check if this is not the current process
				if (pe32.th32ProcessID != currentProcessId)
				{
					processFound = true;
					break;
				}
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return processFound;
	}

	void Bot::CreateErrorPopup(const char* errorMessage)
	{
		string title = exeName + " has shut down";

		cout << "\n"
			<< "===================="
			<< "\n"
			<< "BOT SHUTDOWN\n"
			<< "\n\n"
			<< errorMessage
			<< "\n"
			<< "===================="
			<< "\n";

		int result = MessageBoxA(nullptr, errorMessage, title.c_str(), MB_ICONERROR | MB_OK);

		if (result == IDOK) Shutdown(true);
	}

	void Bot::Run()
	{
		cout << "Running bot...\n";

		isBotRunning = true;

		while (isBotRunning)
		{
			Render::Run();

			// Check if the window should close (e.g., user closed the window)
			if (glfwWindowShouldClose(Render::window))
			{
				isBotRunning = false;
			}
		}
	}

	//counts as idle if minimized
	bool Bot::IsUserIdle()
	{
		int width, height;
		glfwGetWindowSize(Render::window, &width, &height);
		if (width == 0 || height == 0) return true;

		return false;
	}

	void Bot::Shutdown(bool immediate)
	{
		if (immediate)
		{
			isBotRunning = false;

			BotGUI::Shutdown();

			glfwTerminate();
		}
		else
		{
			isBotRunning = false;

			ConfigFile::SaveData();

			BotGUI::Shutdown();

			//clean all glfw resources after program is closed
			glfwTerminate();

			cout << "\n==================================================\n"
				<< "\n"
				<< "EXITED BOT\n"
				<< "\n"
				<< "==================================================\n"
				<< ".\n"
				<< ".\n"
				<< ".\n\n";
		}
	}
}