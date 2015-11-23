

#pragma comment( lib , "dinput8.lib" )
#pragma comment( lib , "dxguid.lib" )

#define _EXPORTING
#include "Input.h"

//Input* DLLInput::GetInput(){
//	static Input input;
//	return &input;
//}

bool DLLInput::Trigger(KeyCoord key){
	return InputManager::mKeyCoord[(int)key] == 1;
}
int DLLInput::Down(KeyCoord key){
	return InputManager::mKeyCoord[(int)key] >= 0 ? InputManager::mKeyCoord[(int)key] : 0;
}
bool DLLInput::Up(KeyCoord key){
	return InputManager::mKeyCoord[(int)key] == -1;
}

bool DLLInput::MTrigger(MouseCoord key){
	return InputManager::mMouse[(int)key] == 1;
}
int DLLInput::MDown(MouseCoord key){
	return InputManager::mMouse[(int)key] >= 0 ? InputManager::mMouse[(int)key] : 0;
}
bool DLLInput::MUp(MouseCoord key){
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
