#include "InputManagerRapper.h"
#include "Input.h"
#include "Window/Window.h"

void InputManagerRapper::Initialize(HWND hWnd, HINSTANCE hInstance){
		mr = false;
		ml = false;
		mx = 0;
		my = 0;
		wx = 1;
		wy = 1;

		InputManager::InitDirectInput(hWnd, hInstance);
#ifdef _ENGINE_MODE
		mScreenFocus = false;
		Window::SetMouseEvents(&mScreenFocus, &ml, &mr, &mx, &my, &wx, &wy);
		mEngineFocus = true;
		Window::SetEngineFocusEvent(&mEngineFocus);
#else
		mScreenFocus = true;
#endif
	}

void InputManagerRapper::Update(){

#ifdef _ENGINE_MODE

		InputManager::SetMouseL(ml);
		InputManager::SetMouseR(mr);
		int x = (int)((mx / (float)wx) * (float)WindowState::mWidth + 1.5f);
		int y = (int)((my / (float)wy) * (float)WindowState::mHeight + 1.5f);
		InputManager::SetMouseXY(x, y);

		int f = 0;
		if(mEngineFocus) {
			f = 2;
		}
		if (mScreenFocus) {
			f = 1;
		}
		InputManager::Update(f);
#else
		InputManager::Update(mScreenFocus?1:0);
#endif

	}

void InputManagerRapper::Release(){
		InputManager::Release();
	}

#ifdef _ENGINE_MODE
bool InputManagerRapper::GetScreenFocus(){
		return mScreenFocus;
	}
#endif