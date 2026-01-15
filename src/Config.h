#pragma once
#include <string>
#include <json.hpp>

struct WindowConfig
{
	std::string title;
	int width;
	int height;
	bool fullScreen;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WindowConfig, title, height, fullScreen)
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