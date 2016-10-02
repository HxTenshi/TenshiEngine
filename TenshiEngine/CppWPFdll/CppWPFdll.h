// CppWPFdll.h

#pragma once

#ifdef _EXPORTING
#define CLASS_DECLSPEC    __declspec(dllexport)
#else
#define CLASS_DECLSPEC    __declspec(dllimport)
#endif
//void CreateWindowTest2(){
//
//}
//
//namespace CppWPFdll {
//
//	public ref class CppWPFdllTestClass
//	{
//	public:
//		static float Add(float a, float b);
//		static void CreateWindowTest();
//	};
//}

namespace Test {
	class WindowThreadCreator;
	class NativeFraction;
}

#include <vector>
#include "InspectorDataSet.h"

class CLASS_DECLSPEC Test::NativeFraction{

	WindowThreadCreator* mWindowThread;
public:

	NativeFraction();
	~NativeFraction();

	void Initialize();
	void Release();

	template<class T>
	void Deleter(T* ptr){
		delete ptr;
	}

	void ChangeTreeViewName(void* ptr, std::string& name);
	void CreateComponentWindow(const std::string& ComponentName, void* comptr, std::vector<InspectorDataSet>& data);
	void CreateComponentWindowUseEnable(const std::string& ComponentName, void* comptr, std::vector<InspectorDataSet>& data, TemplateInspectorDataSet<bool>* enable);
	void ClearAllComponentWindow();
	void ClearTreeViewItem(void* ptr);
	void UpdateComponentWindow();

	void CreateContextMenu_AddComponent(const std::string& ComponentName);
	void CreateContextMenu_CreateObject(const std::string& ObjectName, const std::string& FilePath);

	void AddTreeViewItem(const std::string& Name, void* ptr);
	void AddEngineTreeViewItem(const std::string& Name, void* ptr);

	void SetParentTreeViewItem(void* parent, void* child);
	void SelectTreeViewItem(void* ptr);

	void SetMouseEvents(bool* focus, bool* l, bool* r, int* x, int* y, int* wx, int* wy);
	void SetEngineFocusEvent(bool* focus);


	void AddLog(const std::string& log);
	
	void* GetGameScreenHWND() const;
	void* GetEditorHWND() const;
};
