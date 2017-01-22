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
#include "Script\GameObject.h"
#include "System.h"

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
	typedef shared_ptr<Actor> GameObjectPtr;
	typedef std::map<UniqueID, GameObjectPtr> ListMapType;
	typedef std::map<DrawStage, std::vector<std::function<void()>>> DrawListMapType;


	Game();
	~Game();

	static Game* Get();

	GameObject Instance(const std::string & base, bool undo = false);
	GameObject Instance(bool undo = false);
	GameObject Instance(GameObject base, bool undo = false);
	GameObject Instance(picojson::value base, bool undo = false, bool parentTop = true);
	GameObject Instance(PrefabAssetDataPtr base, bool undo = false);

	static void AddObject(GameObjectPtr actor, bool undoFlag = false, bool DelayInitialize = false);
	static void DestroyObject(GameObjectPtr actor, bool undoFlag = false);
	static void ActorMoveStage();
	static PxRigidActor* CreateRigitBody();
	static PxRigidActor* CreateRigitBodyEngine();
	static PhysX3Main* GetPhysX();
	static PhysXEngine* GetPhysXEngine();
	static void RemovePhysXActor(PxActor* act);
	static void RemovePhysXActorEngine(PxActor* act);
	static void AllDestroyObject();
	static void GetAllObject(const std::function<void(GameObject)>& collbak);
	static GameObject GetRootActor();
	static GameObject FindActor(Actor* actor);
#ifdef _ENGINE_MODE
	static GameObject FindEngineActor(Actor* actor);
#endif
	static GameObject FindNameActor(const char* name);
	static GameObject FindUID(UniqueID uid);
	static void AddDrawList(DrawStage stage, std::function<void()> func);
	static void SetUndo(Actor* actor);
	static void SetUndo(ICommand* command);
	static void SetMainCamera(CameraComponent* Camera);
	static CameraComponent* GetMainCamera();
	static RenderTarget GetMainViewRenderTarget();
	static void LoadScene(const std::string& FilePath);
	static DeltaTime* GetDeltaTime();
	static System* System();
	static std::vector<std::string>& GetLayerNames();

#ifdef _ENGINE_MODE
	static void AddEngineObject(GameObjectPtr actor);
	static bool IsGamePlay();
	void GameStop();
	static void SetMainCameraEngineUpdate(CameraComponent* Camera);

	void WindowParentSet(GameObject child);

	GameObject GetEngineRootActor();
	EditorCamera* GetEditorCamera();
#endif

	void ChangePlayGame(bool isPlay);
	void SaveScene();

	void Draw();

	void Update();
	void GamePlay();

	void ClearDrawList();
	void PlayDrawList(DrawStage Stage);


private:
	Game(const Game&) = delete;
	Game operator = (Game&) = delete;

	enum class ActorMove{
		Create,
		Create_DelayInitialize,
		Delete,
		Count,
	};
	//追加と削除ｇ
	std::queue<std::pair<ActorMove, GameObjectPtr>> mActorMoveList;
	//ゲームオブジェクトのリスト
	ListMapType mList;
	DrawListMapType mDrawList;
	static GameObjectPtr mRootObject;

	EngineDeltaTime mDeltaTime;
	static SystemHelper mSystemHelper;


#ifdef _ENGINE_MODE
	//ツリービューのアイテム削除に失敗したアクター
	std::list<Actor*> mTreeViewItem_ErrerClearList;
	static std::list<GameObjectPtr> mEngineObjects;

	static GameObjectPtr mEngineRootObject;

	SelectActor mSelectActor;
	EditorCamera mCamera;
	WorldGrid mWorldGrid;
	FPSChecker mFPS;
	//ProfileViewer mProfileViewer;
	CommandManager mCommandManager;

	bool mIsPlay;
#endif

	PhysX3Main* mPhysX3Main;


	CameraComponent* mMainCamera;
	CameraComponent* mMainCameraEngineUpdate;
	RenderTarget mMainViewRenderTarget;
	DeferredRendering m_DeferredRendering;
	PostEffectRendering mPostEffectRendering;

	static Scene m_Scene;


	ConstantBuffer<cbGameParameter> mCBGameParameter;
	ConstantBuffer<cbScreen> mCBScreen;


};