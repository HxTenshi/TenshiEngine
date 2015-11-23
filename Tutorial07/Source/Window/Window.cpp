
#include "Window.h"
#include "Input/Input.h"
#include "Graphic/Font/Font.h"


UINT WindowState::mWidth = 1200;
UINT WindowState::mHeight = 800;


HMODULE Window::mhModuleWnd = NULL;
HWND Window::mhWnd = NULL;

Test::NativeFraction Window::mMainWindow_WPF;
HWND Window::mGameScreenHWND = NULL;

std::vector<const std::function<void(void*)>> Window::mWPFCollBacks;



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//BitBlt(hdc, 0, 0, WindowState::mWidth, WindowState::mHeight, hdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		// マウスが移動したときの処理
		InputManager::SetMouseP(lParam);
		break;

	case WM_LBUTTONDOWN:
		// マウス左ボタンがクリックされたときの処理
		InputManager::SetMouseL(true);
		break;
	case WM_LBUTTONUP:
		// マウス左ボタンがクリックが放されたときの処理

		//ウィンドウ外で離すとダメ
		InputManager::SetMouseL(false);
		break;

	case WM_RBUTTONDOWN:
		// マウス右ボタンがクリックされたときの処理
		InputManager::SetMouseR(true);
		break;
	case WM_RBUTTONUP:
		// マウス右ボタンがクリックが放されたときの処理
		InputManager::SetMouseR(false);
		break;

	case WM_CREATE:
		break;

	case WM_MYWMCOLLBACK:{
		const auto& func = Window::mWPFCollBacks[(int)wParam];
		if (func)func((void*)lParam);
		break;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}