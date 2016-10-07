#pragma once
class Actor;
class PhysXEngine;
class DeltaTime;
class DebugEngine;
class System;
class DeltaTime;
#include "Game/Types.h"
#include "Engine/Assets.h"

namespace Hx {
	GameObject GetRootActor();
	GameObject Instance();
	GameObject Instance(GameObject base);
	GameObject Instance(PrefabAsset base);
	GameObject FindActor(const char* name);
	void DestroyObject(GameObject actor);
	void LoadScene(const std::string& FilePath);
	DebugEngine* Debug();
	PhysXEngine* PhysX();
	::DeltaTime* DeltaTime();
	::System* System();
	void Shutdown();
};