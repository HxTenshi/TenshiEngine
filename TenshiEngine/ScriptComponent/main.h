
#ifndef _DLL_SCRIPT_COMPONENT_H_
#define _DLL_SCRIPT_COMPONENT_H_

#ifndef NULL
#define NULL 0
#endif

//#include "Game/Actor.h"
//#include "Game/Component.h"
#include <windows.h>
#include <map>
#include <string>
#include "MySTL\Reflection\MemberInfo_Data.h"

#include "XNAMath/xnamath.h"

class Actor;
class IGame;
class ScriptComponent;
class IDllScriptComponent{
public:
	virtual ~IDllScriptComponent(){}
	virtual void Initialize(){}
	virtual void Start(){}
	virtual void Update(){}
	virtual void Finish(){}
	virtual void OnCollide(Actor* target){ (void)target; }

	Actor* gameObject;
protected:
	IGame* game;
	friend ScriptComponent;
};

IDllScriptComponent* CreateInstance(const char* ClassName);

void ReleseInstance(IDllScriptComponent* p);

std::map<std::string, std::map<std::string, MemberInfo_Data>>* GetReflectionData();

#endif