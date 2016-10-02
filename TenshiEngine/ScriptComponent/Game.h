#pragma once
class Actor;
class PhysXEngine;
class DeltaTime;
class DebugEngine;
class System;
class DeltaTime;
#include "Engine/AssetTypes.h"
#include "Game/Types.h"

namespace Hx {
	GameObject GetRootActor();
	GameObject Instance();
	GameObject Instance(GameObject base);
	GameObject Instance(PrefabAssetDataPtr base);
	GameObject FindActor(const char* name);
	void DestroyObject(GameObject actor);
	void LoadScene(const std::string& FilePath);
	DebugEngine* Debug();
	PhysXEngine* PhysX();
	::DeltaTime* DeltaTime();
	::System* System();
	void Shutdown();
};