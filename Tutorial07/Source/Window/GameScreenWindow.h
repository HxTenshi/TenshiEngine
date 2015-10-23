#pragma once

#include <Windows.h>
#include "ChildWindow.h"

class GameScreenWindow : public ChildWindow{
public:

	GameScreenWindow(){
	}
	~GameScreenWindow(){
	}

	void Create(Window* window) override;

};