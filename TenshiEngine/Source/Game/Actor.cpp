#include "Actor.h"
#include "Game/Component/ComponentFactory.h"
#include "Game/Component/ScriptComponent.h"
#include "Game/Component/PointLightComponent.h"
#include "Game/Component/MeshDrawComponent.h"
#include "MySTL/File.h"
#include "MySTL/ptr.h"
#include "Game.h"

#include "MySTL/ioHelper.h"

class UniqueIDGenerator{
public:
	UniqueIDGenerator(const char* file)
	{
		mID = 0;
		if (!mFile.Open(file)){
			mFile.FileCreate();
		}
		if (mFile)
			mFile.In(&mID);
	}
	~UniqueIDGenerator(){
		mFile.Clear();
		mFile.Out(mID);
	}
	UINT CreateUniqueID(){
		return ++mID;
	}
private:
	UINT mID;
	File mFile;
};

static UniqueIDGenerator gUniqueIDGenerator("./UniqueID.txt");


Actor::Actor()
	:mComponents(this)
	, mTreeViewPtr(NULL)
	, mTransform(NULL)
{
	mName = "new Object";
	mUniqueID = 0;
}
Actor::~Actor()
{
}

void Actor::Initialize(){
	for (const auto& cmp : mComponents.mComponent){
		cmp.second->_Initialize(this);
	}
	mComponents.RunInitialize();
}
void Actor::Start(){
	mComponents.RunStart();
}
void Actor::Finish(){
	mComponents.RunFinish();
}

void Actor::EngineUpdateComponent(float deltaTime){
	mTransform->EngineUpdate();
	for (const auto& cmp : mComponents.mComponent){
		if (cmp.second.Get() == mTransform.Get())continue;
		cmp.second->EngineUpdate();
	}


	for (auto child : mTransform->Children()){
		child->EngineUpdateComponent(deltaTime);
	}
}
void Actor::UpdateComponent(float deltaTime){
	Update(deltaTime);

	mTransform->Update();
	for (const auto& cmp : mComponents.mComponent){
		if (cmp.second.Get() == mTransform.Get())continue;
		cmp.second->Update();
	}

	for (auto child : mTransform->Children()){
		child->UpdateComponent(deltaTime);
	}

}
void Actor::Update(float deltaTime){
	(void)deltaTime;
	std::queue<std::function<void()>> temp;
	mUpdateStageCollQueue.swap(temp);
	while (!temp.empty()){
		temp.front()();
		temp.pop();
	}
}

void Actor::SetUpdateStageCollQueue(const std::function<void()> coll){
	mUpdateStageCollQueue.push(coll);
}

void Actor::CreateInspector(){
	std::function<void(std::string)> collback = [&](std::string name){
		mName = name;
		if (mTreeViewPtr)
			Window::ChangeTreeViewName(mTreeViewPtr,name);
	};
	auto data = Window::CreateInspector();
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Name", &mName, collback), data);
	Window::ViewInspector("Actor",NULL,data);
	for (const auto& cmp : mComponents.mComponent){
		cmp.second->CreateInspector();
	}
}


void Actor::CopyData(Actor* post, Actor* base){

	for (const auto& cmp : base->mComponents.mComponent){
		Component* basecmp = cmp.second.Get();
		Component* postcmp;
		auto postcmpite = post->GetComponent(cmp.first);
		if (postcmpite){
			postcmp = postcmpite.Get();
		}
		else{
			auto c = ComponentFactory::Create(cmp.second->ClassName());
			post->mComponents.AddComponent_NotInitialize(c);
			postcmp = c.Get();
		}
		postcmp->CopyData(postcmp, basecmp);
	}
}
void Actor::ExportSceneDataStart(const std::string& pass, File& sceneFile){
	for (auto child : mTransform->Children()){
		child->ExportSceneData(pass, sceneFile);
	}
}
void Actor::ExportSceneData(const std::string& pass, File& sceneFile){
	ExportData(pass);
	sceneFile.Out(mUniqueID);
	for (auto child : mTransform->Children()){
		child->ExportSceneData(pass, sceneFile);
	}
}

void Actor::ExportData(const std::string& path, const std::string& fileName){

	if (!mUniqueID){
		mUniqueID = gUniqueIDGenerator.CreateUniqueID();
	}

#define _KEY(x) io->func( x , #x)
	I_ioHelper* io = new FileOutputHelper(path + "/" + fileName + ".json");

	_KEY(mUniqueID);
	_KEY(mName);


	io->pushObject();

	for (const auto& cmp : mComponents.mComponent){
		io->pushObject();

		cmp.second->IO_Data(io);

		auto obj = io->popObject();
		io->func(obj, cmp.second->ClassName().c_str());
	}

	auto obj = io->popObject();
	io->func(obj, "components");

	delete io;
#undef _KEY
}
void Actor::ExportData(const std::string& path){

	if (!mUniqueID){
		mUniqueID = gUniqueIDGenerator.CreateUniqueID();
	}
	ExportData(path, "Object_" + std::to_string(mUniqueID));
}

void Actor::ExportData(picojson::value& json){

	if (!mUniqueID){
		mUniqueID = gUniqueIDGenerator.CreateUniqueID();
	}

#define _KEY(x) io->func( x , #x)
	I_ioHelper* io = new MemoryOutputHelper(json);

	_KEY(mUniqueID);
	_KEY(mName);


	io->pushObject();

	for (const auto& cmp : mComponents.mComponent){
		io->pushObject();

		cmp.second->IO_Data(io);

		auto obj = io->popObject();
		io->func(obj, cmp.second->ClassName().c_str());
	}

	auto obj = io->popObject();
	io->func(obj, "components");

	delete io;
#undef _KEY
}


bool Actor::ImportDataAndNewID(const std::string& fileName){
	ImportData(fileName);

	if (!mTransform){
		return false;
	}

	mUniqueID = gUniqueIDGenerator.CreateUniqueID();

	return true;
}

void Actor::CreateNewID(){
	mUniqueID = gUniqueIDGenerator.CreateUniqueID();
}


void Actor::ImportData(const std::string& fileName){

	mComponents.mComponent.clear();

#define _KEY(x) io->func( x , #x)
	I_ioHelper* io = new FileInputHelper(fileName);
	if (io->error){
		delete io;
		io = new FileInputHelper(fileName + ".json");
		if (io->error)return;
	}

	_KEY(mUniqueID);
	_KEY(mName);


	picojson::object components;
	io->func(components, "components");

	for (auto com : components){
		//if (!f.In(&temp))break;
		io->pushObject(com.second.get<picojson::object>());

		if (auto p = ComponentFactory::Create(com.first)){
			//p->ImportData(f);
			p->IO_Data(io);
			mComponents.AddComponent_NotInitialize(p);
			if (dynamic_cast<TransformComponent*>(p.Get())){
				mTransform = p;
			}
		}

		io->popObject();
	}

	delete io;
#undef _KEY
}
void Actor::ImportDataAndNewID(picojson::value& json){

	mComponents.mComponent.clear();

#define _KEY(x) io->func( x , #x)
	I_ioHelper* io = new MemoryInputHelper(json);
	if (io->error){
		return;
	}

	_KEY(mUniqueID);
	mUniqueID = gUniqueIDGenerator.CreateUniqueID();
	_KEY(mName);


	picojson::object components;
	io->func(components, "components");

	for (auto com : components){
		//if (!f.In(&temp))break;
		io->pushObject(com.second.get<picojson::object>());

		if (auto p = ComponentFactory::Create(com.first)){
			//p->ImportData(f);
			p->IO_Data(io);
			mComponents.AddComponent_NotInitialize(p);
			if (dynamic_cast<TransformComponent*>(p.Get())){
				mTransform = p;
			}
		}

		io->popObject();
	}

	delete io;
#undef _KEY
}

//https://github.com/satoruhiga/ofxEulerAngles/blob/master/src/ofxEulerAngles.h
XMVECTOR toEulerXYZ(const XMMATRIX &m)
{
	XMVECTOR v;
	v.w = 1;

	float &thetaX = v.x;
	float &thetaY = v.y;
	float &thetaZ = v.z;

	const float &r00 = m._11;
	const float &r01 = m._21;
	const float &r02 = m._31;

	const float &r10 = m._12;
	const float &r11 = m._22;
	const float &r12 = m._32;

	//const float &r20 = m._13;
	//const float &r21 = m._23;
	const float &r22 = m._33;

	if (r02 < +1)
	{
		if (r02 > -1)
		{
			thetaY = asinf(r02);
			thetaX = atan2f(-r12, r22);
			thetaZ = atan2f(-r01, r00);
		}
		else     // r02 = -1
		{
			// Not a unique solution: thetaZ - thetaX = atan2f(r10,r11)
			thetaY = -XM_PI / 2;
			thetaX = -atan2f(r10, r11);
			thetaZ = 0;
		}
	}
	else // r02 = +1
	{
		// Not a unique solution: thetaZ + thetaX = atan2f(r10,r11)
		thetaY = +XM_PI / 2;
		thetaX = atan2f(r10, r11);
		thetaZ = 0;
	}

	//thetaX = ofRadToDeg(thetaX);
	//thetaY = ofRadToDeg(thetaY);
	//thetaZ = ofRadToDeg(thetaZ);

	return v;
}

#include "../PhysX/PhysX3.h"
//PhysXテスト用
void Actor::SetTransform(physx::PxTransform* t){
	physx::PxTransform& pT = *t;// PxShapeExt::getGlobalPose(*pShape, *actor);
	//PxBoxGeometry bg;
	//pShape->getBoxGeometry(bg);
	XMVECTOR pos = XMVectorSet(pT.p.x, pT.p.y, pT.p.z,1);
	mTransform->WorldPosition(pos);

	auto q = XMVectorSet(pT.q.x, pT.q.y, pT.q.z, pT.q.w);
	mTransform->Rotate(q);

	return;

	//auto rotate = toEulerXYZ(XMMatrixRotationQuaternion(q));
	
	//XMMatrixRotationRollPitchYawFromVector

	//mTransform->Rotate(rotate);
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