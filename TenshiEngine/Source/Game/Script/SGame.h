#pragma once

#include "Game/Game.h"
#include "Game/DeltaTime.h"
#include "IGame.h"
#include "PhysX/PhysX3.h"
#include "Engine/Debug.h"
#include "Engine/DebugEngine.h"
#include "Engine/AssetFile/Prefab/PrefabFileData.h"
#include "Game/System.h"
#include "Application/shutdown.h"

//ゲームのスタティック関数の肩代わり
class SGame : public IGame{
public:
	GameObject GetRootActor()override{
		return Game::GetRootActor();
	}
	GameObject Instance(GameObject base)override{

		return Game::Get()->Instance(base);
	}

	GameObject Instance(PrefabAssetDataPtr base) override{

		return Game::Get()->Instance(base);
	}

	GameObject Instance() override{

		return Game::Get()->Instance();
	}

	GameObject FindActor(const char* name)override{
		return Game::FindNameActor(name);
	}
	void DestroyObject(GameObject actor) override{
		Game::DestroyObject(actor);

	}


	void LoadScene(const std::string& FilePath) override{
		Game::LoadScene(FilePath);
	}
	DebugEngine* Debug() override{
		return &mDebugEngine;
	}

	PhysXEngine* PhysX() override{
		return Game::GetPhysXEngine();
	}

	::DeltaTime* DeltaTime(){
		return Game::GetDeltaTime();
	}
	::System* System(){
		return Game::System();
	}

	void Shutdown(){
#ifdef _ENGINE_MODE
		Game::Get()->ChangePlayGame(false);
#else
		App::Shutdown();
#endif
	}
	
private:

	DebugSystem mDebugEngine;
};
