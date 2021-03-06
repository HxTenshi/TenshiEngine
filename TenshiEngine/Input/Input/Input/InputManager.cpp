

#define _EXPORTING
#include "Input.h"
#include "../DS4.h"
#include "../XInput.h"
//static
int InputManager::mKeyCoord[(int)KeyCode::Count];
int InputManager::mKeyCoordEngine[(int)KeyCode::Count];
bool InputManager::mMouseBool[(int)MouseCode::Count];
int InputManager::mMouse[(int)MouseCode::Count];
int InputManager::mMouseX;
int InputManager::mMouseY;
int InputManager::mMouseLClickX;
int InputManager::mMouseLClickY;

bool InputManager::mDIKeyboardDeviceLost;

LPDIRECTINPUT8			InputManager::pDInput = NULL;			// DirectInputオブジェクト
LPDIRECTINPUTDEVICE8	InputManager::pDIKeyboard = NULL;		// キーボードデバイス
BYTE					InputManager::diKeyState[256];		// キーボード情報

//static
DS4* InputManager::ds4 = NULL;
int InputManager::mDS4Input[(int)PAD_DS4_KeyCode::Count];

XInput* InputManager::xinput = NULL;
int InputManager::mXInput[(int)PAD_X_KeyCode::Count];

//static
void InputManager::InitDirectInput(HWND hWnd, HINSTANCE hInst){

	mDIKeyboardDeviceLost = false;
	mMouseX = 0;
	mMouseY = 0;
	for (int i = 0; i < (int)MouseCode::Count; i++){
		mMouse[i] = 0;
		mMouseBool[i] = false;
	}
	for (int i = 0; i < (int)KeyCode::Count; i++){
		mKeyCoord[i] = 0;
		mKeyCoordEngine[i] = 0;
	}
	for (int i = 0; i < (int)PAD_DS4_KeyCode::Count; i++){
		mDS4Input[i] = 0;
	}
	for (int i = 0; i < (int)PAD_X_KeyCode::Count; i++){
		mXInput[i] = 0;
	}

	//LPDIRECTINPUTDEVICE8	pDIJoypad = NULL;			// ジョイパッドデバイス
	//DWORD					diJoyState = 0x00000000L;	// ジョイパッド情報
	//LPDIRECTINPUTDEVICE8	pDIMouse = NULL;			// マウスデバイス
	//DIMOUSESTATE			diMouseState;				// マウス情報

	HRESULT	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDInput, NULL);
	if (FAILED(hr))
		return;	// DirectInput8の作成に失敗

	//キーボード用にデバイスオブジェクトを作成
	hr = pDInput->CreateDevice(GUID_SysKeyboard, &pDIKeyboard, NULL);
	if (FAILED(hr))
		return;  // デバイスの作成に失敗

	//キーボード用のデータ・フォーマットを設定
	hr = pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return; // デバイスの作成に失敗

	//モードを設定（フォアグラウンド＆非排他モード）
	hr = pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)){
		return; // モードの設定に失敗
	}

	//キーボード入力制御開始
	pDIKeyboard->Acquire();


	ds4 = new DS4();
	ds4->Initialize_And_FindHID();


	xinput = new XInput();

	return;
}

void InputManager::Release(){
	//入力されてない状態で？開放しないといけない？SAFE_REREASE
	if (pDIKeyboard)pDIKeyboard->Release();
	if (pDInput)pDInput->Release();
	if (ds4){
		ds4->Release();
		delete ds4;
	}

	if (xinput){
		delete xinput;
	}
}

void InputManager::Update(int TargetFocus){

	for (int i = 0; i < (int)MouseCode::Count; i++){
		if (mMouseBool[i]){
			mMouse[i]++;
		}
		else if (mMouse[i]>0){
			mMouse[i] = -1;
		}
		else{
			mMouse[i] = 0;
		}
	}


	//pDIKeyboard->Acquire();
	HRESULT			hr;
	hr = pDIKeyboard->GetDeviceState(256, diKeyState);
	//DIERR_INPUTLOST
	if (hr != S_OK){
		mDIKeyboardDeviceLost = true;
	}
	if (mDIKeyboardDeviceLost){
		if (pDIKeyboard->Acquire() == S_OK){
			mDIKeyboardDeviceLost = false;
		}
	}
	for (int i = 0; i < (int)KeyCode::Count; i++){

		if (diKeyState[i] && TargetFocus==1){
			mKeyCoord[i]++;
		}
		else if (mKeyCoord[i]>0){
			mKeyCoord[i] = -1;
		}
		else{
			mKeyCoord[i] = 0;
		}
		if (diKeyState[i] && TargetFocus != 0){
			mKeyCoordEngine[i]++;
			diKeyState[i] = 0;
		}
		else if (mKeyCoordEngine[i]>0){
			mKeyCoordEngine[i] = -1;
		}
		else{
			mKeyCoordEngine[i] = 0;
		}
	}

	ds4->Read();

	for (int i = 0; i < (int)PAD_DS4_KeyCode::Count; i++){

		if (ds4->Down((PAD_DS4_KeyCode)i) && TargetFocus){
			mDS4Input[i]++;
		}
		else if (mDS4Input[i]>0){
			mDS4Input[i] = -1;
		}
		else{
			mDS4Input[i] = 0;
		}
	}

	xinput->Read();
	for (int i = 0; i < (int)PAD_X_KeyCode::Count; i++){

		if (xinput->Down((PAD_X_KeyCode)i) && TargetFocus){
			mXInput[i]++;
		}
		else if (mXInput[i]>0){
			mXInput[i] = -1;
		}
		else{
			mXInput[i] = 0;
		}
	}
}

void InputManager::SetMouseP(const LPARAM& lParam){
	mMouseX = LOWORD(lParam);	// マウスＸ座標取得
	mMouseY = HIWORD(lParam);	// マウスＹ座標取得
}
void InputManager::SetMouseXY(int x, int y){
	mMouseX = x;
	mMouseY = y;
}
void InputManager::SetMouseL(bool donw){
	if (mMouseBool[(int)MouseCode::Left] != donw){
		mMouseLClickX = mMouseX;
		mMouseLClickY = mMouseY;
	}
	mMouseBool[(int)MouseCode::Left] = donw;
}
void InputManager::SetMouseR(bool donw){
	mMouseBool[(int)MouseCode::Right] = donw;
}
