#pragma once
class Actor;
class IGame{
public:
	virtual Actor* CreateActor(const char* prefab) = 0;
	virtual Actor* FindActor(const char* name) = 0;
	virtual void AddObject(Actor* actor) = 0;
	virtual void DestroyObject(Actor* actor) = 0;
};