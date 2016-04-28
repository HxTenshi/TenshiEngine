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


	CLASS_DECLSPEC static bool Trigger_Engine(KeyCoord key);
	CLASS_DECLSPEC static int Down_Engine(KeyCoord key);
	CLASS_DECLSPEC static bool Up_Engine(KeyCoord key);

	CLASS_DECLSPEC static bool MTrigger(MouseCoord key);
	CLASS_DECLSPEC static int MDown(MouseCoord key);
	CLASS_DECLSPEC static bool MUp(MouseCoord key);

	CLASS_DECLSPEC static void MousePosition(int* x, int* y);
	CLASS_DECLSPEC static void MouseLeftDragVector(int* x, int* y);


	CLASS_DECLSPEC static bool DS4Trigger(PAD_DS4_KeyCoord key);
	CLASS_DECLSPEC static int DS4Down(PAD_DS4_KeyCoord key);
	CLASS_DECLSPEC static bool DS4Up(PAD_DS4_KeyCoord key);

	CLASS_DECLSPEC static void DS4Analog1(PAD_DS4_LevelCoord coord,float* x);
	CLASS_DECLSPEC static void DS4Analog2(PAD_DS4_Velo2Coord coord,float* x, float* y);
	CLASS_DECLSPEC static void DS4Analog3(PAD_DS4_Velo3Coord coord,float* x, float* y, float* z);

private:

};

#include <windows.h>

//#define INITGUID
#include <dinput.h>

class DS4;
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
	static int mKeyCoordEngine[(int)KeyCoord::Count];
	static bool mMouseBool[(int)MouseCoord::Count];
	static int mMouse[(int)MouseCoord::Count];
	static int mMouseX, mMouseY;
	static int mMouseLClickX, mMouseLClickY;

	static LPDIRECTINPUT8			pDInput;			// DirectInputオブジェクト
	static LPDIRECTINPUTDEVICE8		pDIKeyboard;		// キーボードデバイス
	static BYTE						diKeyState[256];		// キーボード情報

	static DS4* ds4;
	static int mDS4Input[(int)PAD_DS4_KeyCoord::Count];

	static bool mDIKeyboardDeviceLost;

	friend DLLInput;
};

//#pragma data_seg()