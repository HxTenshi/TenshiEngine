#include "Game/Game.h"

#include "PhysX/PhysX3.h"
#include "Game/Component/ComponentFactory.h"

#include "Game/Component/TransformComponent.h"
#include "Game/Component/ScriptComponent.h"
#include "Game/Component/CameraComponent.h"

#include <stack>

#include "Engine/AssetFile/Prefab/PrefabFileData.h"

#include "Game/RenderingSystem.h"

#include "Engine/ModelConverter.h"


static std::stack<int> gIntPtrStack;

static Game::ListMapType* gpList;
static Game::DrawListMapType *gDrawList;
static PhysX3Main* gpPhysX3Main;
static CommandManager* gCommandManager;
static CameraComponent** gMainCamera;
//
Actor* Game::mRootObject;
#ifdef _ENGINE_MODE
static SelectActor* gSelectActor;
Actor* Game::mEngineRootObject;
#endif
Game* mGame = NULL;
Scene Game::m_Scene;
DeltaTime* gpDeltaTime;

#ifdef _ENGINE_MODE
static bool gIsPlay;
#endif

#include <vector>

#ifdef _ENGINE_MODE
//ツリービューが完成するまで繰り返す関数
std::function<void()> CreateSetParentTreeViewItemColl(Actor* par, Actor* chil){
	return [=](){
		if (par->mTreeViewPtr){
			Window::SetParentTreeViewItem(par->mTreeViewPtr, chil->mTreeViewPtr);
		}
		else{
			chil->SetUpdateStageCollQueue(CreateSetParentTreeViewItemColl(par, chil));
		}
	};
}
#endif

#include "Engine/AssetLoader.h"

Game::Game(){
	_SYSTEM_LOG_H("ゲームシーンの初期化");
	mGame = this;
	mMainCamera = NULL;

	gpDeltaTime = &mDeltaTime;
#ifdef _ENGINE_MODE
	gSelectActor = &mSelectActor;

	mIsPlay = false;
#endif

	HRESULT hr = S_OK;

#ifdef _ENGINE_MODE
	ScriptManager::ReCompile();

	{
		_SYSTEM_LOG_H("コンテキストメニューの初期化");
		auto coms = ComponentFactory::GetComponents();
		for (auto& com : coms){
			auto name = com.first;
			auto str = name.substr(6);
			Window::CreateContextMenu_AddComponent(str);
		}

		Window::CreateContextMenu_CreateObject("Box", "EngineResource/box.prefab");
		Window::CreateContextMenu_CreateObject("Texture", "EngineResource/new Texture");
	}
#endif

	hr = mMainViewRenderTarget.Create(WindowState::mWidth, WindowState::mHeight, DXGI_FORMAT_R11G11B10_FLOAT);
	if (FAILED(hr)){
		//MessageBox(NULL, "RenderTarget Create Error.", "Error", MB_OK);
	}

	mPhysX3Main = new PhysX3Main();
	mPhysX3Main->InitializePhysX();
	gpPhysX3Main = mPhysX3Main;

	m_DeferredRendering.Initialize();
	mPostEffectRendering.Initialize();

	gpList = &mList;
	gDrawList = &mDrawList;
	gMainCamera = &mMainCamera;

#ifdef _ENGINE_MODE
	gCommandManager = &mCommandManager;
	gIsPlay = mIsPlay;
#endif



	mRootObject = new Actor();
	mRootObject->mTransform = mRootObject->AddComponent<TransformComponent>();
	mRootObject->Initialize();
	mRootObject->Start();

#ifdef _ENGINE_MODE
	mEngineRootObject = new Actor();
	mEngineRootObject->mTransform = mEngineRootObject->AddComponent<TransformComponent>();
	mEngineRootObject->Initialize();
	mEngineRootObject->Start();

	mCamera.Initialize();
	mSelectActor.Initialize();
#endif

#ifdef _ENGINE_MODE
	LoadScene("./Assets/Scene_.scene");
#else
	LoadScene("./Assets/Scenes/Title.scene");
#endif

	mCBGameParameter = ConstantBuffer<cbGameParameter>::create(11);
	mCBGameParameter.mParam.Time = XMFLOAT4(0, 0, 0, 0);

	mCBScreen = ConstantBuffer<cbScreen>::create(13);
	mCBScreen.mParam.ScreenSize = XMFLOAT2((float)WindowState::mWidth,(float)WindowState::mHeight);
	mCBScreen.mParam.NULLss = XMFLOAT2(0,0);
#ifdef _ENGINE_MODE
	Window::SetWPFCollBack(MyWindowMessage::StackIntPtr, [&](void* p)
	{
		gIntPtrStack.push((int)p);
	});
	Window::SetWPFCollBack(MyWindowMessage::ReturnTreeViewIntPtr, [&](void* p)
	{
		int intptr = gIntPtrStack.top();
		auto act = ((Actor*)intptr);
		gIntPtrStack.pop();

		//削除失敗リストから検索して削除
		bool remove = false;
		mTreeViewItem_ErrerClearList.remove_if([&](Actor* tar){
			bool f = tar == act;
			if (f){
				Window::ClearTreeViewItem(p);
				remove = true;
				return true;
			}
			return false;
		});
		if (remove)return;

		act->mTreeViewPtr = p;
		//ツリービューで親子関係のセット関数
		if (auto par = act->mTransform->GetParent()){
			if (par == mRootObject)return;

			//ツリービューが完成するまで繰り返す関数
			auto coll = CreateSetParentTreeViewItemColl(par, act);
			//とりあえず１回実行
			coll();
		}
			
	});

	Window::SetWPFCollBack(MyWindowMessage::SelectActor, [&](void* p)
	{
		auto act = ((Actor*)p);
		mSelectActor.SetSelect(act, true);
	});
	Window::SetWPFCollBack(MyWindowMessage::ActorDoubleClick, [&](void* p)
	{
		auto act = ((Actor*)p);
		mCamera.GoActorPosition(act);
	});
	Window::SetWPFCollBack(MyWindowMessage::SetActorParent, [&](void* p)
	{
		int intptr = gIntPtrStack.top();
		gIntPtrStack.pop();
		auto parent = ((Actor*)intptr);
		if (!parent){
			parent = mRootObject;
		}
		auto act = ((Actor*)p);
		act->mTransform->SetParentWorld(parent);
		SetUndo(act);
	});
	Window::SetWPFCollBack(MyWindowMessage::ActorDestroy, [&](void* p)
	{
		Game::DestroyObject((Actor*)p, true);
		mSelectActor.SetSelect(NULL, true);
	});

	Window::SetWPFCollBack(MyWindowMessage::AddComponent, [&](void* p)
	{
		std::string* s = (std::string*)p;
		if (auto actor = mSelectActor.GetSelectOne()){
			auto temp = ComponentFactory::Create(*s);
			if (temp){
				actor->AddComponent(temp);
			}
			Window::ClearInspector();
			actor->CreateInspector();

		}
		Window::Deleter(s);
	});
	Window::SetWPFCollBack(MyWindowMessage::RemoveComponent, [&](void* p)
	{
		if (auto actor = mSelectActor.GetSelectOne()){
			actor->RemoveComponent((Component*)p);
			Window::ClearInspector();
			actor->CreateInspector();
		}
	});

	Window::SetWPFCollBack(MyWindowMessage::ChangeParamComponent, [&](void* p)
	{
		auto coll = (std::function<void()>*)p;
		(*coll)();
		//delete coll;
	});

	Window::SetWPFCollBack(MyWindowMessage::CreateModelConvert, [&](void* p)
	{
		std::string *s = (std::string*)p;
		ModelConverter::Comvert(*s);
		//AssetLoader loader;
		//loader.LoadFile(*s);
		Window::Deleter(s);
	});

	Window::SetWPFCollBack(MyWindowMessage::CreatePrefabToActor, [&](void* p)
	{
		std::string *s = (std::string*)p;
		auto a = new Actor();
		if (!a->ImportDataAndNewID(*s)){
			delete a;
		}
		else{
			AddObject(a,true);
		}
		Window::Deleter(s);
	});

	Window::SetWPFCollBack(MyWindowMessage::CreateActorToPrefab, [&](void* p)
	{

		int intptr = gIntPtrStack.top();
		gIntPtrStack.pop();
		auto obj = ((Actor*)intptr);


		std::string *s = (std::string*)p;
		obj->ExportData("Assets", *s,true);

		AssetDataBase::FileUpdate(("Assets/"+*s+".json").c_str());

		Window::Deleter(s);
	});

	Window::SetWPFCollBack(MyWindowMessage::PlayGame, [&](void* p)
	{
		(void)p;
		ChangePlayGame(true);
		//スクリプトのインスペクターがおかしくなる
		mSelectActor.ReCreateInspector();
	});
	Window::SetWPFCollBack(MyWindowMessage::StopGame, [&](void* p)
	{
		(void)p;
		ChangePlayGame(false);
		//スクリプトのインスペクターがおかしくなる
		mSelectActor.ReCreateInspector();
	});
	Window::SetWPFCollBack(MyWindowMessage::ScriptCompile, [&](void* p)
	{
		(void)p;
		ScriptManager::ReCompile();
		mSelectActor.ReCreateInspector();
	});
	Window::SetWPFCollBack(MyWindowMessage::CreateScriptFile, [&](void* p)
	{
		std::string *s = (std::string*)p;
		if (*s != ""){
			ScriptManager::CreateScriptFile(*s);
		}

		Window::Deleter(s);
	});
	Window::SetWPFCollBack(MyWindowMessage::CreateAssetFile, [&](void* p)
	{
		std::string *s = (std::string*)p;
		if (*s != ""){
			auto ex = behind_than_find_last_of(*s, ".");
			if (ex == "pxmaterial"){
				File f("Assets/"+*s);
				f.FileCreate();
				f.Out(0.0f);
				f.Out(0.0f);
				f.Out(0.0f);
			}
		}

		Window::Deleter(s);
	});
	Window::SetWPFCollBack(MyWindowMessage::SaveScene, [&](void* p)
	{
		(void)p;
		SaveScene();
	});
	Window::SetWPFCollBack(MyWindowMessage::SelectAsset, [&](void* p)
	{
		std::string *s = (std::string*)p;
		mSelectActor.SetSelectAsset(NULL, "");

		PrefabAssetDataPtr data;
		AssetDataBase::Instance(s->c_str(), data);
		if (data){
			mSelectActor.SetSelectAsset(data->GetFileData()->GetActor(), s->c_str());
		}
		else{
			mSelectActor.SetSelectAsset(NULL, s->c_str());
		}
		Window::Deleter(s);
	});

	Window::SetWPFCollBack(MyWindowMessage::OpenAsset, [&](void* p)
	{
		std::string *s = (std::string*)p;

		if ((*s).find(".scene\0") != (*s).npos){
			ChangePlayGame(false);
			AllDestroyObject();
			LoadScene(*s);
			mCommandManager.Clear();
		}
		Window::Deleter(s);
	});
#endif
}

Game::~Game(){

	ChangePlayGame(false);

	//for (auto& act : mList){
	//	DestroyObject(act.second);
	//}

	ActorMoveStage();
	TransformComponent* t = (TransformComponent*)mRootObject->mTransform.Get();
	t->AllChildrenDestroy();
#ifdef _ENGINE_MODE
	TransformComponent* t2 = (TransformComponent*)mEngineRootObject->mTransform.Get();
	t2->AllChildrenDestroy();

#endif

	ActorMoveStage();
	delete mRootObject;
#ifdef _ENGINE_MODE
	delete mEngineRootObject;
	mSelectActor.Finish();
#endif


	delete mPhysX3Main;
	mPhysX3Main = NULL;
	gpPhysX3Main = NULL;

	mMainViewRenderTarget.Release();

}
//static
void Game::AddObject(Actor* actor, bool undoFlag){
	if (!actor->mTransform){
		delete actor;
		return;
	}
	if (actor->GetUniqueID() == 0){
		actor->CreateNewID();
	}

	gpList->insert(std::pair<UINT, Actor*>(actor->GetUniqueID(), actor));
	actor->Initialize();

	mGame->mActorMoveList.push(std::make_pair(ActorMove::Create, actor));

	for (auto child : actor->mTransform->Children()){
		AddObject(child);
	}
#ifdef _ENGINE_MODE
	if (undoFlag){
		gCommandManager->SetUndo(new ActorDestroyUndoCommand(actor));
		gCommandManager->SetUndo(actor);
	}
#endif
}
#ifdef _ENGINE_MODE
//static
void Game::AddEngineObject(Actor* actor){
	if (!actor->mTransform){
		delete actor;
		return;
	}

	actor->Initialize();
	actor->Start();
	if (!actor->mTransform->GetParent()){
		actor->mTransform->SetParent(mEngineRootObject);
	}
}
#endif
//static
void Game::DestroyObject(Actor* actor, bool undoFlag){
	if (!actor)return;
	auto desnum = gpList->erase(actor->GetUniqueID());
	if (!desnum)return;
	mGame->mActorMoveList.push(std::make_pair(ActorMove::Delete, actor));
#ifdef _ENGINE_MODE
	if (undoFlag){
		gCommandManager->SetUndo(actor);
		gCommandManager->SetUndo(new ActorDestroyUndoCommand(actor));
	}
#endif
}

//static
void Game::ActorMoveStage(){
	while (!mGame->mActorMoveList.empty()){
		auto& tar = mGame->mActorMoveList.front();
		mGame->mActorMoveList.pop();
		auto actor = tar.second;
		if (tar.first == ActorMove::Delete){

#ifdef _ENGINE_MODE


			mGame->mSelectActor.ReleaseSelect(actor);
			
			//ツリービューが作成されていれば
			if (actor->mTreeViewPtr){
				Window::ClearTreeViewItem(actor->mTreeViewPtr);
			}
			//ツリービューが作成される前なら
			else{
				//削除リストに追加
				mGame->mTreeViewItem_ErrerClearList.push_back(actor);
			}
			actor->mTreeViewPtr = NULL;
#endif
			TransformComponent* t = (TransformComponent*)actor->mTransform.Get();
			t->AllChildrenDestroy();
			actor->Finish();

			delete actor;

		}
		else{

#ifdef _ENGINE_MODE
			Window::AddTreeViewItem(actor->Name(), actor);
#endif

			actor->Start();

			if (!actor->mTransform->GetParent()){
				actor->mTransform->SetParent(mRootObject);
			}
		}
	}
}

//static
PxRigidActor* Game::CreateRigitBody(){
	return gpPhysX3Main->createBody();
}
PxRigidActor* Game::CreateRigitBodyEngine(){
	return gpPhysX3Main->createBodyEngine();
}
PhysX3Main* Game::GetPhysX(){
	return gpPhysX3Main;
}
PhysXEngine* Game::GetPhysXEngine(){
	return gpPhysX3Main;
}
DeltaTime* Game::GetDeltaTime(){
	return gpDeltaTime;
}
void Game::RemovePhysXActor(PxActor* act){
	return gpPhysX3Main->RemoveActor(act);
}
void Game::RemovePhysXActorEngine(PxActor* act){
	return gpPhysX3Main->RemoveActorEngine(act);
}
Actor* Game::GetRootActor(){
	return mRootObject;
}
Actor* Game::FindActor(Actor* actor){

	for (auto& act : (*gpList)){
		if (act.second == actor){
			return act.second;
		}
	}
	return NULL;
}

Actor* Game::FindNameActor(const char* name){

	for (auto& act : (*gpList)){
		if (act.second->Name() == name){
			return act.second;
		}
	}
	return NULL;
}

Actor* Game::FindUID(UINT uid){
	auto get = (*gpList).find(uid);
	if (get == (*gpList).end())return NULL;
	return get->second;
}

void Game::AddDrawList(DrawStage stage, std::function<void()> func){
	(*gDrawList)[stage].push_back(func);
}

void Game::SetUndo(Actor* actor){
#ifdef _ENGINE_MODE
	gSelectActor->PushUndo();
#endif
	gCommandManager->SetUndo(actor);
}

void Game::SetUndo(ICommand* command){
	gCommandManager->SetUndo(command);
}

void Game::SetMainCamera(CameraComponent* Camera){
	*gMainCamera = Camera;
}
CameraComponent* Game::GetMainCamera(){
	return *gMainCamera;
}
RenderTarget Game::GetMainViewRenderTarget(){
	return mGame->mMainViewRenderTarget;
}
#ifdef _ENGINE_MODE
bool Game::IsGamePlay(){
	return gIsPlay;
}
#endif

void Game::LoadScene(const std::string& FilePath){

	((EngineDeltaTime*)gpDeltaTime)->Reset();

	TransformComponent* t = (TransformComponent*)mRootObject->mTransform.Get();
	//t->AllChildrenDestroy();

	auto children = t->Children();
	for (auto child : children){
		Game::DestroyObject(child);
	}

	m_Scene.LoadScene(FilePath);
}



void Game::ChangePlayGame(bool isPlay){

#ifdef _ENGINE_MODE
	if (mIsPlay == isPlay)return;

	mIsPlay = isPlay;
	gIsPlay = mIsPlay;
	if (isPlay){

		mDeltaTime.Reset();

		m_Scene.MemorySaveScene();

		for (auto& act : *gpList){
			act.second->Initialize_Script();
		}
		for (auto& act : *gpList){
			act.second->Start_Script();
		}
	}
	else{

		mSelectActor.SetSelect(NULL);

		AllDestroyObject();

		m_Scene.MemoryLoadScene();
	}
#else
	(void)isPlay;
#endif
}

#ifdef _ENGINE_MODE
bool g_DebugRender = true;
#endif

void Game::Draw(){
	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	if (!mMainCamera){

		Device::mRenderTargetBack->ClearView(render->m_Context);
		mMainViewRenderTarget.ClearView(render->m_Context);
		ClearDrawList();
		return;
	}
	//ここでもアップデートしてる（仮処理）
	mMainCamera->Update();
	mMainCamera->VSSetConstantBuffers(render->m_Context);
	mMainCamera->PSSetConstantBuffers(render->m_Context);
	mMainCamera->GSSetConstantBuffers(render->m_Context);

	//Device::mRenderTargetBack->ClearView(Device::mpImmediateContext);
	Device::mRenderTargetBack->ClearDepth(render->m_Context);
	//mMainViewRenderTarget.ClearView();
	

	render->PushSet(DepthStencil::Preset::DS_All_Less);
	render->PushSet(Rasterizer::Preset::RS_Back_Solid);

	const RenderTarget* r[1] = { &mMainViewRenderTarget };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], Device::mRenderTargetBack);

	//mMainCamera->ScreenClear();

	mCBGameParameter.mParam.Time.x++;
	mCBGameParameter.mParam.Time.y += mDeltaTime.GetDeltaTime();
	mCBGameParameter.mParam.Time.z = mDeltaTime.GetDeltaTime();
	mCBGameParameter.UpdateSubresource(render->m_Context);
	mCBGameParameter.VSSetConstantBuffers(render->m_Context);
	mCBGameParameter.PSSetConstantBuffers(render->m_Context);
	mCBGameParameter.GSSetConstantBuffers(render->m_Context);
	mCBGameParameter.CSSetConstantBuffers(render->m_Context);

	mCBScreen.UpdateSubresource(render->m_Context);
	mCBScreen.CSSetConstantBuffers(render->m_Context);
	mCBScreen.GSSetConstantBuffers(render->m_Context);
	mCBScreen.VSSetConstantBuffers(render->m_Context);
	mCBScreen.PSSetConstantBuffers(render->m_Context);


	//const RenderTarget* r[1] = { &mMainViewRenderTarget };
	//RenderTarget::SetRendererTarget((UINT)1, r[0], Device::mRenderTargetBack);

	//今は意味なし
	PlayDrawList(DrawStage::Depth);


	ID3D11ShaderResourceView *const pNULL[4] = { NULL, NULL, NULL, NULL };
	render->m_Context->PSSetShaderResources(0, 4, pNULL);
	ID3D11SamplerState *const pSNULL[4] = { NULL, NULL, NULL, NULL };
	render->m_Context->PSSetSamplers(0, 4, pSNULL);

	PlayDrawList(DrawStage::Init);

#ifdef _ENGINE_MODE

	if( Input::Trigger(KeyCoord::Key_F1)){
		g_DebugRender = !g_DebugRender;
	}

	if (!g_DebugRender){

#endif

		m_DeferredRendering.ShadowDepth_Buffer_Rendering(render, [&](){
			PlayDrawList(DrawStage::Diffuse);
		});

		mMainCamera->VSSetConstantBuffers(render->m_Context);
		mMainCamera->PSSetConstantBuffers(render->m_Context);
		mMainCamera->GSSetConstantBuffers(render->m_Context);

		m_DeferredRendering.G_Buffer_Rendering(render, [&](){
			mMainCamera->ScreenClear();
			PlayDrawList(DrawStage::Diffuse);
		});

		m_DeferredRendering.Light_Rendering(render, [&](){
			PlayDrawList(DrawStage::Light);
		});

		m_DeferredRendering.Deferred_Rendering(render, &mMainViewRenderTarget);
		
		m_DeferredRendering.Forward_Rendering(render, &mMainViewRenderTarget, [&](){
			PlayDrawList(DrawStage::Forward);
		});
		
		m_DeferredRendering.Particle_Rendering(render, &mMainViewRenderTarget, [&](){
			PlayDrawList(DrawStage::Particle);
		});

		m_DeferredRendering.HDR_Rendering(render);
#ifdef _ENGINE_MODE
	}
	else{

		m_DeferredRendering.Debug_G_Buffer_Rendering(render,
			[&](){
			mMainCamera->ScreenClear();
			PlayDrawList(DrawStage::Diffuse);
		});

		m_DeferredRendering.Debug_AlbedoOnly_Rendering(render,&mMainViewRenderTarget);
		
		
		m_DeferredRendering.Forward_Rendering(render, &mMainViewRenderTarget, [&](){
			PlayDrawList(DrawStage::Forward);
		});
		
		m_DeferredRendering.Particle_Rendering(render, &mMainViewRenderTarget, [&](){
			PlayDrawList(DrawStage::Particle);
		});
	}

#endif

	mPostEffectRendering.Rendering(render, [&](){
		PlayDrawList(DrawStage::PostEffect);
	});


#ifdef _ENGINE_MODE
	PlayDrawList(DrawStage::Engine);
#endif

	render->PopDS();

	{
		render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
		render->PushSet(BlendState::Preset::BS_Alpha, 0xFFFFFFFF);

		PlayDrawList(DrawStage::UI);

		render->PopBS();
		render->PopDS();
	}

	render->PopRS();

	ClearDrawList();

	SetMainCamera(NULL);

}


void Game::SaveScene(){
	m_Scene.SaveScene(mRootObject);
}

void Game::Update(){
	mDeltaTime.Tick();
	ActorMoveStage();
#ifdef _ENGINE_MODE
	mSelectActor.UpdateInspector();

	mProfileViewer.Update(1);
	if (mIsPlay){
		GamePlay();
	}
	else{
		GameStop();
	}
	mFPS.Update(1);
#else
	GamePlay();
#endif
}
#ifdef _ENGINE_MODE
void Game::GameStop(){
	mDeltaTime.SetTimeScale(1.0f);
	float deltaTime = mDeltaTime.GetDeltaTime();

	mCamera.Update(deltaTime);

	if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_Z)){
		mCommandManager.Undo();
	}

	if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_Y)){
		mCommandManager.Redo();
	}


	mSelectActor.Update(deltaTime);
	if (Input::Trigger(MouseCoord::Left)){

		if (!mSelectActor.ChackHitRay(mPhysX3Main, &mCamera)){

			int x, y;
			Input::MousePosition(&x, &y);
			XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
			XMVECTOR vect = mCamera.PointRayVector(point);
			XMVECTOR pos = mCamera.GetPosition();

			auto act = mPhysX3Main->Raycast(pos, vect,1000);
			if (act)
				mSelectActor.SetSelect(act);
		}
	}

	mRootObject->EngineUpdateComponent(deltaTime);

	Game::AddDrawList(DrawStage::Engine, [&](){
		mWorldGrid.Draw();
	});

	mPhysX3Main->EngineDisplay();
}
#endif
void Game::GamePlay(){
	float deltaTime = mDeltaTime.GetDeltaTime();

	mRootObject->UpdateComponent(deltaTime);
	mPhysX3Main->Display();
}

void Game::ClearDrawList(){
	for (auto &list : mDrawList){
		list.second.clear();
	}
}
void Game::PlayDrawList(DrawStage Stage){
	auto &list = mDrawList[Stage];
	for (auto &p : list){
		p();
	}
}

void Game::AllDestroyObject(){
	TransformComponent* t = (TransformComponent*)mRootObject->mTransform.Get();
	t->AllChildrenDestroy();
}


void Game::GetAllObject(const std::function<void(Actor*)>& collbak){

	for (auto& actpair : *gpList){
		collbak(actpair.second);
	}
}