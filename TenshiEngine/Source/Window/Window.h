#pragma once

#include <windows.h>
#include <string>

#include <CommCtrl.h>

#include <vector>

#include <functional>
#include "Application/Define.h"

#include "Engine/SystemLog.h"

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

	static void Release(){
		//if (mhModuleWnd)
		//	FreeLibrary(mhModuleWnd);
#ifdef _ENGINE_MODE
		mMainWindow_WPF.Release();
#endif
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
	static void CreateContextMenu_CreateObject(const std::string& ObjectName, const std::string& FilePath){
		mMainWindow_WPF.CreateContextMenu_CreateObject(ObjectName, FilePath);
	}

	static void SetMouseEvents(bool* focus,bool* l, bool* r, int* x, int* y, int *wx, int *wy){
		mMainWindow_WPF.SetMouseEvents(focus,l, r, x, y, wx, wy);
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


#ifdef _ENGINE_MODE
	HWND mDummyhWnd;
	static std::vector<const std::function<void(void*)>> mWPFCollBacks;
#endif
};

