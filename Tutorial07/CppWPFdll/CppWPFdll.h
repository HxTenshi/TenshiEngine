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
	class Frac1;
	class NativeFraction;
}

#include <vector>
#include "InspectorDataSet.h"

class CLASS_DECLSPEC Test::NativeFraction{

	Frac1* frac;
public:
	int i;
	char c;

	NativeFraction();
	~NativeFraction();

	void Initialize();
	void Release();

	template<class T>
	void Deleter(T* ptr){
		delete ptr;
	}

	void ChangeTreeViewName(void* ptr, std::string& name);
	void CreateComponentWindow(const std::string& ComponentName, std::vector<InspectorDataSet>& data);
	void ClearAllComponentWindow();
	void ClearTreeViewItem(void* ptr);
	void UpdateComponentWindow();

	void AddTreeViewItem(const std::string& Name, void* ptr);
	void SetParentTreeViewItem(void* parent, void* child);

	void SetMouseEvents(bool* l, bool* r, int* x, int* y, int* wx, int* wy);
	
	void* GetGameScreenHWND() const;
};
