
#ifndef _DLL_SCRIPT_H_
#define _DLL_SCRIPT_H_
#include "main.h"

class DllScriptComponent :public IDllScriptComponent{
public:
	DllScriptComponent();
	virtual void Update(Actor* This)override;

private:
	int timer;
	int timer2;
};

#endif