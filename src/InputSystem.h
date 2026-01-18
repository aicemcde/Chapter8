#pragma once
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>
#include <SDL_mouse.h>
#include "Math.h"

enum ButtonState
{
	ENone,
	EPressed,
	EReleased,
	EHeld
};

class KeyboardState
{
public:
	friend class InputSystem;

	bool GetKeyValue(SDL_Scancode keyCode) const;

	ButtonState GetKeyState(SDL_Scancode keyCode) const;
private:
	const Uint8* mCurrState;
	Uint8 mPrevState[SDL_NUM_SCANCODES];
};

struct InputState
{
	KeyboardState Keyboard;
};

class InputSystem
{
public:
	InputSystem(class Game* game);
	~InputSystem();

	bool Initialize();
	void Shutdown();

	void PrepareForUpdate();
	void Update();

	const InputState& GetState() const { return mState; }
	
private:
	InputState mState;
	class Game* mGame;
};