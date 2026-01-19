#include "InputSystem.h"
#include "SDL_keyboard.h"
#include "Log.h"

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if (mCurrState[keyCode] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool MouseState::GetButtonValue(int button) const
{
	return (SDL_BUTTON(button) & mCurrButtons);
}

ButtonState MouseState::GetButtonState(int button) const
{
	Uint32 mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mCurrButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mCurrButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

InputSystem::InputSystem(Game* game)
	: mGame(game)
	, mController(nullptr)
{

}

InputSystem::~InputSystem()
{

}

bool InputSystem::Initialize()
{
	LOG_INFO("InputSystem start to initialize");

	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);
	mController = SDL_GameControllerOpen(0);
	if (!mController)
	{
		LOG_WARN("Game Controller could not open: {}", SDL_GetError());
	}
	mState.Controller.mIsConnected = (mController != nullptr);
	memset(mState.Controller.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
	memset(mState.Controller.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

	LOG_INFO("InputSystem complete to initialize");
	return true;
}

void InputSystem::Shutdown()
{

}

void InputSystem::PrepareForUpdate()
{
	memcpy(
		mState.Keyboard.mPrevState,
		mState.Keyboard.mCurrState,
		SDL_NUM_SCANCODES
	);
	memcpy(mState.Controller.mPrevButtons, mState.Controller.mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);

	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;

	mState.Mouse.mScrollWheel = Vector2::Zero;
}

void InputSystem::Update()
{
	int x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mState.Mouse.mCurrButtons = SDL_GetMouseState(&x, &y);
	}
	mState.Mouse.mMousePos.x = static_cast<float>(x);
	mState.Mouse.mMousePos.y = static_cast<float>(y);

	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
	{
		mState.Controller.mCurrButtons[i] = SDL_GameControllerGetButton(mController, SDL_GameControllerButton(i));
	}
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		mState.Mouse.mScrollWheel = Vector2
		(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y)
		);
		break;
	default:
		break;
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);
	mState.Mouse.mIsRelative = value;
}