#include "Actor.h"
#include "Game/Component/ComponentFactory.h"
#include "Game/Component/ScriptComponent.h"
#include "Game/Component/PointLightComponent.h"
#include "Game/Component/MeshDrawComponent.h"
#include "Game/Component/PhysxColliderComponent.h"
#include "Game/Component/CharacterControllerComponent.h"
#include "MySTL/File.h"
#include "MySTL/ptr.h"
#include "Game.h"

#include "MySTL/ioHelper.h"

#include "Engine/AssetFile/Prefab/PrefabFileData.h"

#include "Engine/Inspector.h"

Actor::Actor()
	: mTreeViewPtr(NULL)
	, mTransform(NULL)
	, mEndInitialize(false)
	, mEndStart(false)
	, mEndFinish(false)
{
	mName = "new Object";
	mPhysxLayer = 0;

	m_InspectorFindGameObjectFunc = [](auto id) {return Game::FindUID(id); };
}
Actor::~Actor()
{
}

void Actor::PlayInitializeStageColl()
{
	std::queue<std::function<void()>> temp;
	mInitializeStageCollQueue.swap(temp);
	while (!temp.empty()) {
		temp.front()();
		temp.pop();
	}
}

void Actor::Initialize(){
	if (mEndInitialize)return;
	PlayInitializeStageColl();

	mComponents.Initialize(this->shared_from_this());

	for (auto cmp : mComponents.GetComponents()){
		cmp.second->_Initialize(this->shared_from_this());
	}
	mTransform->Initialize();
	RunChangeParentCallback();
	mComponents.RunInitialize();
	mEndInitialize = true;
	mEndFinish = false;
}
void Actor::Start(){
	if (mEndStart)return;
	mComponents.RunStart();
	mEndStart = true;
	mEndFinish = false;
}
void Actor::Finish(){
	if (mEndFinish)return;
	m_InspectorFindGameObjectFunc = [](auto o)->GameObject {return NULL; };
	mComponents.RunFinish();
	mEndInitialize = false;
	mEndStart = false;
	mEndFinish = true;
}
#ifdef _ENGINE_MODE
void Actor::Initialize_Script(){
	if (auto com = mComponents.GetComponent<ScriptComponent>()){
		com->Initialize_Script();
	}
}
void Actor::Start_Script(){
	if (auto com = mComponents.GetComponent<ScriptComponent>()){
		com->Start_Script();
	}
	
}
void Actor::EngineUpdateComponent(){
	Update();
	
	//ツリービュー作成のため
	if (!IsEnabled()){
		for (auto child : mTransform->Children()){
			child->EngineUpdateComponent();
		}
		return;
	}

	mTransform->EngineUpdate();
	for (auto cmp : mComponents.GetComponents()){
		if (cmp.second.Get() == mTransform.Get())continue;

		if (!cmp.second->IsEnabled())continue;
		cmp.second->EngineUpdate();
	}

	for (auto child : mTransform->Children()){
		child->EngineUpdateComponent();
	}
}
#endif

void Actor::UpdateComponent(){
	if (!mEndStart)return;
	if (!IsEnabled()){return;}

	Update();

	mTransform->Update();
	for (auto cmp : mComponents.GetComponents()){
		if (cmp.second.Get() == mTransform.Get())continue;
		if (!cmp.second->IsEnabled())continue;
		cmp.second->Update();
	}

	for (auto child : mTransform->Children()){
		child->UpdateComponent();
	}

}
void Actor::Update(){
	std::queue<std::function<void()>> temp;
	mUpdateStageCollQueue.swap(temp);
	while (!temp.empty()){
		temp.front()();
		temp.pop();
	}
}

void Actor::SetInitializeStageCollQueue(const std::function<void()>& coll)
{
	if (mEndInitialize) {
		coll();
		return;
	}
	mInitializeStageCollQueue.push(coll);
}

void Actor::SetUpdateStageCollQueue(const std::function<void()>& coll){
	mUpdateStageCollQueue.push(coll);
}
#ifdef _ENGINE_MODE
void Actor::CreateInspector(){
	std::function<void(std::string)> collback = [&](std::string name){
		mName = name;
		if (mTreeViewPtr)
			Window::ChangeTreeViewName(mTreeViewPtr,name);
	};
	std::function<void(std::string)> collbackpre = [&](std::string name){
		mPrefab = name;
		AssetDataBase::Instance(mPrefab.c_str(), mPrefabAsset);
	};
	Inspector ins("GameObject",NULL);
	ins.AddEnableButton(this);
	ins.Add("Name", &mName, collback);
	ins.Add("Prefab", &mPrefab, collbackpre);
	ins.AddSelect("Layer", &mPhysxLayer, Game::GetLayerNames(), [&](int f){
		SetLayer(f);
	});
	

	ins.Complete();
	for (auto cmp : mComponents.GetComponents()){
		cmp.second->CreateInspector();
	}
}
#endif

//void Actor::ExportSceneDataStart(const std::string& pass, File& sceneFile){
//	for (auto child : mTransform->Children()){
//		child->ExportSceneData(pass, sceneFile);
//	}
//}
//void Actor::ExportSceneData(const std::string& pass, File& sceneFile){
//	ExportData(pass);
//	sceneFile.Out(mUniqueID);
//	for (auto child : mTransform->Children()){
//		child->ExportSceneData(pass, sceneFile);
//	}
//}

void Actor::ExportData(const std::string& fileName, bool childExport, bool worldTransform){

	shared_ptr<I_InputHelper> prefab_io(NULL);
	//if (mPrefabAsset && path != "Assets"){
	//	prefab_io = mPrefabAsset->GetFileData()->GetData();
		//prefab_io = new FileInputHelper(mPrefab);
		//if (prefab_io->error){
		//	delete prefab_io;
		//}
	//}

	I_ioHelper* io = new FileOutputHelper(fileName, prefab_io.Get());
	if (io->error){
		delete io;
		return;
	}

	_ExportData(io, childExport, worldTransform);


	delete io;
}
//void Actor::ExportData(const std::string& path){
//
//	if (!mUniqueID){
//		CreateNewID();
//	}
//	ExportData(path, "Object_" + std::to_string(mUniqueID));
//}

void Actor::ExportData(picojson::value& json, bool childExport, bool worldTransform){

	shared_ptr<I_InputHelper> prefab_io(NULL);
	if (mPrefabAsset){
		prefab_io = mPrefabAsset->GetFileData()->GetData();
	}


	I_ioHelper* io = new MemoryOutputHelper(json, prefab_io.Get());

	_ExportData(io,childExport, worldTransform);

	delete io;
}


void Actor::_ExportData(I_ioHelper* io, bool childExport, bool worldTransform){

	if (mUniqueHash == ""){
		CreateNewID();
	}

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
#define _KEY_COMPEL(x) io->func( x , #x,true)

	_KEY_COMPEL(mUniqueHash);
	_KEY_COMPEL(mName);
	_KEY_COMPEL(mPrefab);
	_KEY_COMPEL(mPhysxLayer);


	io->pushObject("components");
	if(worldTransform)
		TransformComponent::ExportWorldTransform = true;

	for (auto cmp : mComponents.GetComponents()){
		io->pushObject(cmp.second->ClassName());

		cmp.second->IO_Data(io);
		io->popObject();

	}
	io->popObject();

	TransformComponent::ExportWorldTransform = false;



	if (childExport){

		io->pushObject("children");

		int i = 0;
		for (auto child : mTransform->Children()){
			
			io->pushObject(std::to_string(i));

			picojson::value obj;
			child->ExportData(obj, childExport);
			io->func(obj,"child");
			io->popObject();
			i++;
		}
		io->popObject();
	}


#undef _KEY
#undef _KEY_COMPEL
}

//イニシャライズ周りが危ない？？？
//void Actor::PastePrefabParam(picojson::value& json){
//
//	mComponents.RunFinish();
//
//	picojson::value param;
//	auto filter = new MemoryInputHelper(json,NULL);
//	auto io_out = new MemoryOutputHelper(param, filter, true);
//
//	_ExportData(io_out);
//
//	delete io_out;
//	delete filter;
//
//
//
//	MemoryInputHelper* io_in = NULL;
//	MemoryInputHelper* filter_in = NULL;
//	if (mPrefabAsset){
//		auto val = *mPrefabAsset->GetFileData()->GetParam();
//
//		filter_in = new MemoryInputHelper(param, NULL);
//		io_in = new MemoryInputHelper(val, filter_in);
//	}
//
//
//	if (io_in){
//		picojson::object components;
//		io_in->func(components, "components");
//
//		io_in->pushObject("components");
//
//		for (auto com : components){
//			io_in->pushObject(com.first);
//
//			auto component = mComponents.GetComponent(com.first);
//			bool create = false;
//			if (!component){
//				component = ComponentFactory::Create(com.first);
//				create = true;
//			}
//
//			if (component){
//				component->IO_Data(io_in);
//				if (create){
//					mComponents.AddComponent_NotInitialize(component);
//
//				}
//			}
//
//			io_in->popObject();
//		}
//
//		//_ImportData(io_in);
//
//
//		delete filter_in;
//		delete io_in;
//	}
//	mComponents.RunInitialize();
//	mComponents.RunStart();
//}


bool Actor::ImportDataAndNewID(const std::string& fileName, const std::function<void(shared_ptr<Actor>)>& childstackfunc){

	ImportData(fileName, childstackfunc, true);

	if (!mTransform){
		return false;
	}

	CreateNewID();

	return true;
}

#include "Library/MD5.h"
void Actor::CreateNewID(){
	MD5::MD5HashCode hash;
	MD5::GenerateMD5(hash);
	mBeforeUniqueHash = mUniqueHash;
	mUniqueHash = hash;

	if (!mTransform)return;
	//UniqueID登録しなおし
	for (auto child : mTransform->Children()){
		child->mTransform->SetParentUniqueID(mUniqueHash);
	}
}

void Actor::SetLayer(int layer)
{
	mPhysxLayer = layer;
	if (auto com = mComponents.GetComponent<PhysXColliderComponent>()) {
		com->SetPhysxLayer(mPhysxLayer);
	}
	if (auto com = mComponents.GetComponent<CharacterControllerComponent>()) {
		com->SetPhysxLayer(mPhysxLayer);
	}
}


void Actor::ImportData(const std::string& fileName, const std::function<void(shared_ptr<Actor>)>& childstackfunc, bool newID){

	I_ioHelper* io = new FileInputHelper(fileName, NULL);
	if (io->error){
		delete io;
		io = new FileInputHelper(fileName + ".prefab", NULL);
		if (io->error){
			delete io;
			return;
		}
	}


	_ImportData(io, childstackfunc, newID);

	delete io;
}

void Actor::ImportData(picojson::value& json, const std::function<void(shared_ptr<Actor>)>& childstackfunc, bool newID){

	I_ioHelper* io = new MemoryInputHelper(json, NULL);
	if (io->error){
		delete io;
		return;
	}

	_ImportData(io, childstackfunc, newID);

	delete io;
}

void Actor::ImportDataAndNewID(picojson::value& json, const std::function<void(shared_ptr<Actor>)>& childstackfunc){

	I_ioHelper* io = new MemoryInputHelper(json, NULL);
	if (io->error){
		delete io;
		return;
	}

	_ImportData(io, childstackfunc,true);

	CreateNewID();

	delete io;
}

void Actor::_ImportData(I_ioHelper* io, const std::function<void(shared_ptr<Actor>)>& childstackfunc, bool newID){

	mComponents.Initialize(this->shared_from_this());

	mComponents.mComponent.clear();

#define _KEY(x) io->func( x , #x)

	Enabled::IO_Data(io);

	_KEY(mUniqueHash);
	_KEY(mName);
	_KEY(mPrefab);
	_KEY(mPhysxLayer);

	if (mPrefab!=""){
		AssetDataBase::Instance(mPrefab.c_str(), mPrefabAsset);
		if (mPrefabAsset){
			auto prefab_io = mPrefabAsset->GetFileData()->GetData();
			if (!prefab_io->error){

				picojson::object components;
				prefab_io->func(components, "components");

				prefab_io->pushObject("components");

				for (auto com : components){
					prefab_io->pushObject(com.first);

					if (auto component = ComponentFactory::Create(com.first)){
						mComponents.AddComponent_NotInitialize(component);
						component->IO_Data(prefab_io.Get());
					}

					prefab_io->popObject();
				}

			}
		}
	}


	picojson::object components;
	io->func(components, "components");

	io->pushObject("components");

	for (auto com : components){
		io->pushObject(com.first);

		auto component = mComponents.GetComponent(com.first);
		bool create = false;
		if (!component){
			component = ComponentFactory::Create(com.first);
			create = true;
		}

		if (component){
			if (create){
				mComponents.AddComponent_NotInitialize(component);
			}
			component->IO_Data(io);
		}

		io->popObject();
	}

	mTransform = mComponents.GetComponent<TransformComponent>();

	io->popObject();

	picojson::object children;
	io->func(children, "children");
	
	io->pushObject("children");
	
	for (auto child : children){
		io->pushObject(child.first);
	
		picojson::object childobj;
		io->func(childobj, "child");
	
		io->pushObject("child");
	
		auto a = make_shared<Actor>();
		auto value = (picojson::value)childobj;
		a->SetInspectorFindGameObjectFunc(m_InspectorFindGameObjectFunc);
		if(newID)
			a->ImportDataAndNewID(value, childstackfunc);
		else
			a->ImportData(value, childstackfunc);
		//Game::AddObject(a);
		if (a->mTransform){
			//a->mTransform->SetParentUniqueID(mUniqueHash);
			a->mTransform->SetParent(this->shared_from_this());
		}
		childstackfunc(a);
	
		io->popObject();
		io->popObject();
	}
	

#undef _KEY

}

void* Actor::_GetScript(const char* name){
	auto com = mComponents.GetComponent<ScriptComponent>();
	if (com){
		if (("class "+com->mClassName) == std::string(name)){
			return com.Get()->pDllClass;
		}
	}
	return NULL;
}

//ペアレント変更コールバックを実行
void Actor::RunChangeParentCallback(){

	if (auto par = mTransform->GetParent()) {
		par->ChildEnableChanged(&*this);
	}

	for (auto com : mComponents.GetComponents()){
		if (!com.second)continue;
		this->ChildEnableChanged(com.second.Get());
		com.second->ChangeParentCallback();
	}
	for (auto child : mTransform->Children()){
		this->ChildEnableChanged(child.Get());
		child->RunChangeParentCallback();
	}
}


void Actor::OnEnabled(){
	for (auto com : mComponents.GetComponents()){
		this->ChildEnableChanged(com.second.Get());
	}
	for (auto child : mTransform->Children()){
		this->ChildEnableChanged(child.Get());
	}
}
void Actor::OnDisabled(){
	for (auto com : mComponents.GetComponents()){
		this->ChildEnableChanged(com.second.Get());
	}
	for (auto child : mTransform->Children()){
		this->ChildEnableChanged(child.Get());
	}
}

weak_ptr<Actor> GameObjectFindHelper::ChildrenFind(GameObject obj, const UniqueID & id)
{
	
	if (obj->GetBeforeUniqueID() == id) {
		return obj;
	}
	for (auto child : obj->mTransform->Children()) {
		auto target = GameObjectFindHelper::ChildrenFind(child,id);
		if (target) {
			return target;
		}
	}
	return NULL;
}
