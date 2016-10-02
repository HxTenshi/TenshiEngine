#pragma once
class Actor;
class PhysXEngine;
class DeltaTime;
class DebugEngine;
class System;

#include "Engine/AssetTypes.h"

class IGame{
public:
	virtual GameObject GetRootActor() = 0;
	virtual GameObject Instance() = 0;
	virtual GameObject Instance(GameObject base) = 0;
	virtual GameObject Instance(PrefabAssetDataPtr base) = 0;
	virtual GameObject FindActor(const char* name) = 0;
	virtual void DestroyObject(GameObject actor) = 0;
	virtual void LoadScene(const std::string& FilePath) = 0;
	virtual DebugEngine* Debug() = 0;
	virtual PhysXEngine* PhysX() = 0;
	virtual DeltaTime* DeltaTime() = 0;
	virtual System* System() = 0;
	virtual void Shutdown() = 0;
};