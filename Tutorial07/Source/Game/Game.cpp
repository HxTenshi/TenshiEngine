#include "Game.h"
#include "Component.h"


static std::list<Actor*>* gpList;
static PhysX3Main* gpPhysX3Main;

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

Game::Game()
	:mWorldGrid(1.0f){

	gpList = &mList;

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


	//AddObject(new Player());
	//AddObject(new Tex("texture.png", { 1000, 0 }, { 1200, 200 }));
	//AddObject(new Tex("texture.bmp", { 1000, 200 }, { 1200, 400 }));
	//AddObject(new Tex(mCamera.GetDepthTexture(), { 0, 400 }, { 600, 800 }));
	//AddObject(new Particle());

	Font::Init();
	//AddObject(new Tex(Font::GetFontTexture(), { 600, 400 }, { 1200, 800 }));

	
	mSoundPlayer.Play();

	Window::SetWPFCollBack(MyWindowMessage::SelectActor, [&](void* p)
	{
		auto act = ((Actor*)p);
		mSelectActor.SetSelect(act);
	});
	Window::SetWPFCollBack(MyWindowMessage::ActorDestroy, [&](void* p)
	{
		DeleteObject((Actor*)p);
		mSelectActor.SetSelect(NULL);
	});

	Window::SetWPFCollBack(MyWindowMessage::AddComponent, [&](void* p)
	{
		std::string s((const char *)p);
		if (auto actor = mSelectActor.GetSelect()){
			if (s == "Script")actor->AddComponent(shared_ptr<ScriptComponent>(new ScriptComponent()));
			if (s == "PhysX")actor->AddComponent(shared_ptr<PhysXComponent>(new PhysXComponent()));
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
	for (Actor* p : mList){
		p->ExportData("./Scene");
		delete p;
	}

	mCamera.Release();

	delete mPhysX3Main;
	mPhysX3Main = NULL;
	gpPhysX3Main = NULL;

}
//static
void Game::AddObject(Actor* actor){
	gpList->push_back(actor);
	Window::GetTreeViewWindow()->AddItem(actor);
}
//static
void Game::DeleteObject(Actor* actor){
	if (!actor)return;
	gpList->remove(actor);
	Window::GetTreeViewWindow()->DeleteItem(actor);
	Window::GetInspectorWindow()->InsertConponentData(NULL);
	delete actor;
}

//static
PxRigidActor* Game::CreateRigitBody(){
	return gpPhysX3Main->createBox();
}
void Game::RemovePhysXActor(PxActor* act){
	return gpPhysX3Main->RemoveActor(act);
}