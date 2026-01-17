#pragma once
#include <string>
#include <json.hpp>
#include "Log.h"
#include <fstream>

struct WindowConfig
{
	std::string title;
	int width;
	int height;
	bool fullScreen;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WindowConfig, title, width, height, fullScreen)
};

struct AudioConfig
{
	float master_volume;
	bool mute;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(AudioConfig, master_volume, mute)
};

struct GameConfig
{
	WindowConfig window;
	AudioConfig audio;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameConfig, window, audio)
};

static bool LoadConfig(const std::string& path, GameConfig& outConfig)
{
	LOG_INFO("Load config: {}", path);

	std::ifstream file(path);
	if (!file.is_open())
	{
		LOG_ERROR("Failed to open config file: {}", path);
		return false;
	}

	try
	{
		nlohmann::json j;
		file >> j;

		outConfig = j.get<GameConfig>();

		LOG_INFO("Config loaded successfully");
		return true;
	}
	catch (const nlohmann::json::exception& e)
	{
		LOG_CRITICAL("JSON parsing Error: {}", e.what());
		return false;
	}
}