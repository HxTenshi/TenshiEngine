
#ifndef _DLL_SCRIPT_COMPONENT_H_
#define _DLL_SCRIPT_COMPONENT_H_

#ifndef NULL
#define NULL 0
#endif

//#include "Game/Actor.h"
//#include "Game/Component.h"
#include <windows.h>

#include "XNAMath/xnamath.h"

class Actor;
class IGame;
class ScriptComponent;
class IDllScriptComponent{
public:
	virtual ~IDllScriptComponent(){}
	virtual void Update(){}
	virtual void OnCollide(Actor* target){}

	Actor* gameObject;
protected:
	IGame* game;
	friend ScriptComponent;
};

IDllScriptComponent* CreateInstance(const char* ClassName);

void ReleseInstance(IDllScriptComponent* p);

#endif