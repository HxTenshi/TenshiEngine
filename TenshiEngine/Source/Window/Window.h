#pragma once

#include <windows.h>
#include "../resource/resource.h"
#include <string>

#include <CommCtrl.h>

#include <vector>

#include <functional>
#include "Application/Define.h"
class WindowState{
public:
	static UINT mWidth;
	static UINT mHeight;
};
//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef _DEBUG
#pragma comment(lib, "Debug/CppWPFdll.lib")
#else
#pragma comment(lib, "Release/CppWPFdll.lib")
#endif

#include "../../CppWPFdll/CppWPFdll.h"

class Component;

class Window{
public:
	static Test::NativeFraction mMainWindow_WPF;
	static HWND mGameScreenHWND;
	Window(HINSTANCE hInstance, int nCmdShow)
		: mhInstance(hInstance)
		, mnCmdShow(nCmdShow)
	{

		mMainWindow_WPF.Initialize();

		HWND hWnd = NULL;
		while (!hWnd){
			hWnd = (HWND)mMainWindow_WPF.GetGameScreenHWND();
		}
		mhWnd = (HWND)mMainWindow_WPF.GetEditorHWND();
		//RECT rc;
		//GetClientRect(hWnd, &rc);
		//WindowState::mWidth = rc.right - rc.left;
		//WindowState::mHeight = rc.bottom - rc.top;

		mGameScreenHWND = hWnd;
	}
	int Init(){

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
		RECT rc = { 0, 0, 1,1 };
		AdjustWindowRect(&rc, NULL, FALSE);
		mDummyhWnd = CreateWindow("TenshiEngineDummyWindowClass", "TenshiEngineDummyWindow", NULL,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, mhInstance,
			NULL);
		if (!mDummyhWnd)
			return E_FAIL;

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

	static void Release(){
		if (mhModuleWnd)
			FreeLibrary(mhModuleWnd);
		mMainWindow_WPF.Release();
	}

	static HWND GetMainHWND(){
		return mhWnd;
	}
	static HWND GetGameScreenHWND(){

		return mGameScreenHWND;
		//return mGameScreenWindow.GetHWND();
	}

	static void SetWindowTitle(const std::string& title){
		SetWindowText(mhWnd, title.c_str());
	}

	template<class T>
	static void Deleter(T* ptr){
		mMainWindow_WPF.Deleter<T>(ptr);
	}
	static void AddTreeViewItem(const std::string& name, void* ptr){
		mMainWindow_WPF.AddTreeViewItem(name, ptr);
	}
	static void SetParentTreeViewItem(void* parent, void* child){
		mMainWindow_WPF.SetParentTreeViewItem(parent, child);
	}
	
	static void ChangeTreeViewName(void* ptr, std::string& name){
		mMainWindow_WPF.ChangeTreeViewName(ptr, name);
	}
	static void ClearTreeViewItem(void* ptr){
		if (ptr)mMainWindow_WPF.ClearTreeViewItem(ptr);
	}
	static void AddLog(const std::string& log){
		mMainWindow_WPF.AddLog(log);
	}

	static void ClearInspector(){
		mMainWindow_WPF.ClearAllComponentWindow();
	}
	static std::vector<InspectorDataSet> CreateInspector(){
		std::vector<InspectorDataSet> data;
		return data;
	}
	static void AddInspector(InspectorLabelDataSet* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Label, dataset));
	}
	static void AddInspector(TemplateInspectorDataSet<float>* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Float, dataset));
	}
	static void AddInspector(TemplateInspectorDataSet<bool>* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Bool, dataset));
	}
	static void AddInspector(TemplateInspectorDataSet<int>* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Int, dataset));
	}
	static void AddInspector(InspectorVector3DataSet* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Vector3, dataset));
	}
	static void AddInspector(InspectorVector2DataSet* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Vector2, dataset));
	}
	static void AddInspector(InspectorSlideBarDataSet* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::SlideBar, dataset));
	}
	static void AddInspector(TemplateInspectorDataSet<std::string>* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::String, dataset));
	}
	static void AddInspector(InspectorColorDataSet* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Color, dataset));
	}
	static void AddInspector(InspectorButtonDataSet* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::Button, dataset));
	}
	static void ViewInspector(const std::string& ComponentName,Component* comptr, std::vector<InspectorDataSet>& data){
		mMainWindow_WPF.CreateComponentWindow(ComponentName, (void*)comptr, data);
	}
	static void UpdateInspector(){
		mMainWindow_WPF.UpdateComponentWindow();
	}

	static void CreateContextMenu_AddComponent(const std::string& ComponentName){
		mMainWindow_WPF.CreateContextMenu_AddComponent(ComponentName);
	}

	static void SetMouseEvents(bool* focus,bool* l, bool* r, int* x, int* y, int *wx, int *wy){
		mMainWindow_WPF.SetMouseEvents(focus,l, r, x, y, wx, wy);
	}

	static void SetWPFCollBack(MyWindowMessage massage, const std::function<void(void*)>& collback){
		mWPFCollBacks[(int)massage] = collback;
	}

	

public:
	static HMODULE mhModuleWnd;
	static HWND mhWnd;
	HWND mDummyhWnd;
	HINSTANCE mhInstance;
	int mnCmdShow;

	static std::vector<const std::function<void(void*)>> mWPFCollBacks;
};

