#include "Actor.h"
#include "Component.h"
#include "MySTL/File.h"
#include "../MySTL/ptr.h"
#include "Game.h"

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
	for (const auto& cmp : mComponents.mComponent){
		cmp.second->Initialize();
	}
}
void Actor::Start(){
	for (const auto& cmp : mComponents.mComponent){
		cmp.second->Start();
	}
}
void Actor::Finish(){
	for (const auto& cmp : mComponents.mComponent){
		cmp.second->Finish();
	}
}

void Actor::DrawOnlyUpdateComponent(float deltaTime){
	mTransform->Update();
	auto cmp = mComponents.GetComponent<MeshDrawComponent>();
	if (cmp)
		cmp->Update();
	auto cmp2 = mComponents.GetComponent<PointLightComponent>();
	if (cmp2)
		cmp2->Update();

	for (auto child : mTransform->Children()){
		child->DrawOnlyUpdateComponent(deltaTime);
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

bool Actor::ChackHitRay(const XMVECTOR& pos, const XMVECTOR& vect){
	shared_ptr<ModelComponent> mModelComponent = mComponents.GetComponent<ModelComponent>();
	if (mModelComponent)
		return mModelComponent->HitChack(pos, vect);

	XMVECTOR v = mTransform->Position() - pos;
	XMVECTOR lv = XMVector3Length(v);
	float l = XMVectorGetX(lv);
	XMVECTOR p = pos + vect * l;
	v = mTransform->Position() - p;
	lv = XMVector3Length(v);
	l = XMVectorGetX(lv);
	float r = 0.5f;
	if (r*r > l*l){
		return true;
	}
	return false;
}


void Actor::CreateInspector(){
	std::function<void(std::string)> collback = [&](std::string name){
		mName = name;
		if (mTreeViewPtr)
			Window::ChangeTreeViewName(mTreeViewPtr,name);
	};
	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorStringDataSet("Name", &mName, collback), data);
	Window::ViewInspector("Actor",data);
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
void Actor::ExportData(const std::string& pass){

	File f;
	if (!mUniqueID){
		mUniqueID = gUniqueIDGenerator.CreateUniqueID();
	}

	if (!f.Open(pass + "/Object_" + std::to_string(mUniqueID) + ".txt"))
		f.FileCreate();
	f.Clear();
	if (!f)return;

	f.Out(mUniqueID);

	auto name = mName;
	int ioc = name.find(" ");
	while (std::string::npos != ioc){
		name.replace(ioc, 1, "$");
		ioc = name.find(" ");
	}

	f.Out(name);

	if (mTransform->GetParent()){
		f.Out(mTransform->GetParent()->GetUniqueID());
	}
	else{
		f.Out(NULL);
	}

	for (const auto& cmp : mComponents.mComponent){
		cmp.second->ExportData(f);
	}
}

void Actor::ImportDataAndNewID(const std::string& fileName){
	ImportData(fileName);
	mUniqueID = gUniqueIDGenerator.CreateUniqueID();

}
void Actor::ImportData(const std::string& fileName){
	

	File f(fileName);
	if (!f)return;

	f.In(&mUniqueID);
	f.In(&mName);
	int ioc = mName.find("$");
	while (std::string::npos != ioc){
		mName.replace(ioc, 1, " ");
		ioc = mName.find("$");
	}

	UINT uid;
	f.In(&uid);
	
	std::string temp;

	mComponents.mComponent.clear();

	while (f){
		if (!f.In(&temp))break;
		if (auto p = ComponentFactory::Create(temp)){
			p->ImportData(f);
			mComponents.AddComponent_NotInitialize(p);
			if (dynamic_cast<TransformComponent*>(p.Get())){
				mTransform = p;
			}
		}
	}

	((TransformComponent*)mTransform.Get())->mParentUniqueID = uid;
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

	const float &r20 = m._13;
	const float &r21 = m._23;
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
	mTransform->Position(pos);

	auto q = XMVectorSet(pT.q.x, pT.q.y, pT.q.z, pT.q.w);
	mTransform->Rotate(q);
	return;

	//float yaw = atan2(2 * gyro.x * gyro.y + 2 * gyro.w * gyro.z, gyro.w * gyro.w + gyro.x * gyro.x - gyro.y * gyro.y - gyro.z * gyro.z);
	//float pitch = asin(2 * gyro.w * gyro.y - 2 * gyro.x * gyro.z);
	//float roll = atan2(2 * gyro.y * gyro.z + 2 * gyro.w * gyro.x, -gyro.w * gyro.w + gyro.x * gyro.x + gyro.y * gyro.y - gyro.z * gyro.z);
	//auto rotate = XMVectorSet(pitch,yaw,roll, 1);

	///float roll = atan2(2 * y*w - 2 * x*z, 1 - 2 * y*y - 2 * z*z);
	///float pitch = atan2(2 * x*w - 2 * y*z, 1 - 2 * x*x - 2 * z*z);
	///float yaw = asin(2 * x*y + 2 * z*w);
	///auto rotate = XMVectorSet(pitch, yaw, roll, 1);

	//XMMatrixRotationRollPitchYawFromVector(,gyro);

	auto rotate = toEulerXYZ(XMMatrixRotationQuaternion(q));
	
	//XMMatrixRotationRollPitchYawFromVector

	mTransform->Rotate(rotate);
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