
#include "Game/Game.h"
#include "ScriptComponent.h"
#include "MySTL/Reflection/Reflection.h"
#include "Window/window.h"
#include "Engine/Inspector.h"
#include "Engine/AssetLoad.h"

#include "Engine/ScriptingSystem/ScriptCompiler.h"


ScriptComponent::ScriptComponent(){
	mEndInitialize = false;
	mEndStart = false;
	pDllClass = NULL;
	mSaveParam = NULL;
}
ScriptComponent::~ScriptComponent(){
	if (mSaveParam){
		delete mSaveParam;
		mSaveParam = NULL;
	}
}
void ScriptComponent::Initialize(){

#ifdef _ENGINE_MODE
	UseScriptActors::Get()->Push(this);
#endif
	mCollideMap.clear();
	Load();

	if (pDllClass){
		pDllClass->gameObject = gameObject;
	}

#ifdef _ENGINE_MODE
	if (Game::IsGamePlay()){
		Initialize_Script();
	}
#else
	Initialize_Script();
#endif
}

void ScriptComponent::Start(){
#ifdef _ENGINE_MODE
	if (Game::IsGamePlay()){
		Start_Script();
	}

#else
	Start_Script();
#endif
}
void ScriptComponent::Initialize_Script(){

	if (!mEndInitialize){
		mEndInitialize = true;
		if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::Initialize)) {
			//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "Initialize()");
			Disable();
		}
	}

}

void ScriptComponent::Start_Script(){

	if (!mEndStart){
		mEndStart = true;
		if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::Start)) {
			//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "Start()");
			Disable();
		}
	}
}
void ScriptComponent::Load(){
	if (pDllClass)return;

	//dllで作成したクラスインスタンスを作成する
	pDllClass = UseScriptActors::Get()->Create(mClassName);

	if (pDllClass){
		pDllClass->gameObject = gameObject;
	}
}
void ScriptComponent::Unload(){

	if (pDllClass){
		if (mEndInitialize)
			if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::Finish)) {
				//Window::AddLog("["+gameObject->Name()+"]["+mClassName+"]"+"Finish()");
				Disable();
			}
		UseScriptActors::Get()->Deleter(pDllClass);
	}

	pDllClass = NULL;

	mEndInitialize = false;
	mEndStart = false;

}
void ScriptComponent::ReCompile(){

	Unload();
	Load();
}

void ScriptComponent::SaveParam(){

	if (mSaveParam){
		delete mSaveParam;
		mSaveParam = NULL;
	}
	mSaveParam = new picojson::value();
	shared_ptr<I_InputHelper> prefab_io(NULL);
	I_ioHelper* io = new MemoryOutputHelper(*mSaveParam, prefab_io.Get());
	IO_Data(io);
	delete io;

}
void ScriptComponent::LoadParam(){
	if (mSaveParam){
		shared_ptr<I_InputHelper> prefab_io(NULL);
		I_ioHelper* io = new MemoryInputHelper(*mSaveParam, prefab_io.Get());
		IO_Data(io);
		delete io;
		delete mSaveParam;
		mSaveParam = NULL;
	}
}

void ScriptComponent::Update(){

	if (pDllClass){

		if (pDllClass){
			pDllClass->gameObject = gameObject;
		}

		for (auto ite = mCollideMap.begin(); ite != mCollideMap.end();){

			auto& data = *ite;
			//不正の判定
			if (data.second.HitCount <= -1) {
				ite = mCollideMap.erase(ite);
				continue;
			}

			if (data.second.HitCount == 0){
				if (data.second.State == ColliderState::Begin) {
					if (data.second.Target)	
						if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::OnCollideBegin, data.second.Target)) {
							//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "OnCollideBegin()");
							Disable();
						}
					if (data.second.Target)
						if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::OnCollideEnter, data.second.Target)) {
							//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "OnCollideEnter()");
							Disable();
						}
					if (data.second.Target)
						if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::OnCollideExit, data.second.Target)) {
							//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "OnCollideExit()");
							Disable();
						}
				}
				if (data.second.State == ColliderState::Enter){
					if (data.second.Target)
						if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::OnCollideExit, data.second.Target)) {
							//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "OnCollideExit()");
							Disable();
						}
				}
				ite = mCollideMap.erase(ite);
				continue;

			}
			else {
				if (data.second.State == ColliderState::Begin) {
					if (data.second.Target)
						if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::OnCollideBegin, data.second.Target)) {
							//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "OnCollideBegin()");
							Disable();
						}
					data.second.State = Enter;
				}

				if (data.second.State == ColliderState::Enter) {
					if (data.second.Target)
						if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::OnCollideEnter, data.second.Target)) {
							//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "OnCollideEnter()");
							Disable();
						}
				}
			}
			//else if (data.second.State == ColliderState::Exit){
			//	actors.Function(pDllClass, &IDllScriptComponent::OnCollideExit, data.second.Target);
			//	ite = mCollideMap.erase(ite);
			//	continue;
			//}
			++ite;
		}

		if (!UseScriptActors::Get()->Function(pDllClass, &IDllScriptComponent::Update)) {
			//Window::AddLog("[" + gameObject->Name() + "][" + mClassName + "]" + "Update()");
			Disable();
		}
	}

}
void ScriptComponent::Finish(){

	Unload();
#ifdef _ENGINE_MODE
	UseScriptActors::Get()->Pop(this);
#endif
}


void ScriptComponent::OnCollide(Actor* target){
	auto ite = mCollideMap.find((int)target);
	if (ite == mCollideMap.end()) {
		mCollideMap[(int)target] = ColliderStateData(target->shared_from_this(), ColliderState::Begin, 1);
		return;
	}

	auto& get = *ite;
	if (get.second.Target){
		get.second.HitCount++;
	}


	//if (pDllClass){
	//	actors.Function(pDllClass, &IDllScriptComponent::OnCollideBegin, target);
	//}
}

void ScriptComponent::LostCollide(Actor* target){
	auto ite = mCollideMap.find((int)target);
	if (ite == mCollideMap.end()){
		mCollideMap[(int)target] = ColliderStateData(target->shared_from_this(), ColliderState::Begin, -1);
		return;
	}
	auto& get = *ite;
	if (get.second.Target){
		get.second.HitCount--;
	}


	//mCollideMap[(int)target] = ColliderStateData(target, ColliderState::Exit);

	//mCollideMap.erase((int)target);
	//if (pDllClass){
	//	actors.Function(pDllClass, &IDllScriptComponent::OnCollideExit, target);
	//}
}


#ifdef _ENGINE_MODE

template<class T>
bool reflect(MemberInfo& info, Inspector& ins){

	if (info.GetTypeName() != typeid(T).name()){
		return false;
	}

	volatile T* paramdata = Reflection::Get<T>(info);

	std::function<void(T)> collback = [paramdata](T f){
		*(T*)paramdata = f;
	};
	ins.Add(info.GetName(), (T*)paramdata, collback);

	return true;
}
bool reflect_v(MemberInfo& info, Inspector& ins) {

	if (info.GetTypeName() != typeid(XMVECTOR).name()) {
		return false;
	}

	volatile XMVECTOR* paramdata = Reflection::Get<XMVECTOR>(info);

	Vector3 v(*(XMVECTOR*)paramdata);
	ins.Add(info.GetName(), &v, [paramdata](Vector3 f) {
		paramdata->x = f.x;
		paramdata->y = f.y;
		paramdata->z = f.z;
	});

	return true;
}
bool reflect_g(MemberInfo& info, Inspector& ins) {
	
	if (info.GetTypeName() != typeid(GameObject).name()) {
		return false;
	}

	volatile GameObject* paramdata = Reflection::Get<GameObject>(info);

	ins.Add(info.GetName(), (GameObject*)paramdata, []() {});

	return true;
}

template<class T>
bool reflect_a(MemberInfo& info, Inspector& ins) {

	if (info.GetTypeName() != typeid(T).name()) {
		return false;
	}

	volatile T* paramdata = Reflection::Get<T>(info);

	ins.Add(info.GetName(), (T*)paramdata, []() {});

	return true;
}


#endif

void initparam(int* p){
	*p = 0;
}
void initparam(float* p){
	*p = 0.0f;
}
void initparam(bool* p){
	*p = false;
}
void initparam(std::string* p){
	*p = "";
}
void initparam(XMVECTOR* p) {
	*p = XMVectorSet(0,0,0,0);
}
void initparam(GameObject* p) {
	*p = NULL;
}
void initparam(IAsset* p) {
	p->m_Hash.key_i[0] = 0;
	p->m_Hash.key_i[1] = 0;
	p->m_Hash.key_i[2] = 0;
	p->m_Hash.key_i[3] = 0;
	p->m_Name = "";
}


template<class T>
bool reflect_io(MemberInfo& info,I_ioHelper* io){

	if (info.GetTypeName() != typeid(T).name()){
		return false;
	}

	T* paramdata = Reflection::Get<T>(info);

	if (io->isInput()){
		initparam(paramdata);
	}

	io->func(*paramdata, info.GetName().c_str());

	return true;
}

bool reflect_io_v(MemberInfo& info, I_ioHelper* io) {

	if (info.GetTypeName() != typeid(XMVECTOR).name()) {
		return false;
	}

	XMVECTOR* paramdata = Reflection::Get<XMVECTOR>(info);

	if (io->isInput()) {
		initparam(paramdata);
	}

	io->func(paramdata->x, (info.GetName() + "x").c_str());
	io->func(paramdata->y, (info.GetName() + "y").c_str());
	io->func(paramdata->z, (info.GetName() + "z").c_str());
	io->func(paramdata->w, (info.GetName() + "w").c_str());

	return true;
}


bool reflect_io_g(MemberInfo& info, I_ioHelper* io, Component* com) {

	if (info.GetTypeName() != typeid(GameObject).name()) {
		return false;
	}

	GameObject* paramdata = Reflection::Get<GameObject>(info);

	if (io->isInput()) {
		initparam(paramdata);
	}

	ioGameObjectHelper::func(paramdata, info.GetName().c_str(), io, &com->gameObject);


	return true;
}

template<class T>
bool reflect_io_a(MemberInfo& info, I_ioHelper* io, Component* com) {

	if (info.GetTypeName() != typeid(T).name()) {
		return false;
	}

	T* paramdata = Reflection::Get<T>(info);

	if (io->isInput()) {
		initparam((IAsset*)paramdata);
	}

	io->func(*(IAsset*)paramdata, info.GetName().c_str());

	if (io->isInput()) {
		
		AssetLoad::Instance(paramdata->m_Hash, paramdata);
	}

	return true;
}

#ifdef _ENGINE_MODE
void ScriptComponent::CreateInspector(){


	std::function<void(std::string)> collback = [&](std::string name){
		Window::ClearInspector();
		mClassName = name;
		ReCompile();
	};

	Inspector ins("Script",this);
	ins.AddEnableButton(this);
	ins.Add("Class", &mClassName, collback);

	if (Reflection::map){
		auto infos = Reflection::GetMemberInfos(pDllClass, mClassName);

		for (auto i : infos){
			auto info = Reflection::GetMemberInfo(infos,i);
			bool fal = false;
			do{
				if (reflect<float>(info, ins))break;
				if (reflect<int>(info, ins))break;
				if (reflect<bool>(info, ins))break;
				if (reflect<std::string>(info, ins))break;
				if (reflect_v(info, ins))break;
				if (reflect_g(info, ins))break;
				if (reflect_a<MetaAsset>(info, ins))break;
				if (reflect_a<MeshAsset>(info, ins))break;
				if (reflect_a<BoneAsset>(info, ins))break;
				if (reflect_a<PrefabAsset>(info, ins))break;
				if (reflect_a<ShaderAsset>(info, ins))break;
				if (reflect_a<TextureAsset>(info, ins))break;
				if (reflect_a<PhysxMaterialAsset>(info, ins))break;
				if (reflect_a<SoundAsset>(info, ins))break;
				if (reflect_a<MovieAsset>(info, ins))break;
			} while (fal);
		}

	}

	ins.Complete();
}
#endif

void ScriptComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(mClassName);
	Load();

	if (pDllClass){
		if (Reflection::map){
			auto infos = Reflection::GetMemberInfos(pDllClass, mClassName);

			for (auto i : infos){
				auto info = Reflection::GetMemberInfo(infos, i);
				bool fal = false;
				do{
					if (reflect_io<float>(info, io))break;
					if (reflect_io<int>(info, io))break;
					if (reflect_io<bool>(info, io))break;
					if (reflect_io<std::string>(info, io))break;
					if (reflect_io_v(info, io))break;
					if (reflect_io_g(info, io, this))break;
					if (reflect_io_a<MetaAsset>(info, io, this))break;
					if (reflect_io_a<MeshAsset>(info, io, this))break;
					if (reflect_io_a<BoneAsset>(info, io, this))break;
					if (reflect_io_a<PrefabAsset>(info, io, this))break;
					if (reflect_io_a<ShaderAsset>(info, io, this))break;
					if (reflect_io_a<TextureAsset>(info, io, this))break;
					if (reflect_io_a<PhysxMaterialAsset>(info, io, this))break;
					if (reflect_io_a<SoundAsset>(info, io, this))break;
					if (reflect_io_a<MovieAsset>(info, io, this))break;
				} while (fal);
			}
		}
	}

#undef _KEY
}