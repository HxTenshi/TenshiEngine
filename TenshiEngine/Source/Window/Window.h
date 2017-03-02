#pragma once

#include <windows.h>
#include <string>

#include <CommCtrl.h>

#include <vector>

#include <functional>
#include "Application/Define.h"

#include "DebugSource/SystemLog.h"
#include "Input/InputManagerRapper.h"
#include "Game/Parts/enabled.h"

class WindowState{
public:
	static UINT mWidth;
	static UINT mHeight;
};
//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


#include "../CppWPFdll/CppWPFdll.h"

class Component;

class Window{
public:

#ifdef _ENGINE_MODE
	static Test::NativeFraction mMainWindow_WPF;
#endif
	static HWND mGameScreenHWND;
	Window(HINSTANCE hInstance, int nCmdShow)
		: mhInstance(hInstance)
		, mnCmdShow(nCmdShow)
	{

		_SYSTEM_LOG_H("ウィンドウの作成");
#ifdef _ENGINE_MODE
		mMainWindow_WPF.Initialize();

		HWND hWnd = NULL;
		while (!hWnd){
			hWnd = (HWND)mMainWindow_WPF.GetGameScreenHWND();
		}
		mhWnd = (HWND)mMainWindow_WPF.GetEditorHWND();
		mGameScreenHWND = hWnd;
#endif

	}
	int Init();
	void Update();

	void Release();

	static HWND GetMainHWND(){
		return mhWnd;
	}
	static HWND GetGameScreenHWND(){

		return mGameScreenHWND;
		//return mGameScreenWindow.GetHWND();
	}
	static bool IsActive(){
#ifdef _ENGINE_MODE
		return mInputManagerRapper.GetScreenFocus();
#else
		return GetActiveWindow() == mGameScreenHWND;
#endif
	}

	static void SetWindowTitle(const std::string& title){
		SetWindowText(mhWnd, title.c_str());
	}
	static void AddLog(const std::string& log){
#ifdef _ENGINE_MODE
		mMainWindow_WPF.AddLog(log);
#else
		(void)log;
#endif
	}

#ifdef _ENGINE_MODE
	template<class T>
	static void Deleter(T* ptr){
		mMainWindow_WPF.Deleter(ptr);
	}
	static void AddTreeViewItem(const std::string& name, void* ptr){
		mMainWindow_WPF.AddTreeViewItem(name, ptr);
	}
	static void AddEngineTreeViewItem(const std::string& name, void* ptr){
		mMainWindow_WPF.AddEngineTreeViewItem(name, ptr);
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
	static void SelectTreeViewItem(void* ptr){
		if (ptr)mMainWindow_WPF.SelectTreeViewItem(ptr);
	}


	static void ClearInspector(){
		mMainWindow_WPF.ClearAllComponentWindow();
	}
	static void ViewInspector(const std::string& ComponentName,Component* comptr, std::vector<InspectorDataSet>& data){
		mMainWindow_WPF.CreateComponentWindow(ComponentName, (void*)comptr, data);
	}
	static void ViewInspector(const std::string& ComponentName, Component* comptr, std::vector<InspectorDataSet>& data, TemplateInspectorDataSet<bool>* enable){

		mMainWindow_WPF.CreateComponentWindowUseEnable(ComponentName, (void*)comptr, data, enable);
	}
	static void UpdateInspector(){
		mMainWindow_WPF.UpdateComponentWindow();
	}

	static void CreateContextMenu_AddComponent(const std::string& ComponentName){
		mMainWindow_WPF.CreateContextMenu_AddComponent(ComponentName);
	}
	static void CreateContextMenu_CreateObject(const std::string& ObjectName, const std::string& FilePath){
		mMainWindow_WPF.CreateContextMenu_CreateObject(ObjectName, FilePath);
	}

	static void SetMouseEvents(bool* focus,bool* l, bool* r, int* x, int* y, int *wx, int *wy){
		mMainWindow_WPF.SetMouseEvents(focus,l, r, x, y, wx, wy);
	}
	static void SetEngineFocusEvent(bool* focus) {
		mMainWindow_WPF.SetEngineFocusEvent(focus);
	}

	static void SetWPFCollBack(MyWindowMessage massage, const std::function<void(void*)>& collback){
		mWPFCollBacks[(int)massage] = collback;
	}

#endif
	

public:
	//static HMODULE mhModuleWnd;
	static HWND mhWnd;
	HINSTANCE mhInstance;
	int mnCmdShow;

	static InputManagerRapper mInputManagerRapper;


#ifdef _ENGINE_MODE
	HWND mDummyhWnd;
	static std::vector<std::function<void(void*)>> mWPFCollBacks;
#endif
};

