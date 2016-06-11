
#ifndef _DLL_SCRIPT_COMPONENT_H_
#define _DLL_SCRIPT_COMPONENT_H_

#ifndef NULL
#define NULL 0
#endif

#define SERIALIZE



#define INC_ACTOR "Game/Actor.h"
#define INC_GAME "Game/Script/IGame.h"
#define INC_INPUT "Input/Input.h"
#define INC_DELTA_TIME "Game/DeltaTime.h"
#define INC_PHYSX "PhysX/IPhysXEngine.h"
#define INC_DEBUG "engine/DebugEngine.h"


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
	virtual void OnCollideBegin(Actor* target){ (void)target; }
	virtual void OnCollideEnter(Actor* target){ (void)target; }
	virtual void OnCollideExit(Actor* target){ (void)target; }

	typedef void(IDllScriptComponent::*Func0)();
	typedef void(IDllScriptComponent::*Func1)(Actor*);

public:
	Actor* gameObject;
	IGame* game;
	friend ScriptComponent;
};

IDllScriptComponent* CreateInstance(const char* ClassName);


void Function0(IDllScriptComponent* com, IDllScriptComponent::Func0 func);
void Function1(IDllScriptComponent* com, IDllScriptComponent::Func1 func, Actor* tar);

void ReleseInstance(IDllScriptComponent* p);

std::map<std::string, std::map<std::string, MemberInfo_Data>>* GetReflectionData();

#endif