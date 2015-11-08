
#ifndef _DLL_SCRIPT_COMPONENT_H_
#define _DLL_SCRIPT_COMPONENT_H_

#ifndef NULL
#define NULL 0
#endif

//#include "Game/Actor.h"
//#include "Game/Component.h"
#include "Input/Input.h"
#include <xnamath.h>
#include <map>
#include <typeinfo>
#include <functional>

class Actor;
class IDllScriptComponent{
public:
	virtual void Update(Actor* This) = 0;
	virtual ~IDllScriptComponent(){}
};

IDllScriptComponent* CreateInstance(const char* ClassName);

void ReleseInstance(IDllScriptComponent* p);

#endif