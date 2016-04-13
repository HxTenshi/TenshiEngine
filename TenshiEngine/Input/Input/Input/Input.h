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

	CLASS_DECLSPEC static bool Trigger(KeyCoord key);
	CLASS_DECLSPEC static int Down(KeyCoord key);
	CLASS_DECLSPEC static bool Up(KeyCoord key);

	CLASS_DECLSPEC static bool MTrigger(MouseCoord key);
	CLASS_DECLSPEC static int MDown(MouseCoord key);
	CLASS_DECLSPEC static bool MUp(MouseCoord key);

	CLASS_DECLSPEC static void MousePosition(int* x, int* y);
	CLASS_DECLSPEC static void MouseLeftDragVector(int* x, int* y);

private:

};

#include <windows.h>

//#define INITGUID
#include <dinput.h>


class InputManager{
public:

	CLASS_DECLSPEC static void InitDirectInput(HWND hWnd, HINSTANCE hInst);
	CLASS_DECLSPEC static void Release();

	CLASS_DECLSPEC static void Update(bool TargetFocus);

	CLASS_DECLSPEC static void SetMouseP(const LPARAM& lParam);
	CLASS_DECLSPEC static void SetMouseXY(int x, int y);
	CLASS_DECLSPEC static void SetMouseL(bool donw);
	CLASS_DECLSPEC static void SetMouseR(bool donw);

private:
	static int mKeyCoord[(int)KeyCoord::Count];
	static bool mMouseBool[(int)MouseCoord::Count];
	static int mMouse[(int)MouseCoord::Count];
	static int mMouseX, mMouseY;
	static int mMouseLClickX, mMouseLClickY;

	static LPDIRECTINPUT8			pDInput;			// DirectInputオブジェクト
	static LPDIRECTINPUTDEVICE8		pDIKeyboard;		// キーボードデバイス
	static BYTE						diKeyState[256];		// キーボード情報

	static bool mDIKeyboardDeviceLost;

	friend DLLInput;
};

//#pragma data_seg()