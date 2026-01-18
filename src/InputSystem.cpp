#include "InputSystem.h"
#include "SDL_keyboard.h"

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	return mCurrState[keyCode];
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

InputSystem::InputSystem(Game* game)
	:mGame(game)
{

}

InputSystem::~InputSystem()
{

}

bool InputSystem::Initialize()
{
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);
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
}

void InputSystem::Update()
{

}