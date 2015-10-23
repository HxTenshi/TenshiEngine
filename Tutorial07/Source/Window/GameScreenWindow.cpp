
#include "GameScreenWindow.h"
#include "Window.h"

void GameScreenWindow::Create(Window* window){
	mWindow = window;
	RECT rc = { -1, -1, WindowState::mWidth+2, WindowState::mHeight+2 };


	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mWindow->mhInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "GameScreenWindow";
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex))
		return;

	mhWnd = CreateWindow("GameScreenWindow", "",
		WS_CHILD | WS_BORDER | WS_VISIBLE,
		rc.left, rc.top, rc.right, rc.bottom,
		mWindow->mhWnd, (HMENU)2, mWindow->mhInstance, NULL);
}