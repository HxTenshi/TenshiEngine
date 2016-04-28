

#pragma comment( lib , "dinput8.lib" )
#pragma comment( lib , "dxguid.lib" )

#define _EXPORTING
#include "Input.h"
#include "../DS4.h"

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

bool DLLInput::Trigger_Engine(KeyCoord key){
	return InputManager::mKeyCoordEngine[(int)key] == 1;
}
int DLLInput::Down_Engine(KeyCoord key){
	return InputManager::mKeyCoordEngine[(int)key] >= 0 ? InputManager::mKeyCoordEngine[(int)key] : 0;
}
bool DLLInput::Up_Engine(KeyCoord key){
	return InputManager::mKeyCoordEngine[(int)key] == -1;
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


bool DLLInput::DS4Trigger(PAD_DS4_KeyCoord key){
	return InputManager::mDS4Input[(int)key] == 1;
}
int DLLInput::DS4Down(PAD_DS4_KeyCoord key){
	return InputManager::mDS4Input[(int)key] >= 0 ? InputManager::mDS4Input[(int)key] : 0;
}
bool DLLInput::DS4Up(PAD_DS4_KeyCoord key){
	return InputManager::mDS4Input[(int)key] == -1;
}


void DLLInput::DS4Analog1(PAD_DS4_LevelCoord coord, float* x){
	*x = InputManager::ds4->Level(coord);
}
void DLLInput::DS4Analog2(PAD_DS4_Velo2Coord coord, float* x, float* y){
	int c = 2 + (int)coord * 2;
	*x = InputManager::ds4->Level((PAD_DS4_LevelCoord)c);
	*y = InputManager::ds4->Level((PAD_DS4_LevelCoord)(c+1));

}
void DLLInput::DS4Analog3(PAD_DS4_Velo3Coord coord, float* x, float* y, float* z){
	int c = 2 + 8 + (int)coord * 3;
	*x = InputManager::ds4->Level((PAD_DS4_LevelCoord)c);
	*y = InputManager::ds4->Level((PAD_DS4_LevelCoord)(c + 1));
	*z = InputManager::ds4->Level((PAD_DS4_LevelCoord)(c + 2));
}