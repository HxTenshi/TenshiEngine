#pragma once

#include "Game/Game.h"
#include "Game/DeltaTime.h"
#include "IGame.h"
#include "PhysX/PhysX3.h"
#include "Engine/Debug.h"
#include "Engine/DebugEngine.h"
#include "Engine/AssetFile/Prefab/PrefabFileData.h"

//ゲームのスタティック関数の肩代わり
class SGame : public IGame{
public:
	Actor* CreateActor(const char* prefab)override{


		auto a = new Actor();

		PrefabAssetDataPtr mPrefabAsset;
		AssetDataBase::Instance(prefab, mPrefabAsset);
		if (mPrefabAsset){
			if (mPrefabAsset->GetFileData()){
				auto val = mPrefabAsset->GetFileData()->GetParam();
				a->ImportDataAndNewID(*val);
				return a;
			}

		}


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
	
private:
	DebugSystem mDebugEngine;
};
