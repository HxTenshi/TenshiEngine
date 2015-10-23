
#include "InspectorWindow.h"
#include "Window.h"
#include "Game/Actor.h"


InspectorEDIT::InspectorEDIT(HWND hWnd, HINSTANCE hInstance, float* param, bool* fixFlag, int PosY)
:mParam(param)
, mFixFlag(fixFlag){
	//mWindow = window;
	//Create(window);

	std::string text = std::to_string(*param);
	//mhWnd = CreateWindow(
	//	TEXT("EDIT"), text.c_str(),
	//	WS_CHILD | WS_VISIBLE | WS_BORDER,
	//	0, PosY, WindowState::mTreeViewWidth, 20,
	//	hWnd, (HMENU)10000 + PosY, hInstance, NULL
	//	);
	mhWnd = CreateWindowEx(0,
		TEXT("buttonTemplate"), text.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		0, PosY, WindowState::mTreeViewWidth, 20,
		hWnd, (HMENU)10000 + PosY, hInstance, NULL
		);

	SetMyWndProc();
}
InspectorEDIT::~InspectorEDIT(){
	DestroyWindow(mhWnd);
}

void InspectorEDIT::Create(Window* window){
}


LRESULT CALLBACK InspectorEDIT::MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch (HIWORD(wParam))
	{
	case EN_CHANGE:
		// “à—e‚ª•Ï‚í‚é’¼‘O‚Ìˆ—
		break;
	case EN_UPDATE:{
		// “à—e‚ª•Ï‚í‚Á‚½’¼Œã
		std::string str;
		int len = GetWindowTextLength(mhWnd);
		str.resize(len);
		GetWindowText(mhWnd, (LPSTR)str.c_str(), len);
		*mParam = (float)atof(str.c_str());
		*mFixFlag = false;
		break;
	}
	case EN_MAXTEXT:
		// ...
		break;
	}
	return 0;
}

void InspectorWindow::Create(Window* window){
	mWindow = window;
	RECT rc = { WindowState::mWidth + WindowState::mTreeViewWidth, 0, WindowState::mTreeViewWidth, WindowState::mHeight/2 };


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
	wcex.lpszClassName = "InspectorWindow";
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex))
		return;

	mhWnd = CreateWindow("InspectorWindow", "",
		WS_CHILD | WS_BORDER | WS_VISIBLE,
		rc.left, rc.top, rc.right, rc.bottom,
		mWindow->mhWnd, (HMENU)3, mWindow->mhInstance, NULL);

	

	mhInstance = mWindow->mhInstance;
}

void InspectorWindow::InsertConponentData(Actor* actor){

	if (!actor){
		Window::ClearInspector();
		mCurrentAddress = (int)actor;
	}
	if (mCurrentAddress == (int)actor)return;
	mCurrentAddress = (int)actor;
	//mDataPosY = 0;
	//for (auto h : mList)delete h;
	//mList.clear();
	Window::ClearInspector();
	actor->CreateInspector();

}

void InspectorWindow::AddLabel(const char* text){
	//auto h = CreateWindow(
	//	TEXT("STATIC"), text,
	//	WS_CHILD | WS_VISIBLE,
	//	0, mDataPosY, WindowState::mTreeViewWidth, 20,
	//	mhWnd, 0, mhInstance, NULL
	//	);
	//mDataPosY += 20;
	//mList.push_back(h);
}
void InspectorWindow::AddParam(float* param,bool* fixFlag){

	//mList.push_back(new InspectorEDIT(mhWnd, mhInstance, param, fixFlag, mDataPosY));
	//mDataPosY += 20;
}