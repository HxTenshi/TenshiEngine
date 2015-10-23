#include "Actor.h"
#include "Component.h"
#include "MySTL/File.h"
#include "../MySTL/ptr.h"

unsigned char operator & (const unsigned char& bit, const DrawBit& bit2){
	return (unsigned char)bit&(unsigned char)bit2;
}
unsigned char operator | (const DrawBit& bit, const DrawBit& bit2){
	return (unsigned char)bit | (unsigned char)bit2;
}
unsigned char operator | (const unsigned char& bit, const DrawBit& bit2){
	return (unsigned char)bit | (unsigned char)bit2;
}

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
{
	mName = "new Object";
	mTransform = shared_ptr<TransformComponent>(new TransformComponent());
	mComponents.AddComponent<TransformComponent>(mTransform);
	//mDrawBit = 0x0000;
	mDrawBit = (DrawBit::Depth | DrawBit::Diffuse);
	mUniqueID = 0;
}
Actor::~Actor()
{
}
void Actor::UpdateComponent(float deltaTime){
	Update(deltaTime);

	for (const auto& cmp : mComponents.mComponent){
		cmp.second->Update();
	}
}
void Actor::Update(float deltaTime){
	(void)deltaTime;
}
void Actor::Draw(DrawBit drawbit)const{
	if (!(mDrawBit&drawbit))return;
	for (const auto& cmp : mComponents.mDrawComponent){
		cmp.second->Update();
	}
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
	for (const auto& cmp : mComponents.mComponent){
		cmp.second->CreateInspector();
	}
	for (const auto& cmp : mComponents.mDrawComponent){
		cmp.second->CreateInspector();
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

	for (const auto& cmp : mComponents.mComponent){
		cmp.second->ExportData(f);
	}

	for (const auto& cmp : mComponents.mDrawComponent){
		cmp.second->ExportData(f);
	}
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

	std::string temp;

	mComponents.mComponent.clear();
	mComponents.mDrawComponent.clear();
	
	while (f){
		f.In(&temp);
		if (auto p = ComponentFactory::Create(temp)){
			p->ImportData(f);
			if (dynamic_cast<DrawComponent*>(p.Get()))
				mComponents.AddComponent<DrawComponent>(p);
			else
				mComponents.AddComponent(p);
			if (dynamic_cast<TransformComponent*>(p.Get()))
				mTransform = p;
			
		}
	}

	for (const auto& cmp : mComponents.mComponent){
		cmp.second->Initialize();
	}

	for (const auto& cmp : mComponents.mDrawComponent){
		cmp.second->Initialize();
	}
}



#include "../PhysX/PhysX3.h"
#include <xnamath.h>
//PhysXテスト用
void Actor::SetTransform(physx::PxTransform* t){
	physx::PxTransform& pT = *t;// PxShapeExt::getGlobalPose(*pShape, *actor);
	//PxBoxGeometry bg;
	//pShape->getBoxGeometry(bg);
	XMVECTOR pos = XMVectorSet(pT.p.x, pT.p.y, pT.p.z,1);
	mTransform->Position(pos);

	XMVECTOR rot = XMVectorSet(pT.q.x, pT.q.y, pT.q.z, pT.q.w);
	mTransform->Rotate(rot);
}