#pragma once
#include <Windows.h>
class Window;

class ChildWindow{
public:
	ChildWindow(){}
	virtual ~ChildWindow(){}
	
	virtual void Create(Window* window) = 0;
	
	HWND GetHWND(){
		return mhWnd;
	}

	virtual LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	void SetMyWndProc(){
		//GWL_USERDATAという好きに使っても良い領域に自分のポインタを入れる。
		SetWindowLong(mhWnd, GWL_USERDATA, (LONG)this);
	}

protected:
	Window* mWindow;
	HWND mhWnd;
};