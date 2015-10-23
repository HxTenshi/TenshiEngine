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

	NativeFraction();
	~NativeFraction();

	void Initialize();
	void Release();

	void CreateComponentWindow(std::vector<InspectorDataSet>& data);
	void ClearAllComponentWindow();
	void UpdateComponentWindow();

	void AddTreeViewItem(std::string& Name, void* ptr);

	void SetMouseEvents(bool* l, bool* r, int* x, int* y);
	
	void* GetGameScreenHWND() const;
};
