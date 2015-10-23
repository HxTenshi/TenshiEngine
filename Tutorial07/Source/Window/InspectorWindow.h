#pragma once

#include <Windows.h>
#include "ChildWindow.h"
#include <list>

class Actor;

class InspectorEDIT : public ChildWindow{
public:
	InspectorEDIT(HWND hWnd, HINSTANCE hInstance, float* param, bool* fixFlag, int PosY);
	~InspectorEDIT();
	void Create(Window* window) override;
	LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;
private:
	float* mParam;
	bool* mFixFlag;

};

class InspectorWindow : public ChildWindow{
public:

	InspectorWindow():
		mCurrentAddress(NULL){
	}
	~InspectorWindow(){
		for (auto h : mList)delete h;
		mList.clear();
	}

	void Create(Window* window) override;

	void InsertConponentData(Actor* actor);

	void AddLabel(const char* text);
	void AddParam(float* param, bool* fixFlag);

private:
	HINSTANCE mhInstance;
	int mCurrentAddress;
	int mDataPosY;
	std::list<ChildWindow*> mList;
};