#include "Game.h"
#include "Component.h"

#include <stack>

static std::stack<int> gIntPtrStack;

static std::map<UINT,Actor*>* gpList;
static std::map<DrawStage, std::list<std::function<void()>>> *gDrawList;
static PhysX3Main* gpPhysX3Main;
static CommandManager* gCommandManager;
static CameraComponent** gMainCamera;
//
Actor* Game::mRootObject;
Game* mGame = NULL;

static bool gIsPlay;

#include <algorithm>
#include <filesystem>
#include <vector>
void file_(const std::string& pass, std::vector<std::string>& fileList) {
	namespace sys = std::tr2::sys;
	sys::path p = pass;
	std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
		//  再帰的に走査するならコチラ↓
		//std::for_each(sys::recursive_directory_iterator(p), sys::recursive_directory_iterator(),
		[&](const sys::path& p) {
		if (sys::is_regular_file(p)) { // ファイルなら...
			fileList.push_back(p.filename());
		}
		else if (sys::is_directory(p)) { // ディレクトリなら...
		}
	});
}

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
void SelectActor::SelectActorDraw(){
	Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
		auto mModel = mSelect->GetComponent<ModelComponent>();
		if (!mModel)return;
		Model& model = *mModel->mModel;

		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		//descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		ID3D11DepthStencilState* pDS = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
		Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);

		D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		descRS.CullMode = D3D11_CULL_BACK;
		descRS.FillMode = D3D11_FILL_WIREFRAME;

		ID3D11RasterizerState* pRS = NULL;
		Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

		Device::mpImmediateContext->RSSetState(pRS);


		model.Draw(mSelectWireMaterial);


		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	}));
	//Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
	//	auto mModel = mVectorBox[0].GetComponent<ModelComponent>();
	//	if (!mModel)return;
	//	Model& model1 = *mModel->mModel;
	//	mModel = mVectorBox[1].GetComponent<ModelComponent>();
	//	if (!mModel)return;
	//	Model& model2 = *mModel->mModel;
	//	mModel = mVectorBox[2].GetComponent<ModelComponent>();
	//	if (!mModel)return;
	//	Model& model3 = *mModel->mModel;
	//
	//	ID3D11DepthStencilState* pBackDS;
	//	UINT ref;
	//	Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);
	//
	//	D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	//	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//	//descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//	descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
	//	ID3D11DepthStencilState* pDS = NULL;
	//	Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
	//	Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);
	//
	//	D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	//	descRS.CullMode = D3D11_CULL_BACK;
	//	descRS.FillMode = D3D11_FILL_SOLID;
	//
	//	ID3D11RasterizerState* pRS = NULL;
	//	Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);
	//
	//	Device::mpImmediateContext->RSSetState(pRS);
	//
	//
	//	model1.Draw(mVectorBox[0].GetComponent<MaterialComponent>());
	//	model2.Draw(mVectorBox[1].GetComponent<MaterialComponent>());
	//	model3.Draw(mVectorBox[2].GetComponent<MaterialComponent>());
	//
	//
	//	Device::mpImmediateContext->RSSetState(NULL);
	//	if (pRS)pRS->Release();
	//
	//	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
	//	pDS->Release();
	//
	//	Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
	//	if (pBackDS)pBackDS->Release();
	//}));
}

void SelectActor::SetSelect(Actor* select){
	mDragBox = -1;

	if (mGame->FindActor(select)){
		mSelect = select;
	}
	else{
		mSelect = NULL;
	}
	Window::ClearInspector();
	if (mSelect)mSelect->CreateInspector();
}

Game::Game()
	:mWorldGrid(1.0f){

	mGame = this;
	mIsPlay = false;

	HRESULT hr = S_OK;

	ComponentFactory::Initialize();
	auto coms = ComponentFactory::GetComponents();
	for (auto& com : coms){
		auto name = com.first;
		auto str = name.substr(6);
		Window::CreateContextMenu_AddComponent(str);
	}

	hr = mMainViewRenderTarget.Create(WindowState::mWidth, WindowState::mHeight);
	if (FAILED(hr))
		MessageBox(NULL, "RenderTarget Create Error.", "Error", MB_OK);

	m_DeferredRendering.Initialize();
	mPostEffectRendering.Initialize();

	gpList = &mList;
	gDrawList = &mDrawList;
	gCommandManager = &mCommandManager;
	gMainCamera = &mMainCamera;
	gIsPlay = mIsPlay;

	mRootObject = new Actor();
	mRootObject->mTransform = mRootObject->AddComponent<TransformComponent>();
	mRootObject->Initialize();

	mCamera.Initialize();
	//hr = mCamera.Init();
	//if (FAILED(hr))
	//	MessageBox(NULL, "Camera Create Error.", "Error", MB_OK);

	mPhysX3Main = new PhysX3Main();
	mPhysX3Main->InitializePhysX();
	gpPhysX3Main = mPhysX3Main;

	mSelectActor.Initialize();

	//std::vector<std::string> fList;
	//file_("./Scene/", fList);
	//for (auto& f : fList){
	//	auto a = new Actor();
	//	a->ImportData("./Scene/" + f);
	//	AddObject(a);
	//}

	m_Scene.LoadScene("./Assets/Scene_.scene");

	//File scenefile("./Assets/Scene.scene");
	//if (scenefile){
	//	UINT id;
	//	while (scenefile){
	//		if (!scenefile.In(&id))break;
	//		auto a = new Actor();
	//		a->ImportData("./Scene/Object_" + std::to_string(id) + ".txt");
	//		AddObject(a);
	//	}
	//}


	
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
		if (auto actor = mSelectActor.GetSelect()){
			auto temp = ComponentFactory::Create(*s);
			if (temp){
				actor->AddComponent(temp);
			}
			//if (s == "PostEffect")actor->AddComponent(shared_ptr<PostEffectComponent>(new PostEffectComponent()));
			//if (s == "Script")actor->AddComponent(shared_ptr<ScriptComponent>(new ScriptComponent()));
			//if (s == "PhysX")actor->AddComponent(shared_ptr<PhysXComponent>(new PhysXComponent()));
			//if (s == "Collider")actor->AddComponent(shared_ptr<PhysXColliderComponent>(new PhysXColliderComponent()));
			Window::ClearInspector();
			actor->CreateInspector();

		}
		Window::Deleter(s);
	});
	Window::SetWPFCollBack(MyWindowMessage::RemoveComponent, [&](void* p)
	{
		if (auto actor = mSelectActor.GetSelect()){
			actor->RemoveComponent((Component*)p);
			Window::ClearInspector();
			actor->CreateInspector();
		}
		return;

		std::string s((const char *)p);
		if (auto actor = mSelectActor.GetSelect()){
			if (s == "MeshDraw")actor->RemoveComponent<MeshDrawComponent>();
			if (s == "Script")actor->RemoveComponent<ScriptComponent>();
			if (s == "PhysX")actor->RemoveComponent<PhysXComponent>();
			if (s == "Collider")actor->RemoveComponent<PhysXColliderComponent>();
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

	Window::SetWPFCollBack(MyWindowMessage::PlayGame, [&](void* p)
	{
		ChangePlayGame(true);
	});
	Window::SetWPFCollBack(MyWindowMessage::StopGame, [&](void* p)
	{
		ChangePlayGame(false);
	});
	Window::SetWPFCollBack(MyWindowMessage::ScriptCompile, [&](void* p)
	{
		ScriptManager::ReCompile();
	});
	Window::SetWPFCollBack(MyWindowMessage::SaveScene, [&](void* p)
	{
		SaveScene();
	});

	Window::SetWPFCollBack(MyWindowMessage::OpenAsset, [&](void* p)
	{
		std::string *s = (std::string*)p;
		if ((*s).find(".scene\0") != (*s).npos){
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
	TransformComponent* t = (TransformComponent*)mRootObject->mTransform.Get();
	t->AllChildrenDestroy();
	delete mRootObject;

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

	gpList->insert(std::pair<UINT, Actor*>(actor->GetUniqueID(), actor));
	actor->Initialize();
	actor->Start();
	if (!gIsPlay)
	{
		Window::AddTreeViewItem(actor->Name(), actor);
		if (!actor->mTransform->GetParent()){
			actor->mTransform->SetParent(mRootObject);
		}
	}
	else{
		mGame->mActorMoveList.push(std::make_pair(ActorMove::Create, actor));
	}
}
//static
void Game::DestroyObject(Actor* actor){
	if (!actor)return;
	auto desnum = gpList->erase(actor->GetUniqueID());
	if (!desnum)return;
	if (!gIsPlay){
		//ツリービューが作成されていれば
		if (actor->mTreeViewPtr){
			Window::ClearTreeViewItem(actor->mTreeViewPtr);
		}
		//ツリービューが作成される前なら
		else{
			//削除リストに追加
			mGame->mTreeViewItem_ErrerClearList.push_back(actor);
		}
		TransformComponent* t = (TransformComponent*)actor->mTransform.Get();
		t->AllChildrenDestroy();
		actor->Finish();

		auto ptr = mGame->mSelectActor.GetSelect();
		if (actor == ptr){
			mGame->mSelectActor.SetSelect(NULL);
		}

		delete actor;
	}
	else{
		mGame->mActorMoveList.push(std::make_pair(ActorMove::Delete, actor));
		//Window::ClearTreeViewItem(actor->mTreeViewPtr);
		//actor->mTreeViewPtr = NULL;
		//TransformComponent* t = (TransformComponent*)actor->mTransform.Get();
		//t->AllChildrenDestroy();
		//actor->Finish();
	}
}

//static
void Game::ActorMoveStage(){
	while (!mGame->mActorMoveList.empty()){
		auto& tar = mGame->mActorMoveList.front();
		mGame->mActorMoveList.pop();
		auto actor = tar.second;
		if (tar.first == ActorMove::Delete){
			auto ptr = mGame->mSelectActor.GetSelect();
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

			//ゲームプレイ中に追加されたオブジェクトならデリート
			if (mGame->mList.find(actor->GetUniqueID()) == mGame->mList.end()){
				delete actor;
			}
		}
		else{
			Window::AddTreeViewItem(actor->Name(), actor);
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

void Game::ChangePlayGame(bool isPlay){
	if (mIsPlay == isPlay)return;
	mIsPlay = isPlay;
	gIsPlay = mIsPlay;
	if (isPlay){
		for (auto& act : mList){
			auto postactor = new Actor();
			mListBack[act.first] = postactor;

			postactor->CopyData(postactor, act.second);
		}
		mGamePlayList = mList;
		gpList = &mGamePlayList;
		for (auto& act : mGamePlayList){
			act.second->Finish();
		}
	}
	else{

		for (auto& act : *gpList){
			act.second->Finish();
		}
		//戻すデータ一覧
		for (auto& act : mListBack){
			//戻す対象
			auto postactor = mList[act.first];

			postactor->CopyData(postactor, act.second);
			//親子関係を解除しないとデストラクタで子が消される
			//act.second->GetComponent<TransformComponent>()->mParent = NULL;

			//親子関係が構築されていないので手動で構築
			//if (!postactor->mTransform->GetParent()){
			//	postactor->mTransform->SetParent(mRootObject);
			//}
			//else{
			//postactor->GetComponent<TransformComponent>()->SetParent(postactor->GetComponent<TransformComponent>()->GetParent());
			//}

			//ゲームシーンで削除されていれば
			if (mGamePlayList.find(act.first) == mGamePlayList.end()){
				Window::AddTreeViewItem(postactor->Name(), postactor);	
			}
			//終わったアクターを削除
			mGamePlayList.erase(act.first);

			//ツリービュー再構築
			if (auto par = postactor->GetComponent<TransformComponent>()->GetParent())
				if (par->mTreeViewPtr&&postactor->mTreeViewPtr)
					Window::SetParentTreeViewItem(par->mTreeViewPtr, postactor->mTreeViewPtr);
		}

		//残ったアクターは新しく追加されたアクター
		while (!mGamePlayList.empty()){
			auto ite = mGamePlayList.begin();
			auto& addact = *ite;
			DestroyObject(addact.second);
		}

		//親子を解除してから削除
		for (auto& act : mListBack){
			delete act.second;
		}
		mListBack.clear();

		mGamePlayList.clear();
		gpList = &mList;
	}

	for (auto& act : *gpList){
		act.second->Initialize();
	}
	for (auto& act : *gpList){
		act.second->Start();
	}
}


void Game::SaveScene(){
	//File scenefile;
	//if (scenefile.Open("./Assets/Scene.scene")){
	//	scenefile.FileCreate();
	//}
	//scenefile.Clear();

	//mRootObject->ExportSceneDataStart("./Scene", scenefile);

	m_Scene.SaveScene(mRootObject);
}

PostEffectRendering::~PostEffectRendering(){
	mModelTexture.Release();
}
void PostEffectRendering::Initialize(){
	mModelTexture.Create("");
	mMaterial.Create("EngineResource/PostEffectRendering.fx");
	mMaterial.SetTexture(Game::GetMainViewRenderTarget().GetTexture(), 0);
}