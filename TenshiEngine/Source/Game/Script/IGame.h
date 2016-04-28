#pragma once
class Actor;
class PhysXEngine;
class DebugEngine;
class IGame{
public:
	virtual Actor* CreateActor(const char* prefab) = 0;
	virtual Actor* FindActor(const char* name) = 0;
	virtual void AddObject(Actor* actor) = 0;
	virtual void DestroyObject(Actor* actor) = 0;
	virtual void LoadScene(const std::string& FilePath) = 0;
	virtual DebugEngine* Debug() = 0;
	virtual PhysXEngine* PhysX() = 0;
};