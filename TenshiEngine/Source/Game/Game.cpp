#include "Game/Game.h"

#include "PhysX/PhysX3.h"
#include "Game/Component/ComponentFactory.h"

#include "Game/Component/TransformComponent.h"
#include "Game/Component/CameraComponent.h"
#include "Engine/ScriptingSystem/ScriptManager.h"

#include <stack>

#include "Engine/AssetFile/Prefab/PrefabFileData.h"

#include "Game/RenderingSystem.h"

#include "Engine/ModelConverter.h"
#include "SettingObject/PhysxLayer.h"
#include "SettingObject/Canvas.h"
#include "Engine/Asset/GenerateMetaFile.h"

#include <algorithm>
#include <filesystem>


void _LoadMetaFiles(const char* path){
	namespace sys = std::tr2::sys;
	sys::path p(path); // 列挙の起点
	std::for_each(sys::recursive_directory_iterator(p), sys::recursive_directory_iterator(),
		[](const sys::path& p) {
		if (sys::is_regular_file(p)) { // ファイルなら...
			if (p.extension() == ".meta")return;
			//メタファイルが存在していなければ
			if (GetFileAttributes((p.generic_string() + ".meta").c_str()) == -1){
				MakeMetaFile(p.generic_string());
			}
			//メタファイル読み込み
			AssetDataBase::InitializeMetaData(p.generic_string().c_str());
		}
		else if (sys::is_directory(p)) { // ディレクトリなら...

		}
	});
}
void LoadMetaFiles() {
	_LoadMetaFiles("EngineResource/");
	_LoadMetaFiles("Assets/");
}


static std::stack<int> gIntPtrStack;

static Game::ListMapType* gpList;
static Game::DrawListMapType *gDrawList;
static PhysX3Main* gpPhysX3Main;
static CommandManager* gCommandManager;
static CameraComponent** gMainCamera;
//
Game::GameObjectPtr Game::mRootObject;
#ifdef _ENGINE_MODE
static CameraComponent** gMainCameraEngineUpdate;
static SelectActor* gSelectActor;
Game::GameObjectPtr Game::mEngineRootObject;
std::list<Game::GameObjectPtr> Game::mEngineObjects;
#endif
Game* mGame = NULL;
Scene Game::m_Scene;
DeltaTime* gpDeltaTime;
SystemHelper Game::mSystemHelper;

template <class T>
struct SettingObjectData {
	shared_ptr<T> object = NULL;
	const std::string name;
	const std::string folder;
	SettingObjectData(const char* name, const char* folder)
		:name(name),folder(folder)
	{}

	void Create() {
		auto o = make_shared<T>();
		object = o;
		o->mTransform = o->AddComponent<TransformComponent>();
		o->ImportData(folder+"/"+name+".prefab");

#ifdef _ENGINE_MODE
		Game::AddEngineObject(o);
		Window::AddEngineTreeViewItem(name, (void*)o.Get());
#endif
	}
	void Save() {
		object->ExportData(folder+"/"+name + ".prefab");
	}
};
struct SettingObject {
	SettingObjectData<PhysxLayer> PhysxLayer = {"PhysxLayer","Settings" };
	SettingObjectData<Canvas> Canvas = { "Canvas","Settings" };

	void Create() {
		PhysxLayer.Create();
		Canvas.Create();
	}
	void Save() {
		PhysxLayer.Save();
		Canvas.Save();
	}
}g_SettingObject;


#ifdef _ENGINE_MODE
static bool gIsPlay;
#endif

#include <vector>

#ifdef _ENGINE_MODE
//ツリービューが完成するまで繰り返す関数
std::function<void()> CreateSetParentTreeViewItemColl(Actor* par, Actor* chil){
	return [=](){
		if (par->mTreeViewPtr && chil->mTreeViewPtr){
			Window::SetParentTreeViewItem(par->mTreeViewPtr, chil->mTreeViewPtr);
		}
		else{
			chil->SetUpdateStageCollQueue(CreateSetParentTreeViewItemColl(par, chil));
		}
	};
}
//ツリービューが完成するまで繰り返す関数
std::function<void()> CreateSetParentTreeViewItemCollRoot(Actor* chil) {
	return [=]() {
		if (chil->mTreeViewPtr) {
			Window::SetParentTreeViewItem(NULL, chil->mTreeViewPtr);
		}
		else {
			chil->SetUpdateStageCollQueue(CreateSetParentTreeViewItemCollRoot(chil));
		}
	};
}
#endif

#include "Engine/AssetLoader.h"

#include "Engine/AssetFile/MetaFileData.h"
#include "Engine/AssetFile/Material/TextureFileData.h"


void InitContextMenu(){

#ifdef _ENGINE_MODE
	_SYSTEM_LOG_H("コンテキストメニューの初期化");
	auto coms = ComponentFactory::GetComponents();
	for (auto& com : coms){
		auto name = com.first;
		auto str = name.substr(6);
		Window::CreateContextMenu_AddComponent(str);
	}

	namespace sys = std::tr2::sys;
	sys::path p("EngineResource/prefab/"); // 列挙の起点
	//std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
	//  再帰的に走査するならコチラ↓
	std::for_each(sys::recursive_directory_iterator(p), sys::recursive_directory_iterator(),
		[&](const sys::path& p) {
		if (sys::is_regular_file(p)) { // ファイルなら...
			if (p.extension() == ".prefab"){
				Window::CreateContextMenu_CreateObject(p.stem().string() , p.string());
			}
		}
		else if (sys::is_directory(p)) { // ディレクトリなら...
			//std::cout << "dir.: " << p.string() << std::endl;
		}
	});
#endif
}

Game::Game() {

	LoadMetaFiles();


	_SYSTEM_LOG_H("ゲームシーンの初期化");
	mGame = this;
	mMainCamera = NULL;
	mMainCameraEngineUpdate = NULL;


	gpDeltaTime = &mDeltaTime;
#ifdef _ENGINE_MODE
	mWorldGrid.Initialize();
	gSelectActor = &mSelectActor;

	mIsPlay = false;
#endif

	HRESULT hr = S_OK;

#ifdef _ENGINE_MODE
	ScriptManager::ReCompile();

	InitContextMenu();
#endif

	hr = mMainViewRenderTarget.CreateRTandDepth(WindowState::mWidth, WindowState::mHeight, DXGI_FORMAT_R11G11B10_FLOAT);
	if (FAILED(hr)) {
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
	gMainCameraEngineUpdate = &mMainCameraEngineUpdate;
	gCommandManager = &mCommandManager;
	gIsPlay = mIsPlay;
#endif

#ifdef _ENGINE_MODE
	mEngineRootObject = make_shared<Actor>();
	auto t = mEngineRootObject->AddComponent<TransformComponent>();
	mEngineRootObject->mTransform = t;
	t->m_EngineObject = true;
	mEngineRootObject->Initialize();
	mEngineRootObject->Start();
#endif

	mRootObject = make_shared<Actor>();
	mRootObject->mTransform = mRootObject->AddComponent<TransformComponent>();
	mRootObject->Initialize();
	mRootObject->Start();

#ifdef _ENGINE_MODE

	g_SettingObject.Create();


	mCamera.Initialize();
	mSelectActor.Initialize();
#endif

#ifdef _ENGINE_MODE
	//LoadScene("./Assets/Scene_.scene");
#else
	LoadScene("./Assets/Title.scene");
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
		WindowParentSet(act->shared_from_this());
			
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
			parent = mRootObject.Get();
		}
		auto act = ((Actor*)p);

		act->mTransform->SetParentWorld(parent->shared_from_this());

		SetUndo(act);

	});
	Window::SetWPFCollBack(MyWindowMessage::ActorDestroy, [&](void* p)
	{
		Game::DestroyObject(((Actor*)p)->shared_from_this(), true);
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
		bool undo = !IsGamePlay();
		Instance(*s, undo);
		Window::Deleter(s);
	});

	Window::SetWPFCollBack(MyWindowMessage::CreateActorToPrefab, [&](void* p)
	{

		int intptr = gIntPtrStack.top();
		gIntPtrStack.pop();
		auto obj = ((Actor*)intptr);

		std::string *s = (std::string*)p;
		obj->ExportData(*s,true,true);

		AssetDataBase::FileUpdate((*s).c_str());

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
		if (IsGamePlay())return;
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
		if (IsGamePlay())return;
		SaveScene();
	});
	Window::SetWPFCollBack(MyWindowMessage::SelectAsset, [&](void* p)
	{
		std::string *s = (std::string*)p;
		mSelectActor.SetSelectAsset(NULL, "");

		PrefabAssetDataPtr data;
		AssetDataBase::Instance(s->c_str(), data);
		if (data){
			mSelectActor.SetSelectAsset(data->GetFileData()->GetActor().Get(), s->c_str());
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
		}else if((*s).find(".cpp\0") != (*s).npos || (*s).find(".h\0") != (*s).npos) {
			int line = 0;
			if (gIntPtrStack.size() != 0) {
				line = gIntPtrStack.top();
				gIntPtrStack.pop();
			}

			ScriptManager::OpenScriptFile(*s, line);
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
	//delete mRootObject;
#ifdef _ENGINE_MODE
	////delete mEngineRootObject;
	mSelectActor.Finish();
#endif


	delete mPhysX3Main;
	mPhysX3Main = NULL;
	gpPhysX3Main = NULL;

	mMainViewRenderTarget.Release();

}


Game* Game::Get(){
	return mGame;
}

GameObject Game::Instance(const std::string& base,bool undo) {
	PrefabAssetDataPtr prefub;
	AssetDataBase::Instance(base.c_str(), prefub);
	return Game::Instance(prefub, undo);
}
GameObject Game::Instance(bool undo) {
	auto a = make_shared<Actor>();
	a->AddComponent<TransformComponent>();
	a->mTransform = a->GetComponent<TransformComponent>();
	a->CreateNewID();
	a->mTransform->SetParentWorld(NULL);
	Game::AddObject(a, undo);
	return a;
}
GameObject Game::Instance(GameObject base, bool undo) {
	if (base) {
		picojson::value val;
		base->ExportData(val, true);

		return Instance(val,undo);
	}

	return NULL;
}
GameObject Game::Instance(picojson::value base, bool undo, bool parentTop) {

	auto a = make_shared<Actor>();
	auto This = a;
	a->SetInspectorFindGameObjectFunc([This](auto id)->GameObject {
		auto target = GameObjectFindHelper::ChildrenFind(This, id);
		if (!target) {
			target = Game::FindUID(id);
		}
		return target;
	});

	a->ImportDataAndNewID(base, [](auto o) { Game::AddObject(o, false, true); });
	if (parentTop) {
		a->mTransform->SetParentWorld(NULL);
	}

	Game::AddObject(a, undo);
	//addchild(a);
	return a;
}
GameObject Game::Instance(PrefabAssetDataPtr base, bool undo) {
	if (base) {
		if (base->GetFileData()) {
			auto val = base->GetFileData()->GetParam();
			auto a = make_shared<Actor>();
			auto This = a;
			a->SetInspectorFindGameObjectFunc([This](auto id)->GameObject {
				return GameObjectFindHelper::ChildrenFind(This, id);
			});

			a->ImportDataAndNewID(*val, [](auto o) { Game::AddObject(o,false,true); });
			a->mTransform->SetParentWorld(NULL);
			Game::AddObject(a, undo);
			//addchild(a);
			return a;
		}
	}

	return NULL;
}
void Game::AddObject(GameObjectPtr actor, bool undoFlag, bool DelayInitialize){
	if (!actor->mTransform){
		//delete actor;
		return;
	}
	if (actor->GetUniqueID().IsNull()){
		actor->CreateNewID();
	}

	gpList->insert(std::make_pair(actor->GetUniqueID(), actor));
	if (DelayInitialize) {
		mGame->mActorMoveList.push(std::make_pair(ActorMove::Create_DelayInitialize, actor));
	}
	else {
		actor->Initialize();
		mGame->mActorMoveList.push(std::make_pair(ActorMove::Create, actor));
	}

	//for (auto child : actor->mTransform->Children()){
	//	AddObject(child);
	//}
#ifdef _ENGINE_MODE
	if (undoFlag){
		gCommandManager->SetUndo(new ActorDestroyUndoCommand(actor.Get()));
		gCommandManager->SetUndo(new ActorUndoCommand(actor.Get()));
	}
#endif
}
#ifdef _ENGINE_MODE
//static
void Game::AddEngineObject(GameObjectPtr actor){
	if (!actor->mTransform){
		//delete actor;
		return;
	}
	auto t = (TransformComponent*)actor->mTransform.Get();
	t->m_EngineObject = true;
	actor->Initialize();
	actor->Start();
	actor->mTransform->SetParent(mEngineRootObject);

	mEngineObjects.push_back(actor);
}
#endif
//static
void Game::DestroyObject(GameObjectPtr actor, bool undoFlag){
	if (!actor)return;
	auto destroy = gpList->find(actor->GetUniqueID());
	if (gpList->end() == destroy)return;
	if (destroy->second != actor)return;
	gpList->erase(destroy);
	// if (!desnum)return;
	mGame->mActorMoveList.push(std::make_pair(ActorMove::Delete, actor));
#ifdef _ENGINE_MODE
	if (undoFlag){
		gCommandManager->SetUndo(new ActorUndoCommand(actor.Get()));
		gCommandManager->SetUndo(new ActorDestroyUndoCommand(actor.Get()));
	}
#endif
}

//static
void Game::ActorMoveStage(){
	for (int i = 0; i < mGame->mActorMoveList.size();i++){
		auto& tar = mGame->mActorMoveList._Get_container()[i];
		if (tar.first == ActorMove::Create_DelayInitialize) {
			tar.second->Initialize();
		}
	}
	while (!mGame->mActorMoveList.empty()){
		auto tar = mGame->mActorMoveList.front();
		mGame->mActorMoveList.pop();
		auto actor = tar.second.Get();
		if (tar.first == ActorMove::Delete){

#ifdef _ENGINE_MODE
			if (actor->EndFinish())continue;

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

			//delete actor;

		}
		else if(tar.first == ActorMove::Create){

#ifdef _ENGINE_MODE
			Window::AddTreeViewItem(actor->Name(), actor);
#endif

			actor->Start();

			if (!actor->mTransform->GetParent()){
				actor->mTransform->SetParent(mRootObject);
			}
		}
		else if (tar.first == ActorMove::Create_DelayInitialize) {
#ifdef _ENGINE_MODE
			Window::AddTreeViewItem(actor->Name(), actor);
#endif

			//actor->Initialize();
			actor->Start();

			if (!actor->mTransform->GetParent()) {
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
System* Game::System(){
	return &mSystemHelper;
}
std::vector<std::string>& Game::GetLayerNames(){
	return g_SettingObject.PhysxLayer.object->GetSelects();
}
void Game::RemovePhysXActor(PxActor* act){
	return gpPhysX3Main->RemoveActor(act);
}
void Game::RemovePhysXActorEngine(PxActor* act){
	return gpPhysX3Main->RemoveActorEngine(act);
}
GameObject Game::GetRootActor(){
	return mRootObject;
}
#ifdef _ENGINE_MODE
GameObject Game::GetEngineRootActor() {
	return mEngineRootObject;
}
EditorCamera * Game::GetEditorCamera()
{
	return &mCamera;
}
#endif
GameObject Game::FindActor(Actor* actor){

	for (auto& act : (*gpList)){
		if (act.second.Get() == actor){
			return act.second;
		}
	}
	return NULL;
}
#ifdef _ENGINE_MODE
GameObject Game::FindEngineActor(Actor* actor){

	
	for (auto act : mEngineRootObject->mTransform->Children()){
		if (act.Get() == actor){
			return act;
		}
	}
	return NULL;
}
#endif


GameObject Game::FindNameActor(const char* name){

	for (auto& act : (*gpList)){
		if (act.second->Name() == name){
			return act.second;
		}
	}
	return NULL;
}

GameObject Game::FindUID(UniqueID uid){
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

void Game::SetMainCameraEngineUpdate(CameraComponent* Camera){
	*gMainCameraEngineUpdate = Camera;
}
void Game::WindowParentSet(GameObject child)
{
	if (!child)return;
	//ツリービューで親子関係のセット関数
	if (auto par = child->mTransform->GetParent()) {
		if (par.Get() == mRootObject.Get()) {
			auto coll = CreateSetParentTreeViewItemCollRoot(child.Get());
			coll();
			return;
		}

		//ツリービューが完成するまで繰り返す関数
		auto coll = CreateSetParentTreeViewItemColl(par.Get(), child.Get());
		//とりあえず１回実行
		coll();
	}
}
#endif

void Game::LoadScene(const std::string& FilePath){

	((EngineDeltaTime*)gpDeltaTime)->Reset();

	TransformComponent* t = (TransformComponent*)mRootObject->mTransform.Get();
	//t->AllChildrenDestroy();

	auto children = t->Children();
	for (auto child : children){
		Game::DestroyObject(child.lock());
	}

	m_Scene.LoadScene(FilePath);
}



void Game::ChangePlayGame(bool isPlay){

#ifdef _ENGINE_MODE
	if (mIsPlay == isPlay)return;

	mIsPlay = isPlay;
	gIsPlay = mIsPlay;
	if (isPlay){

		mPhysX3Main->DisplayInitialize();

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
		mSystemHelper.Initialize();

		mSelectActor.SetSelect(NULL);

		AllDestroyObject();

		m_Scene.MemoryLoadScene();
	}
#else
	(void)isPlay;
#endif
}
bool g_IsGameStopFrame = false;

#include "../Engine/AssetFile/Material/TextureFileData.h"
void Game::Draw(){
	static UINT w = 1;
	static UINT h = 1;
	if (w != WindowState::mWidth || h != WindowState::mHeight){
		w = WindowState::mWidth;
		h = WindowState::mHeight;
		Device::Resize(w, h);
		m_DeferredRendering.Resize(w, h);
		mMainViewRenderTarget.Resize(w, h);
	}


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)WindowState::mWidth;
	vp.Height = (FLOAT)WindowState::mHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	render->m_Context->RSSetViewports(1, &vp);
	render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


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
	TextureAsset skyTex = mMainCamera->GetSkyTexture();
#ifdef _ENGINE_MODE
	if(mMainCameraEngineUpdate){
		skyTex = mMainCameraEngineUpdate->GetSkyTexture();
	}
#endif
	m_DeferredRendering.SetSkyTexture(skyTex);

	//Device::mRenderTargetBack->ClearView(Device::mpImmediateContext);
	Device::mRenderTargetBack->ClearDepth(render->m_Context);
	mMainViewRenderTarget.ClearDepth(render->m_Context);
	

	render->PushSet(DepthStencil::Preset::DS_All_Less);
	render->PushSet(Rasterizer::Preset::RS_Back_Solid);

	//const RenderTarget* r[1] = { &mMainViewRenderTarget };
	//RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], &mMainViewRenderTarget);
	mMainViewRenderTarget.SetRendererTarget(render->m_Context);

	//mMainCamera->ScreenClear();

	mCBGameParameter.mParam.Time.x++;
	mCBGameParameter.mParam.Time.y += mDeltaTime.GetDeltaTime();
	mCBGameParameter.mParam.Time.z = mDeltaTime.GetDeltaTime();
	mCBGameParameter.UpdateSubresource(render->m_Context);
	mCBGameParameter.VSSetConstantBuffers(render->m_Context);
	mCBGameParameter.PSSetConstantBuffers(render->m_Context);
	mCBGameParameter.GSSetConstantBuffers(render->m_Context);
	mCBGameParameter.CSSetConstantBuffers(render->m_Context);


	mCBScreen.mParam.ScreenSize = XMFLOAT2((float)WindowState::mWidth, (float)WindowState::mHeight);
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
	static bool g_DebugRender = false;
	if( Input::Trigger(KeyCode::Key_F1)){
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
			//mMainCamera->ScreenClear();
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
			//mMainCamera->ScreenClear();
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

	if (g_IsGameStopFrame)
		mDrawList[DrawStage::Engine].clear();
#endif


	mPostEffectRendering.Flip(render);

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
#ifdef _ENGINE_MODE
	if (!g_IsGameStopFrame)
#endif
		SetMainCamera(NULL);
#ifdef _ENGINE_MODE
	SetMainCameraEngineUpdate(NULL);
#endif

	{
		ID3D11ShaderResourceView *const pNULL[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		ID3D11SamplerState *const pSNULL[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		render->m_Context->PSSetShaderResources(0, 8, pNULL);
		render->m_Context->PSSetSamplers(0, 8, pSNULL);
		render->m_Context->VSSetShaderResources(0, 8, pNULL);
		render->m_Context->VSSetSamplers(0, 8, pSNULL);
		render->m_Context->GSSetShaderResources(0, 8, pNULL);
		render->m_Context->GSSetSamplers(0, 8, pSNULL);
		render->m_Context->PSSetShader(NULL, NULL, 0);
		render->m_Context->VSSetShader(NULL, NULL, 0);
		render->m_Context->GSSetShader(NULL, NULL, 0);
		render->m_Context->CSSetShader(NULL, NULL, 0);
	}
}


void Game::SaveScene(){
	m_Scene.SaveScene();
	g_SettingObject.Save();
}

void Game::Update(){
	mDeltaTime.Tick();
	ActorMoveStage();
#ifdef _ENGINE_MODE
	mSelectActor.UpdateInspector();

	mProfileViewer.Update();
	if (mIsPlay){
		static float forceStop = 0.0f;
		if (Input::Down(KeyCode::Key_ESCAPE)) {
			forceStop += mDeltaTime.GetNoScaleDeltaTime();
			if (forceStop > 1.0f) {
				ChangePlayGame(false);
			}
		}
		else {
			forceStop = 0.0f;
		}

		if (!g_IsGameStopFrame) {
			GamePlay();
		}
		else {
			//mDeltaTime.Reset();
		}
		if (Input::Trigger(KeyCode::Key_F12)) {
			g_IsGameStopFrame = !g_IsGameStopFrame;
		}
	}
	else{
		GameStop();
	}

	mFPS.Update();
#else
	GamePlay();
#endif
}
#ifdef _ENGINE_MODE
void Game::GameStop(){
	mDeltaTime.SetTimeScale(1.0f);

	mCamera.Update();

	if (EngineInput::Down(KeyCode::Key_LCONTROL) && EngineInput::Trigger(KeyCode::Key_S)) {
		SaveScene();
	}
	if (EngineInput::Down(KeyCode::Key_LCONTROL) && EngineInput::Trigger(KeyCode::Key_Z)){
		mCommandManager.Undo();
	}
	else if (EngineInput::Down(KeyCode::Key_LCONTROL) && EngineInput::Trigger(KeyCode::Key_Y)){
		mCommandManager.Redo();
	}

	mSelectActor.Update();

	mRootObject->EngineUpdateComponent();

	Game::AddDrawList(DrawStage::Engine, [&](){
		mWorldGrid.Draw();
	});

	mPhysX3Main->EngineDisplay();
}
#endif
void Game::GamePlay(){
	mSystemHelper.Update();
	float deltaTime = mDeltaTime.GetDeltaTime();

	mRootObject->UpdateComponent();
	mPhysX3Main->Display();
}

void Game::ClearDrawList(){
#ifdef _ENGINE_MODE
	if (g_IsGameStopFrame)return;
#endif
	for (auto &list : mDrawList) {
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


void Game::GetAllObject(const std::function<void(GameObject)>& collbak){

	for (auto& actpair : *gpList){
		collbak(actpair.second);
	}
}