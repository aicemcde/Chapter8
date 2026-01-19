#include "InputSystem.h"
#include "SDL_keyboard.h"
#include "Log.h"

const int CONTROLLER_1D_DEADZONE = 250;
const int CONTROLLER_1D_MAXVALUE = 30000;

const float CONTROLLER_2D_DEADZONE = 8000.0f;
const float CONTROLLER_2D_MAXVALUE = 30000.0f;

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
	,mController(nullptr)
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
	
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			if (!controller)
			{
				LOG_WARN("Game Controller {} could not open: {}", i, SDL_GetError());
			}
			else
			{
				SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
				SDL_JoystickID joystickID = SDL_JoystickInstanceID(joystick);
				int newPlayerID = static_cast<float>(mControllers.size());
				mJoystickIDs[joystickID] = newPlayerID;
			}
			mControllers.emplace_back(controller);
		}
	}
	
	mState.Controller.mIsConnected = !mControllers.empty();
	if (mState.Controller.mIsConnected)
	{
		mController = mControllers[0];
		Log::Info("Game controller is connected");
	}
	else
	{
		LOG_INFO("Game controller is not connected");
	}
	memset(mState.Controller.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
	memset(mState.Controller.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

	LOG_INFO("InputSystem complete to initialize");
	return true;
}

void InputSystem::Shutdown()
{
	for (SDL_GameController* controller : mControllers)
	{
		SDL_GameControllerClose(controller);
	}
	mControllers.clear();
	mJoystickIDs.clear();
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

	if (!mController)
	{
		return;
	}
	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
	{
		mState.Controller.mCurrButtons[i] = SDL_GameControllerGetButton(mController, SDL_GameControllerButton(i));
	}

	mState.Controller.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
	mState.Controller.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

	x = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTX);
	y = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTY);
	mState.Controller.mLeftStick = Filter2D(x, y);

	x = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_RIGHTX);
	y = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_RIGHTY);
	mState.Controller.mRightStick = Filter2D(x, y);

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
	case SDL_CONTROLLERBUTTONDOWN: {
		SDL_JoystickID id = event.cbutton.which;
		SDL_Joystick* currJoystick = SDL_GameControllerGetJoystick(mController);
		if (currJoystick && SDL_JoystickInstanceID(currJoystick) != id)
		{
			mController = SDL_GameControllerFromInstanceID(id);
			LOG_INFO("Game controller is changed");
		}
		break;
	}

	case SDL_CONTROLLERDEVICEADDED: {
		int deviceIndex = event.cdevice.which;
		SDL_GameController* newController = SDL_GameControllerOpen(deviceIndex);
		if (newController)
		{
			SDL_Joystick* joystick = SDL_GameControllerGetJoystick(newController);
			SDL_JoystickID joystickID = SDL_JoystickInstanceID(joystick);
			int newPlayerID = mControllers.size();
			mJoystickIDs[joystickID] = newPlayerID;
		}
		break;
	}
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

float InputSystem::Filter1D(int input)
{
	float retVal = 0.0f;

	int absValue = input > 0 ? input : -input;

	if (absValue > CONTROLLER_1D_DEADZONE)
	{
		retVal = static_cast<float>(absValue - CONTROLLER_1D_DEADZONE) / (CONTROLLER_1D_MAXVALUE - CONTROLLER_1D_DEADZONE);
		retVal = input > 0 ? retVal : retVal * -1.0f;
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}
	return retVal;
}

const Vector2& InputSystem::Filter2D(int inputX, int inputY)
{
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	if (length < CONTROLLER_2D_DEADZONE)
	{
		dir = Vector2::Zero;
	}
	else
	{
		float f = (length - CONTROLLER_2D_DEADZONE) / (CONTROLLER_2D_MAXVALUE / CONTROLLER_2D_DEADZONE);
		f = Math::Clamp(f, 0.0f, 1.0f);
		dir *= f / length;
	}
	return dir;
}