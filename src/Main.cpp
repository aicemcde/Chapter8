#include "Log.h"

int main()
{
	Log::Init();

	LOG_INFO("--- Game Engine Starting ---");

	int playerHP = 100;
	float x = 10.5f;
	float y = 20.0f;

	LOG_TRACE("Initialization complete");
	LOG_INFO("Player HP: {}", playerHP);
	LOG_WARN("Position warning: ({}, {})", x, y);
	
	bool failedToLoad = true;
	if (failedToLoad) {
		LOG_ERROR("Failed to load texture: assets/hero.png");
	}

	LOG_INFO("--- Game Engine Closing ---");

	return 0;
}