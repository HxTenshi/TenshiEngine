#pragma once

#include <windows.h>

//#define INITGUID
#include <dinput.h>

#pragma comment( lib , "dinput8.lib" )

#ifdef _DEBUG
#pragma comment(lib, "MyInput.lib")
#else
#pragma comment(lib, "MyInput.lib")
#endif

//class DLLInput
//{
//public:
//	static bool Trigger(KeyCoord key);
//	static int Down(KeyCoord key);
//	static bool Up(KeyCoord key);
//
//	static bool MTrigger(MouseCoord key);
//	static int MDown(MouseCoord key);
//	static bool MUp(MouseCoord key);
//
//	static void MousePosition(int* x, int* y);
//	static void MouseLeftDragVector(int* x, int* y);
//
//private:
//
//	
//};

#include "../../Input/Input/Input/Input.h"
class Input
{
public:
	static bool Trigger(KeyCode key){
		return DLLInput::Trigger(key);
	}
	static int Down(KeyCode key){
		return DLLInput::Down(key);

	}
	static bool Up(KeyCode key){
		return DLLInput::Up(key);

	}

	static bool Trigger(MouseCode key){
		return DLLInput::MTrigger(key);

	}
	static int Down(MouseCode key){
		return DLLInput::MDown(key);

	}
	static bool Up(MouseCode key){
		return DLLInput::MUp(key);

	}

	static void MousePosition(int* x, int* y){
		DLLInput::MousePosition(x, y);
	}
	static void MouseLeftDragVector(int* x, int* y){
		DLLInput::MouseLeftDragVector(x, y);
	}

	static bool Trigger(PAD_DS4_KeyCode key){
		return DLLInput::DS4Trigger(key);
	}
	static int Down(PAD_DS4_KeyCode key){
		return DLLInput::DS4Down(key);
	}
	static bool Up(PAD_DS4_KeyCode key){
		return DLLInput::DS4Up(key);
	}

	static XMVECTOR Analog(PAD_DS4_LevelCode key){
		float x;
		DLLInput::DS4Analog1(key,&x);
		return XMVectorSet(x, 0, 0, 1);
	}
	static XMVECTOR Analog(PAD_DS4_Velo2Code key){
		float x,y;
		DLLInput::DS4Analog2(key, &x,&y);
		return XMVectorSet(x, y, 0, 1);
	}
	static XMVECTOR Analog(PAD_DS4_Velo3Code key){
		float x,y,z;
		DLLInput::DS4Analog3(key, &x,&y,&z);
		return XMVectorSet(x, y, z, 1);
	}


	static bool Trigger(PAD_X_KeyCode key){
		return DLLInput::XTrigger(key);
	}
	static int Down(PAD_X_KeyCode key){
		return DLLInput::XDown(key);
	}
	static bool Up(PAD_X_KeyCode key){
		return DLLInput::XUp(key);
	}

	static XMVECTOR Analog(PAD_X_LevelCode key){
		float x;
		DLLInput::XAnalog1(key, &x);
		return XMVectorSet(x, 0, 0, 1);
	}
	static XMVECTOR Analog(PAD_X_Velo2Code key){
		float x, y;
		DLLInput::XAnalog2(key, &x, &y);
		return XMVectorSet(x, y, 0, 1);
	}
private:


};
class EngineInput
{
public:
	static bool Trigger(KeyCode key){
		return DLLInput::Trigger_Engine(key);
	}
	static int Down(KeyCode key){
		return DLLInput::Down_Engine(key);

	}
	static bool Up(KeyCode key){
		return DLLInput::Up_Engine(key);

	}

private:


};

//class InputManager{
//public:
//	InputManager();
//	static void Release();
//
//	static void InitDirectInput(HWND hWnd, HINSTANCE hInst);
//
//	static void Update();
//
//	static void SetMouseP(const LPARAM& lParam);
//	static void SetMouseL(bool donw);
//	static void SetMouseR(bool donw);
//
//private:
//	static bool mKeyCoordBool[(int)KeyCoord::Count];
//	static int mKeyCoord[(int)KeyCoord::Count];
//	static bool mMouseBool[(int)MouseCoord::Count];
//	static int mMouse[(int)MouseCoord::Count];
//	static int mMouseX, mMouseY;
//	static int mMouseLClickX, mMouseLClickY;
//
//	static LPDIRECTINPUT8			pDInput;			// DirectInputオブジェクト
//	static LPDIRECTINPUTDEVICE8		pDIKeyboard;		// キーボードデバイス
//	static BYTE						diKeyState[256];		// キーボード情報
//
//	static bool mDIKeyboardDeviceLost;
//
//	friend Input;
//};