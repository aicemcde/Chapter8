#pragma once
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>
#include <SDL_mouse.h>
#include "Math.h"
#include <SDL_events.h>
#include <vector>
#include <map>

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

class MouseState
{
public:
	friend class InputSystem;

	const Vector2& GetPosition() const { return mMousePos; }

	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;

private:
	Vector2 mMousePos;
	Vector2 mScrollWheel;
	Uint32 mCurrButtons = 0;
	Uint32 mPrevButtons = 0;
	bool mIsRelative = false;
};

class ControllerState
{
public:
	friend class InputSystem;

	float GetLeftTrigger() const { return mLeftTrigger; }
	float GetRightTrigger() const { return mRightTrigger; }

	const Vector2& GetLeftStick() const { return mLeftStick; }
	const Vector2& GetRightStick() const { return mRightStick; }
private:
	Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];
	Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];
	bool mIsConnected;
	float mLeftTrigger;
	float mRightTrigger;
	Vector2 mLeftStick;
	Vector2 mRightStick;
};

struct InputState
{
	KeyboardState Keyboard;
	MouseState Mouse;
	ControllerState Controller;
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
	void ProcessEvent(SDL_Event& event);

	const InputState& GetState() const { return mState; }
	
	void SetRelativeMouseMode(bool value);

	float Filter1D(int input);
	const Vector2& Filter2D(int inputX, int inputY);
private:
	InputState mState;
	class Game* mGame;
	SDL_GameController* mController;
	std::vector<SDL_GameController*> mControllers;
	std::map<int, SDL_JoystickID> mJoystickIDs;
};