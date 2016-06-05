
#include "Window.h"
#include "Input/Input.h"
#include "Graphic/Font/Font.h"

#include "../resource/resource.h"

#ifdef _ENGINE_MODE

#ifdef _DEBUG
#pragma comment(lib, "Debug/CppWPFdll.lib")
#else
#pragma comment(lib, "Release/CppWPFdll.lib")
#endif

#endif


UINT WindowState::mWidth = 1200;
UINT WindowState::mHeight = 800;


//HMODULE Window::mhModuleWnd = NULL;
HWND Window::mhWnd = NULL;
#ifdef _ENGINE_MODE
std::vector<const std::function<void(void*)>> Window::mWPFCollBacks;
Test::NativeFraction Window::mMainWindow_WPF;
#endif

HWND Window::mGameScreenHWND = NULL;


int Window::Init(){

#ifdef _ENGINE_MODE
	mWPFCollBacks.resize((int)MyWindowMessage::Count);
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = NULL;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mhInstance;
	wcex.hIcon = LoadIcon(mhInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "TenshiEngineDummyWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	RECT rc = { 0, 0, 1, 1 };
	AdjustWindowRect(&rc, NULL, FALSE);
	mDummyhWnd = CreateWindow("TenshiEngineDummyWindowClass", "TenshiEngineDummyWindow", NULL,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, mhInstance,
		NULL);
	if (!mDummyhWnd)
		return E_FAIL;
#else
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = NULL;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mhInstance;
	wcex.hIcon = LoadIcon(mhInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "GameWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	RECT rc = { 0, 0, WindowState::mWidth, WindowState::mHeight };
	AdjustWindowRect(&rc, NULL, FALSE);
	mhWnd = CreateWindow("GameWindowClass", "GameWindow", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, mhInstance,
		NULL);
	mGameScreenHWND = mhWnd;
	ShowWindow(mhWnd, mnCmdShow);
#endif

	//CreateProcess(NULL, lpszPathName, NULL, NULL, TRUE,
	//	DEBUG_PROCESS | CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS,
	//	NULL, NULL, &si, &pi);
	//hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	///* プロセスにアタッチする */
	//DebugActiveProcess(dwProcessId);
	//
	//while (TRUE){
	//	/* イベントを待つ */
	//	if (!WaitForDebugEvent(&debug, INFINITE)){
	//		return;
	//	}
	//
	//	switch (debug.dwDebugEventCode){
	//	case OUTPUT_DEBUG_STRING_EVENT:   /* デバッグ文字列を受信した */
	//		/* デバッグ文字列を読み出す */
	//		ReadProcessMemory(hProcess,
	//			debug.u.DebugString.lpDebugStringData,
	//			szBuff, debug.u.DebugString.nDebugStringLength, &dwRead);
	//		*(szBuff + dwRead) = '\0';
	//		break;
	//	case CREATE_PROCESS_DEBUG_EVENT:  /* プロセスを生成した */
	//		break;
	//	case CREATE_THREAD_DEBUG_EVENT:   /* スレッドを生成した */
	//		break;
	//	case EXIT_THREAD_DEBUG_EVENT:     /* スレッドが終了した */
	//		break;
	//	case LOAD_DLL_DEBUG_EVENT:        /* DLLをロードした */
	//		break;
	//	case UNLOAD_DLL_DEBUG_EVENT:      /* DLLをアンロードした */
	//		break;
	//	case EXCEPTION_DEBUG_EVENT:       /* 例外が発生した */
	//		break;
	//	case RIP_EVENT:                   /* RIPイベント */
	//		break;
	//
	//	case EXIT_PROCESS_DEBUG_EVENT:    /* プロセスが終了した */
	//		return;
	//	default:
	//		break;
	//	}
	//	/* デバッグを続行する */
	//	ContinueDebugEvent(dwProcessId, debug.dwThreadId, DBG_CONTINUE);
	//}

	//InitCommonControls();

	//mGameScreenWindow.Create(this);
	//mInspectorWindow.Create(this);
	//mProjectWindow.Create(this);

	//ShowWindow(mhWnd, mnCmdShow);
	//UpdateWindow(mhWnd);

	//D&Dの許可
	//DragAcceptFiles(mhWnd,true);

	return S_OK;
}

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

#ifdef _ENGINE_MODE
	case WM_MYWMCOLLBACK:{
		const auto& func = Window::mWPFCollBacks[(int)wParam];
		if (func)func((void*)lParam);
		break;
	}
#endif
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}