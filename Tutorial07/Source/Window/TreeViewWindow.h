#pragma once

#include <windows.h>
#include "ChildWindow.h"

class TreeViewWindow: public ChildWindow {
public:
	TreeViewWindow(){
	}
	~TreeViewWindow(){

	}

	void Create(Window* window) override;

	void AddItem(void* item);
	void DeleteItem(void* item);

	void* GetSelectItem();

	bool IsActive();
};