#pragma once
#include <Xinput.h>
#include "Input/InputCoord.h"

#pragma comment(lib, "xinput9_1_0.lib")

class XInput{
public:
	XInput(){
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		for (int i = 0; i < (int)PAD_X_KeyCode::Count; i++){
			m_XState[i] = 0;
		}
		for (int i = 0; i < (int)PAD_X_LevelCode::Count; i++){
			m_XAnalog[i] = 0;
		}

	}

	~XInput(){

	}

	void Read(){
		DWORD dwResult;
		for (DWORD i = 0; i< MAX_CONTROLLERS; i++)
		{

			// Simply get the state of the controller from XInput.
			dwResult = XInputGetState(i, &state);

			if (dwResult == ERROR_SUCCESS)
			{
				// Zero value if thumbsticks are within the dead zone 
				if ((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
					(state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
					state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
				{
					state.Gamepad.sThumbLX = 0;
					state.Gamepad.sThumbLY = 0;
				}




				if ((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
					state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
					(state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
					state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
				{
					state.Gamepad.sThumbRX = 0;
					state.Gamepad.sThumbRY = 0;
				}

				if (state.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
				{
					state.Gamepad.bLeftTrigger = 0;
				}
				if (state.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
				{
					state.Gamepad.bRightTrigger = 0;
				}
			}
			else
			{
				ZeroMemory(&state, sizeof(XINPUT_STATE));
			}
		}


		m_XState[(int)PAD_X_KeyCode::Button_A] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_A?1:0;
		m_XState[(int)PAD_X_KeyCode::Button_B] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_B ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_X] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_X ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_Y] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_Y ? 1 : 0;


		m_XState[(int)PAD_X_KeyCode::Button_UP] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_Up ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_DOWN] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_Down ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_LEFT] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_Left ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_RIGHT] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_Right ? 1 : 0;



		m_XState[(int)PAD_X_KeyCode::Button_START] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_Start ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_BACK] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_Back ? 1 : 0;

		m_XState[(int)PAD_X_KeyCode::Button_R1] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_R1 ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_R2] =
			state.Gamepad.bRightTrigger;
		m_XState[(int)PAD_X_KeyCode::Button_R3] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_R3 ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_L1] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_L1 ? 1 : 0;
		m_XState[(int)PAD_X_KeyCode::Button_L2] =
			state.Gamepad.bLeftTrigger;
		m_XState[(int)PAD_X_KeyCode::Button_L3] =
			state.Gamepad.wButtons & XInput_BitFlag::Button_L3 ? 1 : 0;

		m_XAnalog[(int)PAD_X_LevelCode::Level_R2] =
			state.Gamepad.bRightTrigger / 255.0f;
		m_XAnalog[(int)PAD_X_LevelCode::Level_L2] = 
			state.Gamepad.bLeftTrigger / 255.0f;

		m_XAnalog[(int)PAD_X_LevelCode::Level_R2] =
			state.Gamepad.bRightTrigger / 255.0f;
		m_XAnalog[(int)PAD_X_LevelCode::Level_L2] =
			state.Gamepad.bLeftTrigger / 255.0f;

#define _SHROT_MAX 32768.0f

		m_XAnalog[(int)PAD_X_LevelCode::Level_LStickX] =
			state.Gamepad.sThumbLX / _SHROT_MAX;
		m_XAnalog[(int)PAD_X_LevelCode::Level_LStickY] =
			state.Gamepad.sThumbLY / _SHROT_MAX;

		m_XAnalog[(int)PAD_X_LevelCode::Level_RStickX] =
			state.Gamepad.sThumbRX / _SHROT_MAX;
		m_XAnalog[(int)PAD_X_LevelCode::Level_RStickY] =
			state.Gamepad.sThumbRY / _SHROT_MAX;
	}


	bool Down(PAD_X_KeyCode keyCoord){
		return m_XState[(int)keyCoord] != 0;
	}
	float Level(PAD_X_LevelCode levelCoord){
		return m_XAnalog[(int)levelCoord];
	}

private:

	//XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
	//XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
	//XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
	const int MAX_CONTROLLERS = 1;
	XINPUT_STATE state;


	BYTE m_XState[(int)PAD_X_KeyCode::Count];
	float m_XAnalog[(int)PAD_X_LevelCode::Count];

	enum XInput_BitFlag{
		Button_Up = XINPUT_GAMEPAD_DPAD_UP,
		Button_Down = XINPUT_GAMEPAD_DPAD_DOWN,
		Button_Left = XINPUT_GAMEPAD_DPAD_LEFT,
		Button_Right = XINPUT_GAMEPAD_DPAD_RIGHT,
		Button_Start = XINPUT_GAMEPAD_START,
		Button_Back = XINPUT_GAMEPAD_BACK,
		Button_L3 = XINPUT_GAMEPAD_LEFT_THUMB,
		Button_R3 = XINPUT_GAMEPAD_RIGHT_THUMB,
		Button_L1 = XINPUT_GAMEPAD_LEFT_SHOULDER,
		Button_R1 = XINPUT_GAMEPAD_RIGHT_SHOULDER,
		Button_A = XINPUT_GAMEPAD_A,
		Button_B = XINPUT_GAMEPAD_B,
		Button_X = XINPUT_GAMEPAD_X,
		Button_Y = XINPUT_GAMEPAD_Y,
	};
};