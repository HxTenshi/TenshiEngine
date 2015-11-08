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
}

Game::Game()
	:mWorldGrid(1.0f){

	mIsPlay = false;

	HRESULT hr = S_OK;

	FontManager::Init();


	hr = mMeinViewRenderTarget.Create(WindowState::mWidth, WindowState::mHeight);
	if (FAILED(hr))
		MessageBox(NULL, "RenderTarget Create Error.", "Error", MB_OK);

	gpList = &mList;
	gDrawList = &mDrawList;
	gCommandManager = &mCommandManager;
	gMainCamera = &mMainCamera;
	gIsPlay = mIsPlay;

	mRootObject = new Actor();
	mRootObject->mTransform = shared_ptr<TransformComponent>(new TransformComponent());
	mRootObject->AddComponent<TransformComponent>(mRootObject->mTransform);
	//hr = mCamera.Init();
	//if (FAILED(hr))
	//	MessageBox(NULL, "Camera Create Error.", "Error", MB_OK);

	mPhysX3Main = new PhysX3Main();
	mPhysX3Main->InitializePhysX();
	gpPhysX3Main = mPhysX3Main;


	//std::vector<std::string> fList;
	//file_("./Scene/", fList);
	//for (auto& f : fList){
	//	auto a = new Actor();
	//	a->ImportData("./Scene/" + f);
	//	AddObject(a);
	//}

	File scenefile("./Assets/Scene.scene");
	if (scenefile){
		UINT id;
		while (scenefile){
			if (!scenefile.In(&id))break;
			auto a = new Actor();
			a->ImportData("./Scene/Object_" + std::to_string(id) + ".txt");
			AddObject(a);
		}
	}


	
	mSoundPlayer.Play();
	
	Window::SetWPFCollBack(MyWindowMessage::StackIntPtr, [&](void* p)
	{
		gIntPtrStack.push((int)p);
	});
	Window::SetWPFCollBack(MyWindowMessage::ReturnTreeViewIntPtr, [&](void* p)
	{
		int intptr = gIntPtrStack.top();
		gIntPtrStack.pop();
		((Actor*)intptr)->mTreeViewPtr = p;
		if (auto par = ((Actor*)intptr)->mTransform->GetParent())
			if (par->mTreeViewPtr)
				Window::SetParentTreeViewItem(par->mTreeViewPtr, ((Actor*)intptr)->mTreeViewPtr);
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
		std::string s((const char *)p);
		if (auto actor = mSelectActor.GetSelect()){
			if (s == "Script"){
				auto c = shared_ptr<ScriptComponent>(new ScriptComponent());
				actor->AddComponent(c);
				c->Initialize();
			}
			if (s == "PhysX")actor->AddComponent(shared_ptr<PhysXComponent>(new PhysXComponent()));
			if (s == "Collider")actor->AddComponent(shared_ptr<PhysXColliderComponent>(new PhysXColliderComponent()));
			Window::ClearInspector();
			actor->CreateInspector();
		}
	});
	Window::SetWPFCollBack(MyWindowMessage::RemoveComponent, [&](void* p)
	{
		std::string s((const char *)p);
		if (auto actor = mSelectActor.GetSelect()){
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
		a->ImportDataAndNewID(*s);
		AddObject(a);
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
}

Game::~Game(){

	ChangePlayGame(false);
	mSoundPlayer.Stop();

	delete mRootObject;

	delete mPhysX3Main;
	mPhysX3Main = NULL;
	gpPhysX3Main = NULL;

	mMeinViewRenderTarget.Release();
	FontManager::Release();


}
//static
void Game::AddObject(Actor* actor){
	if (!actor->mTransform){
		delete actor;
	}

	gpList->insert(std::pair<UINT, Actor*>(actor->GetUniqueID(), actor));
	actor->Initialize();
	Window::GetTreeViewWindow()->AddItem(actor);
	if (!actor->mTransform->GetParent()){
		actor->mTransform->SetParent(mRootObject);
	}
}
//static
void Game::DestroyObject(Actor* actor){
	if (!actor)return;
	gpList->erase(actor->GetUniqueID());
	Window::GetInspectorWindow()->InsertConponentData(NULL);
	if (!gIsPlay)
		delete actor;
	else{
		actor->mTransform->SetParent(NULL);
	}
}

//static
PxRigidActor* Game::CreateRigitBody(){
	return gpPhysX3Main->createBox();
}
PhysX3Main* Game::GetPhysX(){
	return gpPhysX3Main;
}
void Game::RemovePhysXActor(PxActor* act){
	return gpPhysX3Main->RemoveActor(act);
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


void Game::ChangePlayGame(bool isPlay){

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
	}
	else{
		for (auto& act : mListBack){
			auto postactor = mList[act.first];

			postactor->CopyData(postactor, act.second);
			//親子関係を解除しないとデストラクタで子が消される
			act.second->GetComponent<TransformComponent>()->mParent = NULL;

			//親子関係が構築されていないので手動で構築
			postactor->GetComponent<TransformComponent>()->SetParent(postactor->GetComponent<TransformComponent>()->GetParent());

			if (mGamePlayList.find(act.first) == mGamePlayList.end())
				Window::GetTreeViewWindow()->AddItem(postactor);

			//ツリービュー再構築
			if (auto par = postactor->GetComponent<TransformComponent>()->GetParent())
				if (par->mTreeViewPtr)
					Window::SetParentTreeViewItem(par->mTreeViewPtr, postactor->mTreeViewPtr);
		}
		//親子を解除してから削除
		for (auto& act : mListBack){
			delete act.second;
		}
		mListBack.clear();

		mGamePlayList.clear();
		gpList = &mList;
	}
}


void Game::SaveScene(){
	//for (auto& p : mList){
	//	
	//	p.second->ExportData("./Scene");
	//	//delete p;
	//}
	File scenefile;
	if (scenefile.Open("./Assets/Scene.scene")){
		scenefile.FileCreate();
	}
	scenefile.Clear();

	mRootObject->ExportSceneDataStart("./Scene", scenefile);
}