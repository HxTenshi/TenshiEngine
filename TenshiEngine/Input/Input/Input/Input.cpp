

#pragma comment( lib , "dinput8.lib" )
#pragma comment( lib , "dxguid.lib" )

#define _EXPORTING
#include "Input.h"
#include "../DS4.h"
#include "../XInput.h"

//Input* DLLInput::GetInput(){
//	static Input input;
//	return &input;
//}

bool DLLInput::Trigger(KeyCode key){
	return InputManager::mKeyCoord[(int)key] == 1;
}
int DLLInput::Down(KeyCode key){
	return InputManager::mKeyCoord[(int)key] >= 0 ? InputManager::mKeyCoord[(int)key] : 0;
}
bool DLLInput::Up(KeyCode key){
	return InputManager::mKeyCoord[(int)key] == -1;
}

bool DLLInput::Trigger_Engine(KeyCode key){
	return InputManager::mKeyCoordEngine[(int)key] == 1;
}
int DLLInput::Down_Engine(KeyCode key){
	return InputManager::mKeyCoordEngine[(int)key] >= 0 ? InputManager::mKeyCoordEngine[(int)key] : 0;
}
bool DLLInput::Up_Engine(KeyCode key){
	return InputManager::mKeyCoordEngine[(int)key] == -1;
}

bool DLLInput::MTrigger(MouseCode key){
	return InputManager::mMouse[(int)key] == 1;
}
int DLLInput::MDown(MouseCode key){
	return InputManager::mMouse[(int)key] >= 0 ? InputManager::mMouse[(int)key] : 0;
}
bool DLLInput::MUp(MouseCode key){
	return InputManager::mMouse[(int)key] == -1;
}
void DLLInput::MousePosition(int* x, int* y){
	*x = InputManager::mMouseX;
	*y = InputManager::mMouseY;
}
void DLLInput::MouseLeftDragVector(int* x, int* y){
	*x = InputManager::mMouseX - InputManager::mMouseLClickX;
	*y = InputManager::mMouseY - InputManager::mMouseLClickY;
}


bool DLLInput::DS4Trigger(PAD_DS4_KeyCode key){
	return InputManager::mDS4Input[(int)key] == 1;
}
int DLLInput::DS4Down(PAD_DS4_KeyCode key){
	return InputManager::mDS4Input[(int)key] >= 0 ? InputManager::mDS4Input[(int)key] : 0;
}
bool DLLInput::DS4Up(PAD_DS4_KeyCode key){
	return InputManager::mDS4Input[(int)key] == -1;
}


void DLLInput::DS4Analog1(PAD_DS4_LevelCode coord, float* x){
	*x = InputManager::ds4->Level(coord);
}
void DLLInput::DS4Analog2(PAD_DS4_Velo2Code coord, float* x, float* y){
	int c = 2 + (int)coord * 2;
	*x = InputManager::ds4->Level((PAD_DS4_LevelCode)c);
	*y = InputManager::ds4->Level((PAD_DS4_LevelCode)(c+1));

}
void DLLInput::DS4Analog3(PAD_DS4_Velo3Code coord, float* x, float* y, float* z){
	int c = 2 + 8 + (int)coord * 3;
	*x = InputManager::ds4->Level((PAD_DS4_LevelCode)c);
	*y = InputManager::ds4->Level((PAD_DS4_LevelCode)(c + 1));
	*z = InputManager::ds4->Level((PAD_DS4_LevelCode)(c + 2));
}


bool DLLInput::XTrigger(PAD_X_KeyCode key){
	return InputManager::mXInput[(int)key] == 1;
}
int DLLInput::XDown(PAD_X_KeyCode key){
	return InputManager::mXInput[(int)key] >= 0 ? InputManager::mXInput[(int)key] : 0;
}
bool DLLInput::XUp(PAD_X_KeyCode key){
	return InputManager::mXInput[(int)key] == -1;
}

void DLLInput::XAnalog1(PAD_X_LevelCode coord, float* x){
	*x = InputManager::xinput->Level(coord);
}
void DLLInput::XAnalog2(PAD_X_Velo2Code coord, float* x, float* y){
	int c = 2 + (int)coord * 2;
	*x = InputManager::xinput->Level((PAD_X_LevelCode)c);
	*y = InputManager::xinput->Level((PAD_X_LevelCode)(c + 1));
}