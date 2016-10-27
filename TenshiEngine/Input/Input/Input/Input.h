#pragma once

#include "InputCoord.h"
//class InputManager;

//SECTIONS
//.MYSEC READ WRITE SHARED
//#pragma data_seg(".MYSEC")


#ifdef _EXPORTING
#define CLASS_DECLSPEC    __declspec(dllexport)
#else
#define CLASS_DECLSPEC    __declspec(dllimport)
#endif

class DLLInput
{
public:
	//static Input* GetInput();

	CLASS_DECLSPEC static bool Trigger(KeyCode key);
	CLASS_DECLSPEC static int Down(KeyCode key);
	CLASS_DECLSPEC static bool Up(KeyCode key);


	CLASS_DECLSPEC static bool Trigger_Engine(KeyCode key);
	CLASS_DECLSPEC static int Down_Engine(KeyCode key);
	CLASS_DECLSPEC static bool Up_Engine(KeyCode key);

	CLASS_DECLSPEC static bool MTrigger(MouseCode key);
	CLASS_DECLSPEC static int MDown(MouseCode key);
	CLASS_DECLSPEC static bool MUp(MouseCode key);

	CLASS_DECLSPEC static void MousePosition(int* x, int* y);
	CLASS_DECLSPEC static void MouseLeftDragVector(int* x, int* y);


	CLASS_DECLSPEC static bool DS4Trigger(PAD_DS4_KeyCode key);
	CLASS_DECLSPEC static int DS4Down(PAD_DS4_KeyCode key);
	CLASS_DECLSPEC static bool DS4Up(PAD_DS4_KeyCode key);

	CLASS_DECLSPEC static void DS4Analog1(PAD_DS4_LevelCode coord,float* x);
	CLASS_DECLSPEC static void DS4Analog2(PAD_DS4_Velo2Code coord,float* x, float* y);
	CLASS_DECLSPEC static void DS4Analog3(PAD_DS4_Velo3Code coord,float* x, float* y, float* z);


	CLASS_DECLSPEC static bool XTrigger(PAD_X_KeyCode key);
	CLASS_DECLSPEC static int XDown(PAD_X_KeyCode key);
	CLASS_DECLSPEC static bool XUp(PAD_X_KeyCode key);

	CLASS_DECLSPEC static void XAnalog1(PAD_X_LevelCode coord, float* x);
	CLASS_DECLSPEC static void XAnalog2(PAD_X_Velo2Code coord, float* x, float* y);

private:

};

#include <windows.h>

//#define INITGUID
#include <dinput.h>

class DS4;
class XInput;
class InputManager{
public:

	CLASS_DECLSPEC static void InitDirectInput(HWND hWnd, HINSTANCE hInst);
	CLASS_DECLSPEC static void Release();

	CLASS_DECLSPEC static void Update(int TargetFocus);

	CLASS_DECLSPEC static void SetMouseP(const LPARAM& lParam);
	CLASS_DECLSPEC static void SetMouseXY(int x, int y);
	CLASS_DECLSPEC static void SetMouseL(bool donw);
	CLASS_DECLSPEC static void SetMouseR(bool donw);

private:
	static int mKeyCoord[(int)KeyCode::Count];
	static int mKeyCoordEngine[(int)KeyCode::Count];
	static bool mMouseBool[(int)MouseCode::Count];
	static int mMouse[(int)MouseCode::Count];
	static int mMouseX, mMouseY;
	static int mMouseLClickX, mMouseLClickY;

	static LPDIRECTINPUT8			pDInput;			// DirectInputオブジェクト
	static LPDIRECTINPUTDEVICE8		pDIKeyboard;		// キーボードデバイス
	static BYTE						diKeyState[256];		// キーボード情報

	static DS4* ds4;
	static int mDS4Input[(int)PAD_DS4_KeyCode::Count];


	static XInput* xinput;
	static int mXInput[(int)PAD_X_KeyCode::Count];

	static bool mDIKeyboardDeviceLost;

	friend DLLInput;
};

//#pragma data_seg()