#pragma once

#include "Game/Game.h"
#include "IGame.h"

//ゲームのスタティック関数の肩代わり
class SGame : public IGame{
public:
	Actor* CreateActor(const char* prefab)override{
		auto a = new Actor();
		if (!a->ImportDataAndNewID(prefab)){
			delete a;
			return NULL;
		}
		return a;
	}

	Actor* FindActor(const char* name)override{
		return Game::FindNameActor(name);
	}
	void AddObject(Actor* actor) override{
		Game::AddObject(actor);
	}
	void DestroyObject(Actor* actor) override{
		Game::DestroyObject(actor);

	}
};
