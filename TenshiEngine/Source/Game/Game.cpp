#include "Game/Game.h"
#include "Game/Component/ComponentFactory.h"

#include "Game/Component/TransformComponent.h"
#include "Game/Component/ScriptComponent.h"
#include "Game/Component/CameraComponent.h"

#include <stack>

static std::stack<int> gIntPtrStack;

static std::map<UINT,Actor*>* gpList;
static std::map<DrawStage, std::list<std::function<void()>>> *gDrawList;
static PhysX3Main* gpPhysX3Main;
static CommandManager* gCommandManager;
static CameraComponent** gMainCamera;
//
Actor* Game::mRootObject;
Actor* Game::mEngineRootObject;
Game* mGame = NULL;

static bool gIsPlay;

#include <vector>

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

#include "Engine/AssetLoader.h"

Game::Game(){

	mGame = this;
	mIsPlay = false;

	HRESULT hr = S_OK;

	ScriptManager::ReCompile();

	auto coms = ComponentFactory::GetComponents();
	for (auto& com : coms){
		auto name = com.first;
		auto str = name.substr(6);
		Window::CreateContextMenu_AddComponent(str);
	}

	hr = mMainViewRenderTarget.Create(WindowState::mWidth, WindowState::mHeight);
	if (FAILED(hr)){
		//MessageBox(NULL, "RenderTarget Create Error.", "Error", MB_OK);
	}

	m_DeferredRendering.Initialize();
	mPostEffectRendering.Initialize();

	gpList = &mList;
	gDrawList = &mDrawList;
	gCommandManager = &mCommandManager;
	gMainCamera = &mMainCamera;
	gIsPlay = mIsPlay;



	DWORD start = GetTickCount();

	for (int i = 0; i < 1000; i++){
		volatile auto act = new Actor();
		delete act;

	}

	DWORD end = GetTickCount();
	Window::AddLog("new:" + std::to_string(end - start));

	BYTE b[sizeof(Actor) * 1000];
	start = GetTickCount();

	for (int i = 0; i < 1000; i++){
#pragma push_macro("new")
#undef new
		volatile auto act = new(&b[sizeof(Actor)*i]) Actor();
		act->~Actor();

#pragma pop_macro("new")
	}

	end = GetTickCount();
	Window::AddLog("placement new:"+std::to_string(end - start));

	mRootObject = new Actor();
	mRootObject->mTransform = mRootObject->AddComponent<TransformComponent>();
	mRootObject->Initialize();

	mEngineRootObject = new Actor();
	mEngineRootObject->mTransform = mEngineRootObject->AddComponent<TransformComponent>();
	mEngineRootObject->Initialize();

	mCamera.Initialize();

	mPhysX3Main = new PhysX3Main();
	mPhysX3Main->InitializePhysX();
	gpPhysX3Main = mPhysX3Main;

	mSelectActor.Initialize();

	m_Scene.LoadScene("./Assets/Scene_.scene");
	
	mSoundPlayer.Play();
	
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
		mSelectActor.SetSelect(act);
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
		act->mTransform->SetParent(parent);
	});
	Window::SetWPFCollBack(MyWindowMessage::ActorDestroy, [&](void* p)
	{
		Game::DestroyObject((Actor*)p);
		mSelectActor.SetSelect(NULL);
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

	Window::SetWPFCollBack(MyWindowMessage::CreatePMXtoTEStaticMesh, [&](void* p)
	{
		std::string *s = (std::string*)p;
		AssetLoader loader;
		loader.LoadFile(*s);
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
			AddObject(a);
		}
		Window::Deleter(s);
	});

	Window::SetWPFCollBack(MyWindowMessage::CreateActorToPrefab, [&](void* p)
	{

		int intptr = gIntPtrStack.top();
		gIntPtrStack.pop();
		auto obj = ((Actor*)intptr);


		std::string *s = (std::string*)p;
		obj->ExportData("Assets", *s);

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
			mSelectActor.SetSelectAsset(data->GetFileData().GetActor(), s->c_str());
		}
		Window::Deleter(s);
	});

	Window::SetWPFCollBack(MyWindowMessage::OpenAsset, [&](void* p)
	{
		std::string *s = (std::string*)p;

		if ((*s).find(".scene\0") != (*s).npos){
			ChangePlayGame(false);
			TransformComponent* t = (TransformComponent*)mRootObject->mTransform.Get();
			t->AllChildrenDestroy();
			m_Scene.LoadScene(*s);
		}
		Window::Deleter(s);
	});

}

Game::~Game(){

	ChangePlayGame(false);
	mSoundPlayer.Stop();

	//for (auto& act : mList){
	//	DestroyObject(act.second);
	//}

	ActorMoveStage();
	TransformComponent* t = (TransformComponent*)mRootObject->mTransform.Get();
	t->AllChildrenDestroy();

	TransformComponent* t2 = (TransformComponent*)mEngineRootObject->mTransform.Get();
	t2->AllChildrenDestroy();

	ActorMoveStage();
	delete mRootObject;
	delete mEngineRootObject;

	mSelectActor.Finish();

	delete mPhysX3Main;
	mPhysX3Main = NULL;
	gpPhysX3Main = NULL;

	mMainViewRenderTarget.Release();

}
//static
void Game::AddObject(Actor* actor){
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
	
}

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
//static
void Game::DestroyObject(Actor* actor){
	if (!actor)return;
	auto desnum = gpList->erase(actor->GetUniqueID());
	if (!desnum)return;
	mGame->mActorMoveList.push(std::make_pair(ActorMove::Delete, actor));
}

//static
void Game::ActorMoveStage(){
	while (!mGame->mActorMoveList.empty()){
		auto& tar = mGame->mActorMoveList.front();
		mGame->mActorMoveList.pop();
		auto actor = tar.second;
		if (tar.first == ActorMove::Delete){
			auto ptr = mGame->mSelectActor.GetSelectOne();
			if (actor == ptr){
				mGame->mSelectActor.SetSelect(NULL);
			}
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
			TransformComponent* t = (TransformComponent*)actor->mTransform.Get();
			t->AllChildrenDestroy();
			actor->Finish();

			delete actor;
		}
		else{
			Window::AddTreeViewItem(actor->Name(), actor);

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
void Game::RemovePhysXActor(PxActor* act){
	return gpPhysX3Main->RemoveActor(act);
}
void Game::RemovePhysXActorEngine(PxActor* act){
	return gpPhysX3Main->RemoveActorEngine(act);
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

bool Game::IsGamePlay(){
	return gIsPlay;
}



void Game::ChangePlayGame(bool isPlay){
	if (mIsPlay == isPlay)return;

	mIsPlay = isPlay;
	gIsPlay = mIsPlay;
	if (isPlay){

		m_Scene.MemorySaveScene();

		//for (auto& act : *gpList){
		//	act.second->Start();
		//}
	}
	else{

		mSelectActor.SetSelect(NULL);

		AllDestroyObject();

		m_Scene.MemoryLoadScene();
	}

}

bool g_DebugRender = true;

void Game::Draw(){
	if (!mMainCamera){
		Device::mRenderTargetBack->ClearView();
		mMainViewRenderTarget.ClearView();
		ClearDrawList();
		return;
	}
	//ここでもアップデートしてる（仮処理）
	mMainCamera->Update();
	mMainCamera->VSSetConstantBuffers();
	mMainCamera->PSSetConstantBuffers();
	mMainCamera->GSSetConstantBuffers();

	//Device::mRenderTargetBack->ClearView();
	Device::mRenderTargetBack->ClearDepth();
	//mMainViewRenderTarget.ClearView();
	
	const RenderTarget* r[1] = { &mMainViewRenderTarget };
	RenderTarget::SetRendererTarget((UINT)1, r[0], Device::mRenderTargetBack);

	//mMainCamera->ScreenClear();



	D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDS.DepthFunc = D3D11_COMPARISON_LESS;
	ID3D11DepthStencilState* pDS = NULL;
	Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
	Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);


	//const RenderTarget* r[1] = { &mMainViewRenderTarget };
	//RenderTarget::SetRendererTarget((UINT)1, r[0], Device::mRenderTargetBack);

	//今は意味なし
	PlayDrawList(DrawStage::Depth);


	ID3D11ShaderResourceView *const pNULL[4] = { NULL, NULL, NULL, NULL };
	Device::mpImmediateContext->PSSetShaderResources(0, 4, pNULL);
	ID3D11SamplerState *const pSNULL[4] = { NULL, NULL, NULL, NULL };
	Device::mpImmediateContext->PSSetSamplers(0, 4, pSNULL);

	if( Input::Trigger(KeyCoord::Key_F1)){
		g_DebugRender = !g_DebugRender;
	}


	PlayDrawList(DrawStage::Init);
	if (!g_DebugRender){
		m_DeferredRendering.Start_G_Buffer_Rendering();
		mMainCamera->ScreenClear();
		PlayDrawList(DrawStage::Diffuse);

		for (int i = 0; i < 4; i++){
			m_DeferredRendering.Start_ShadowDepth_Buffer_Rendering(i);
			PlayDrawList(DrawStage::Diffuse);
		}
		m_DeferredRendering.End_ShadowDepth_Buffer_Rendering();
		
		m_DeferredRendering.Start_Light_Rendering();
		PlayDrawList(DrawStage::Light);
		m_DeferredRendering.End_Light_Rendering();
		m_DeferredRendering.Start_Deferred_Rendering(&mMainViewRenderTarget);

	}
	else{

		m_DeferredRendering.Debug_G_Buffer_Rendering();
		mMainCamera->ScreenClear();
		PlayDrawList(DrawStage::Diffuse);

		m_DeferredRendering.Debug_AlbedoOnly_Rendering(&mMainViewRenderTarget);
	}

	PlayDrawList(DrawStage::PostEffect);

	mPostEffectRendering.Rendering();

	PlayDrawList(DrawStage::Engine);
	PlayDrawList(DrawStage::UI);

	ClearDrawList();

	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
	if (pDS)pDS->Release();

	SetMainCamera(NULL);

}


void Game::SaveScene(){
	m_Scene.SaveScene(mRootObject);
}

float Game::GetDeltaTime(){
	float deltaTime;
	if (Device::mDriverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		deltaTime = (float)XM_PI * 0.0125f;
	}
	else
	{
		static float t = 0.0f;
		static float bt = 0.0f;
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		bt = t;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
		deltaTime = t - bt;
	}
	return deltaTime;
}

void Game::Update(){

	ActorMoveStage();

	if (mIsPlay){
		GamePlay();
	}
	else{
		GameStop();
	}
	mFPS.Update(1);
	mSelectActor.UpdateInspector();
}
void Game::GameStop(){
	float deltaTime = GetDeltaTime();

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

			auto act = mPhysX3Main->Raycast(pos, vect);
			mSelectActor.SetSelect(act);
		}
	}

	mRootObject->EngineUpdateComponent(deltaTime);

	Game::AddDrawList(DrawStage::Engine, [&](){
		mWorldGrid.Draw();
	});

	mPhysX3Main->EngineDisplay();
}
void Game::GamePlay(){
	float deltaTime = GetDeltaTime();

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