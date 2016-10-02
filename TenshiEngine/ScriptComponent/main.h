
#ifndef _DLL_SCRIPT_COMPONENT_H_
#define _DLL_SCRIPT_COMPONENT_H_

#ifndef NULL
#define NULL 0
#endif

#define SERIALIZE


#pragma warning( push )
#pragma warning ( disable : 4838 )
#define INC_ACTOR "Game/Actor.h"
#define INC_GAMEOBJECT "Game/Script/GameObject.h"
#define INC_TRANCEFORM "Game/Component/TransformComponent.h"
#define INC_GAME "Game.h"
#define INC_INPUT "Input/Input.h"
#define INC_DELTA_TIME "Game/DeltaTime.h"
#define INC_PHYSX "PhysX/IPhysXEngine.h"
#define INC_DEBUG "engine/DebugEngine.h"
#define INC_SYSTEM "Game/System.h"

#include INC_ACTOR
#include INC_GAMEOBJECT
#include INC_TRANCEFORM
#include INC_GAME


#include <windows.h>
#include <map>
#include <string>
#include "MySTL/Reflection/MemberInfo_Data.h"

#include "XNAMath/xnamath.h"
#pragma warning( pop )

class IGame;
class ScriptComponent;
class IDllScriptComponent{
public:
	virtual ~IDllScriptComponent(){}
	virtual void Initialize(){}
	virtual void Start(){}
	virtual void Update(){}
	virtual void Finish(){}
	virtual void OnCollideBegin(GameObject target){ (void)target; }
	virtual void OnCollideEnter(GameObject target){ (void)target; }
	virtual void OnCollideExit(GameObject target){ (void)target; }

	typedef void(IDllScriptComponent::*Func0)();
	typedef void(IDllScriptComponent::*Func1)(GameObject);

public:
	GameObject gameObject;
	friend ScriptComponent;
};

void InitializeIGame(IGame* game);
IDllScriptComponent* CreateInstance(const char* ClassName);


void Function0(IDllScriptComponent* com, IDllScriptComponent::Func0 func);
void Function1(IDllScriptComponent* com, IDllScriptComponent::Func1 func, GameObject tar);

void ReleseInstance(IDllScriptComponent* p);

std::map<std::string, std::map<std::string, MemberInfo_Data>>* GetReflectionData();

#endif