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
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = mhInstance;
		wcex.hIcon = LoadIcon(mhInstance, (LPCTSTR)IDI_TUTORIAL1);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = "TutorialWindowClass";
		wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
		if (!RegisterClassEx(&wcex))
			return E_FAIL;

		// Create window
		RECT rc = { 0, 0, WindowState::mWidth, WindowState::mHeight };
		AdjustWindowRect(&rc, NULL, FALSE);
		mhWnd = CreateWindow("TutorialWindowClass", "Direct3D 11 Tutorial 7", NULL,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, mhInstance,
			NULL);
		if (!mhWnd)
			return E_FAIL;


		//InitCommonControls();

		//mGameScreenWindow.Create(this);
		//mInspectorWindow.Create(this);
		//mProjectWindow.Create(this);

		//ShowWindow(mhWnd, mnCmdShow);
		//UpdateWindow(mhWnd);

		//D&D‚Ì‹–‰Â
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
	static void AddInspector(InspectorSlideBarDataSet* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::SlideBar, dataset));
	}
	static void AddInspector(TemplateInspectorDataSet<std::string>* dataset, std::vector<InspectorDataSet>& data){
		data.push_back(InspectorDataSet(InspectorDataFormat::String, dataset));
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

	static void SetMouseEvents(bool* l, bool* r, int* x, int* y, int *wx, int *wy){
		mMainWindow_WPF.SetMouseEvents(l, r, x, y, wx, wy);
	}

	static void SetWPFCollBack(MyWindowMessage massage, const std::function<void(void*)>& collback){
		mWPFCollBacks[(int)massage] = collback;
	}

	

public:
	static HMODULE mhModuleWnd;
	static HWND mhWnd;
	HINSTANCE mhInstance;
	int mnCmdShow;

	static std::vector<const std::function<void(void*)>> mWPFCollBacks;
};

