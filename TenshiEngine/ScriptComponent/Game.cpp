#include "Game.h"

#include "Game/Script/IGame.h"
#include "Engine/Assets.h"
IGame* g_IGame = NULL;

void InitializeIGame(IGame* game) {
	g_IGame = game;
}

namespace Hx {
	GameObject GetRootActor() {
		return g_IGame->GetRootActor();
	}
	GameObject Instance() {
		return g_IGame->Instance();
	}
	GameObject Instance(GameObject base) {
		return g_IGame->Instance(base);
	}
	GameObject Instance(PrefabAsset base) {
		return g_IGame->Instance((PrefabAssetDataPtr)base.m_Ptr);
	}
	GameObject FindActor(const char* name){
		return g_IGame->FindActor(name);
	}
	void DestroyObject(GameObject actor) {
		g_IGame->DestroyObject(actor);
	}
	void LoadScene(const std::string& FilePath) {
		g_IGame->LoadScene(FilePath);
	}
	DebugEngine* Debug() {
		return g_IGame->Debug();
	}
	PhysXEngine* PhysX() {
		return g_IGame->PhysX();
	}
	::DeltaTime* DeltaTime() {
		return g_IGame->DeltaTime();
	}
	::System* System() {
		return g_IGame->System();
	}
	void Shutdown() {
		g_IGame->Shutdown();
	}
};