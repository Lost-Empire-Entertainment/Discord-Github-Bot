//Copyright(C) 2024 Lost Empire Entertainment

#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>

#include "imgui.h"

#include "configfile.hpp"
#include "core.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"

using std::ifstream;
using std::ofstream;
using std::filesystem::exists;
using std::cout;
using std::map;
using std::ios;

using Utils::File;
using Utils::String;

namespace Core
{
	void ConfigFile::SaveData()
	{
		if (!exists(Bot::configFilePath))
		{
			CreateNewConfigFile();
		}
		else
		{
			File::DeleteFileOrfolder(Bot::configFilePath);
		}

		string configFilePath = Bot::docsPath + "\\config.txt";
		ofstream configFile(configFilePath);

		if (!configFile.is_open())
		{
			cout << "Couldn't write into config file '" << Bot::configFilePath << "'!\n";
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			configFile << key << "= " << value << "\n";
		}

		configFile.close();

		cout << "Successfully saved config file!\n";
	}

	void ConfigFile::LoadData()
	{
		if (!exists(Bot::configFilePath)) CreateNewConfigFile();
		else
		{
			//opens file in binary mode and moves to end
			//if file is empty then a new one is created
			ifstream configFileBinary(Bot::configFilePath, ios::binary | ios::ate);
			if (configFileBinary.tellg() == 0)
			{
				configFileBinary.close();

				cout << "Failed to load config file '" << Bot::configFilePath
					<< "' because it was empty! Creating a new one.\n\n";

				CreateNewConfigFile();
				return;
			}

			ifstream configFile(Bot::configFilePath);
			if (!configFile.is_open())
			{
				cout << "Failed to open config file '" << Bot::configFilePath << "'!\n\n";
				return;
			}

			keys.clear();
			values.clear();

			string line;
			map<string, string> configValue;
			while (getline(configFile, line))
			{
				if (!line.empty()
					&& line.find("=") != string::npos)
				{
					vector<string> splitLine = String::Split(line, '=');
					string key = splitLine[0];
					string value = splitLine[1];

					//remove one space in front of value if it exists
					if (value[0] == ' ') value.erase(0, 1);
					//remove one space in front of each value comma if it exists
					for (size_t i = 0; i < value.length(); i++)
					{
						if (value[i] == ','
							&& i + 1 < value.length()
							&& value[i + 1] == ' ')
						{
							value.erase(i + 1, 1);
						}
					}

					keys.push_back(key);
					values.push_back(value);
				}
			}

			configFile.close();

			cout << "Successfully loaded config file!\n";
		}
	}

	void ConfigFile::CreateNewConfigFile()
	{
		keys.clear();
		values.clear();

		keys.push_back("fontScale");
		values.push_back("1.5");

		string configFilePath = Bot::docsPath + "\\config.txt";
		ofstream configFile(configFilePath);

		if (!configFile.is_open())
		{
			cout << "Couldn't write into config file '" << Bot::configFilePath << "'!\n";
			return;
		}

		for (int i = 0; i < keys.size(); i++)
		{
			string key = keys[i];
			string value = values[i];

			configFile << key << "= " << value << "\n";
		}

		configFile.close();

		cout << "Successfully created new config file!\n";
	}

	string ConfigFile::GetValue(const string& key)
	{
		int keyIndex = -1;
		for (int i = 0; i < keys.size(); i++)
		{
			if (keys[i] == key)
			{
				keyIndex = i;
				break;
			}
		}

		if (keyIndex != -1)
		{
			return values[keyIndex];
		}
		else
		{
			cout << "Cannot get config key " << key << " value because it does not exist! This will cause a crash if the config file was not filled correctly.\n";

			return "";
		}
	}

	void ConfigFile::SetValue(const string& key, const string& value)
	{
		int keyIndex = -1;
		for (int i = 0; i < keys.size(); i++)
		{
			if (keys[i] == key)
			{
				keyIndex = i;
				break;
			}
		}

		if (keyIndex != -1)
		{
			values[keyIndex] = value;

			if (key == "fontScale")
			{
				ImGui::GetIO().FontGlobalScale = stof(value);
			}
		}
		else
		{
			cout << "Cannot set value to config key " << key << " because it does not exist!\n";
		}
	}
}