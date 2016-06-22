#pragma once

#include <map>
#include "Device/DirectX11Device.h"
#include "Graphic/RenderTarget/RenderTarget.h"

#include "Input/Input.h"
#include "Game/Component/Component.h"
#include "Actor.h"

#include "Sound/Sound.h"

#include "Engine/ICommand.h"

#include "Scene.h"

#include "PostEffectRendering.h"
#include "DeferredRendering.h"


#include "Engine/SelectActor.h"

#include "Game/EngineObject/FPSChecker.h"
#include "Game/EngineObject/EditorCamera.h"
#include "Game/EngineObject/ProfileViewer.h"
#include "Engine/WorldGrid.h"

#include "DeltaTime.h"

class CameraComponent;

enum class DrawStage{
	Init,
	Diffuse,
	Depth,
	Normal,
	Light,
	Particle,
	PostEffect,
	Forward,
	Engine,
	UI,
	Count
};
typedef void(*DrawFunc)();

class PhysX3Main;
class PhysXEngine;
namespace physx{
	class PxActor;
	class PxRigidActor;
}
using namespace physx;

class Game{
public:
	Game();
	~Game();

	static void AddObject(Actor* actor);
	static void DestroyObject(Actor* actor);
	static void ActorMoveStage();
	static PxRigidActor* CreateRigitBody();
	static PxRigidActor* CreateRigitBodyEngine();
	static PhysX3Main* GetPhysX();
	static PhysXEngine* GetPhysXEngine();
	static void RemovePhysXActor(PxActor* act);
	static void RemovePhysXActorEngine(PxActor* act);
	static void AllDestroyObject();
	static void GetAllObject(const std::function<void(Actor*)>& collbak);
	static Actor* GetRootActor();
	static Actor* FindActor(Actor* actor);
	static Actor* FindNameActor(const char* name);
	static Actor* FindUID(UINT uid);
	static void AddDrawList(DrawStage stage, std::function<void()> func);
	static void SetUndo(ICommand* command);
	static void SetMainCamera(CameraComponent* Camera);
	static CameraComponent* GetMainCamera();
	static RenderTarget GetMainViewRenderTarget();
	static void LoadScene(const std::string& FilePath);
	static DeltaTime* GetDeltaTime();

#ifdef _ENGINE_MODE
	static void AddEngineObject(Actor* actor);
	static bool IsGamePlay();
	void GameStop();
#endif

	void ChangePlayGame(bool isPlay);
	void SaveScene();

	void Draw();

	void Update();
	void GamePlay();

	void ClearDrawList();
	void PlayDrawList(DrawStage Stage);

	typedef std::map<UINT, Actor*> ListMapType;
	typedef std::map<DrawStage, std::vector<std::function<void()>>> DrawListMapType;

private:
	Game(const Game&);
	Game operator = (Game&);

	enum class ActorMove{
		Create,
		Delete,
		Count,
	};
	//追加と削除
	std::queue<std::pair<ActorMove, Actor*>> mActorMoveList;
	//ゲームオブジェクトのリスト
	ListMapType mList;
	DrawListMapType mDrawList;
	static Actor* mRootObject;

	EngineDeltaTime mDeltaTime;

#ifdef _ENGINE_MODE
	//ツリービューのアイテム削除に失敗したアクター
	std::list<Actor*> mTreeViewItem_ErrerClearList;

	static Actor* mEngineRootObject;

	SelectActor mSelectActor;
	EditorCamera mCamera;
	WorldGrid mWorldGrid;
	FPSChecker mFPS;
	ProfileViewer mProfileViewer;
	CommandManager mCommandManager;

	bool mIsPlay;
#endif

	PhysX3Main* mPhysX3Main;


	CameraComponent* mMainCamera;
	RenderTarget mMainViewRenderTarget;
	DeferredRendering m_DeferredRendering;
	PostEffectRendering mPostEffectRendering;

	static Scene m_Scene;


	ConstantBuffer<cbGameParameter> mCBGameParameter;
	ConstantBuffer<cbScreen> mCBScreen;


};