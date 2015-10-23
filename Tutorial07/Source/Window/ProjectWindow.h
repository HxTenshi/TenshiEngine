#pragma once

#include <windows.h>
#include "ChildWindow.h"

class FolderWindow;

class FolderTreeWindow : public ChildWindow {
public:
	FolderTreeWindow(FolderWindow* folderWindow)
		:mFolderWindow(folderWindow){
	}
	~FolderTreeWindow(){

	}

	void Create(Window* window) override;

	LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	FolderWindow* mFolderWindow;
};
#include <string>
class FolderWindow : public ChildWindow {
public:
	FolderWindow(){
	}
	~FolderWindow(){

	}

	void Create(Window* window) override;

	void OpenFolder(const std::string& pass);

	LRESULT CALLBACK MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	std::string mOpenFolderPass;
};

class ProjectWindow : public ChildWindow {
public:
	ProjectWindow():
		mFolderTreeWindow(&mFolderWindow){
	}
	~ProjectWindow(){

	}

	void Create(Window* window) override;

private:
	FolderTreeWindow mFolderTreeWindow;
	FolderWindow mFolderWindow;
};