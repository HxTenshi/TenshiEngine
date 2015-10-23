
#include "Window.h"
#include "Input/Input.h"
#include "Graphic/Font/Font.h"


UINT WindowState::mWidth = 1200;
UINT WindowState::mHeight = 800;

UINT WindowState::mTreeViewWidth = 200;
UINT WindowState::mTreeViewHeight = 0;


HMODULE Window::mhModuleWnd = NULL;
HWND Window::mhWnd = NULL;
GameScreenWindow Window::mGameScreenWindow;
TreeViewWindow Window::mTreeViewWindow;
InspectorWindow Window::mInspectorWindow;

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

	case WM_DROPFILES:{
		HDROP hDrop = (HDROP)wParam;
		char szFileName[256];
		//D&Dしたファイルの数
		int FileNum = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
		for (int i = 0; i < (int)FileNum; i++) {
			DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
			if (ShellExecute(hWnd, NULL, szFileName, NULL, NULL, SW_SHOWNORMAL) <= (HINSTANCE)32)
				MessageBox(hWnd, "ファイルを開けませんでした", "失敗", MB_OK);
		}
		DragFinish(hDrop);
		break;
	}
	case WM_COMMAND:{
		//ここで元々のプロシージアのアドレスを取得する。
		HWND childHWND = GetDlgItem(hWnd, (int)LOWORD(wParam));
		if (!childHWND)return DefWindowProc(hWnd, message, wParam, lParam);
		ChildWindow* thisPtr = (ChildWindow*)GetWindowLong(childHWND, GWL_USERDATA);
		if (!thisPtr){//取得できなかった場合（作成に失敗してる)
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		else{//取得できた場合
			thisPtr->MyWndProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->idFrom)
		{
		case (UINT_PTR)IDC::ForlderTree:{
			//ここで元々のプロシージアのアドレスを取得する。
			HWND childHWND = GetDlgItem(hWnd, (int)IDC::ForlderTree);
			ChildWindow* thisPtr = (ChildWindow*)GetWindowLong(childHWND, GWL_USERDATA);
			if (!thisPtr){//取得できなかった場合（作成に失敗してる)
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			else{//取得できた場合
				thisPtr->MyWndProc(hWnd, message, wParam, lParam);
				break;
			}
		}
			break;
		default:{
			//ここで元々のプロシージアのアドレスを取得する。
			HWND childHWND = GetDlgItem(hWnd, (int)((LPNMHDR)lParam)->idFrom);
			if (!childHWND)return DefWindowProc(hWnd, message, wParam, lParam);
			ChildWindow* thisPtr = (ChildWindow*)GetWindowLong(childHWND, GWL_USERDATA);
			if (!thisPtr){//取得できなかった場合（作成に失敗してる)
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			else{//取得できた場合
				thisPtr->MyWndProc(hWnd, message, wParam, lParam);
			}
			break;
		}
		}
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