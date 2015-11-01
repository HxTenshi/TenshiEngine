#include "Game.h"
#include "Component.h"

#include <stack>
static std::stack<int> gIntPtrStack;

static std::map<UINT,Actor*>* gpList;
static std::map<DrawStage, std::list<std::function<void()>>> *gDrawList;
static PhysX3Main* gpPhysX3Main;
static CommandManager* gCommandManager;
//
Actor* Game::mRootObject;

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

	FontManager::Init();

	gpList = &mList;
	gDrawList = &mDrawList;
	gCommandManager = &mCommandManager;

	mRootObject = new Actor();

	HRESULT hr = S_OK;
	hr = mCamera.Init();
	if (FAILED(hr))
		MessageBox(NULL, "Camera Create Error.", "Error", MB_OK);

	mPhysX3Main = new PhysX3Main();
	mPhysX3Main->InitializePhysX();
	gpPhysX3Main = mPhysX3Main;


	std::vector<std::string> fList;
	file_("./Scene/", fList);
	for (auto& f : fList){
		auto a = new Actor();
		a->ImportData("./Scene/" + f);
		AddObject(a);
	}

	//AddObject(new Text({ 0, 0 }, { 500, 500 }));

	//AddObject(new Player());
	//AddObject(new Tex("texture.png", { 1000, 0 }, { 1200, 200 }));
	//AddObject(new Tex("texture.bmp", { 1000, 200 }, { 1200, 400 }));
	//AddObject(new Tex(mCamera.GetDepthTexture(), { 0, 400 }, { 600, 800 }));
	//AddObject(new Particle());

	//AddObject(new Tex(Font::GetFontTexture(), { 600, 400 }, { 1200, 800 }));

	
	mSoundPlayer.Play();
	
	Window::SetWPFCollBack(MyWindowMessage::StackIntPtr, [&](void* p)
	{
		gIntPtrStack.push((int)p);
	});
	Window::SetWPFCollBack(MyWindowMessage::ReturnTreeViewIntPtr, [&](void* p)
	{
		int intptr = gIntPtrStack.top();
		((Actor*)intptr)->mTreeViewPtr = p;
		gIntPtrStack.pop();
		if (auto par = ((Actor*)intptr)->mTransform->GetParent())
			if (par->mTreeViewPtr)
				Window::SetParentTreeViewItem(par->mTreeViewPtr, ((Actor*)intptr)->mTreeViewPtr);
	});

	Window::SetWPFCollBack(MyWindowMessage::SelectActor, [&](void* p)
	{
		auto act = ((Actor*)p);
		mSelectActor.SetSelect(act);
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
			if (s == "Script")actor->AddComponent(shared_ptr<ScriptComponent>(new ScriptComponent()));
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

}

Game::~Game(){

	mSoundPlayer.Stop();
	for (auto& p : mList){
		p.second->ExportData("./Scene");
		//delete p;
	}

	delete mRootObject;

	mCamera.Release();

	delete mPhysX3Main;
	mPhysX3Main = NULL;
	gpPhysX3Main = NULL;

	FontManager::Release();


}
//static
void Game::AddObject(Actor* actor){
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
	Window::GetTreeViewWindow()->DeleteItem(actor);
	Window::GetInspectorWindow()->InsertConponentData(NULL);
	delete actor;
}

//static
PxRigidActor* Game::CreateRigitBody(){
	return gpPhysX3Main->createBox();
}
PxShape* Game::CreateShape(){
	return gpPhysX3Main->CreateShape();
}
PxShape* Game::CreateShapeSphere(){
	return gpPhysX3Main->CreateShapeSphere();
}
void Game::RemovePhysXActor(PxActor* act){
	return gpPhysX3Main->RemoveActor(act);
}

Actor* Game::FindUID(UINT uid){
	return (*gpList)[uid];
}

void Game::AddDrawList(DrawStage stage, std::function<void()> func){
	(*gDrawList)[stage].push_back(func);
}

void Game::SetUndo(ICommand* command){
	gCommandManager->SetUndo(command);
}