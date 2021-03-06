#include "CharacterControllerComponent.h"

#include "physx/physx3.h"

#include "Game/Actor.h"
#include "Game/Game.h"

#include "Engine/Inspector.h"

CharacterControllerComponent::CharacterControllerComponent()
	:mController(NULL)
	//, mGravity(XMVectorSet(0, -9.81, 0, 1))
	//, mJump(XMVectorSet(0, 0, 0, 1))
	, mSlopeLimitDigree(45.0f)
	, mStepOffset(0.1f)
	, mSize(1.0f)
	, mShape(NULL)
{
}

CharacterControllerComponent::~CharacterControllerComponent(){
}
void CharacterControllerComponent::Initialize(){

	mController = Game::GetPhysX()->CreateController();
	auto act = mController->getActor();
	mController->setUserData(gameObject.Get());
	act->userData = gameObject.Get();
	act->getShapes(&mShape, 1);
	mShape->userData = gameObject.Get();
	
	SetPhysxLayer(gameObject->GetLayer());

	SetSlopeLimit(mSlopeLimitDigree);
	SetStepOffset(mStepOffset);
	SetSize(mSize);

}


void CharacterControllerComponent::Start(){
	if (!mController)return;
	auto wp = gameObject->mTransform->WorldPosition();
	PxExtendedVec3 setpos(wp.x, wp.y, wp.z);
	mController->setPosition(setpos);
}
void CharacterControllerComponent::Finish(){

	if (mController){

		auto act = mController->getActor();
		mController->setUserData(NULL);
		act->userData = NULL;
		act->getShapes(&mShape, 1);
		mShape->userData = NULL;

		mController->release();
		mController = NULL;
	}
	mShape = NULL;
}
#ifdef _ENGINE_MODE
void CharacterControllerComponent::EngineUpdate(){

	if (!mController)return;
	auto wp = gameObject->mTransform->WorldPosition();
	PxExtendedVec3 setpos(wp.x, wp.y, wp.z);
	mController->setPosition(setpos);
}
#endif
void CharacterControllerComponent::Update(){

	//if (!mController)return;
	//
	//float time = Game::GetDeltaTime()->GetDeltaTime();
	//
	//PxVec3 disp;
	//{
	//	float speed = 10.0f;
	//	float x = 0, y = 0;
	//	if (Input::Down(KeyCoord::Key_W)){
	//		y = -1.0f;
	//	}
	//	if (Input::Down(KeyCoord::Key_S)){
	//		y = 1.0f;
	//	}
	//	if (Input::Down(KeyCoord::Key_D)){
	//		x = -1.0f;
	//	}
	//	if (Input::Down(KeyCoord::Key_A)){
	//		x = 1.0f;
	//	}
	//
	//	PxControllerState state;
	//	mController->getState(state);
	//	
	//	if (state.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN){
	//		mJump = XMVectorZero();
	//		if (Input::Trigger(KeyCoord::Key_SPACE)){
	//			mJump.y += 3.5f;
	//		}
	//	}
	//	mJump += mGravity * time;
	//
	//	auto xy = XMVector2Normalize(XMVectorSet(x, y, 0, 1)) * speed * time;
	//	auto p = XMVectorZero();
	//	p += xy.y * gameObject->mTransform->Forward();
	//	p += xy.x * gameObject->mTransform->Left();
	//	p += mJump * time;
	//	disp = PxVec3(p.x,p.y,p.z);
	//}
	//
	//PxF32 elapsedTime = Game::GetDeltaTime()->GetDeltaTime();
	//PxF32 minDist = 0.0f;
	//PxControllerFilters filters;
	//PxFilterData  filterData;
	//filterData.word0 = 0;//FilterGroup::eSUBMARINE;  //ワード0 =自分のID 
	//filters.mFilterData = &filterData;
	//filters.mFilterFlags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eRESERVED | PxQueryFlag::eNO_BLOCK | PxQueryFlag::ePOSTFILTER;
	//
	//filters.mFilterCallback = Game::GetPhysX();
	//
	//PxControllerCollisionFlags collisionFlags =
	//	mController->move(disp, minDist, elapsedTime, filters, NULL);
	//

	//
	//auto pos = mController->getFootPosition();
	//auto p = XMVectorSet(pos.x, pos.y, pos.z,1.0f);
	//gameObject->mTransform->WorldPosition(p);

}

#ifdef _ENGINE_MODE
void CharacterControllerComponent::CreateInspector() {


	Inspector ins("CharacterController",this);
	ins.Add("StepOffset", &mStepOffset, [&](float f){
		SetStepOffset(f);
	});
	ins.Add("SlopeLimit", &mSlopeLimitDigree, [&](float f){
		SetSlopeLimit(f);
	});
	ins.Add("Size", &mSize, [&](float f){
		SetSize(f);
	});
	ins.AddEnableButton(this);
	ins.Complete();
}
#endif

void CharacterControllerComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)

	_KEY(mStepOffset);
	_KEY(mSlopeLimitDigree);
	_KEY(mSize);

#undef _KEY
}


void CharacterControllerComponent::Move(const XMVECTOR& velocity){
	if (!mController)return;
	PxVec3 v(velocity.x, velocity.y, velocity.z);
	if (!v.isFinite())return;

	PxF32 elapsedTime = Game::GetDeltaTime()->GetDeltaTime();
	PxF32 minDist = 0.0f;
	PxControllerFilters filters;
	PxFilterData  filterData;
	int layer = gameObject->GetLayer();
	auto l = (Layer::Enum)(1 << layer);
	filterData.word0 = Game::Get()->GetPhysX()->GetLayerCollideFlag(l);
	filters.mFilterData = &filterData;
	filters.mFilterFlags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eRESERVED | PxQueryFlag::eNO_BLOCK | PxQueryFlag::ePOSTFILTER;

	filters.mFilterCallback = Game::GetPhysX();


	PxControllerCollisionFlags collisionFlags =
		mController->move(v, minDist, elapsedTime, filters, NULL);

	//{
	//	PxScene* scene = mController->getScene();
	//	PxSceneReadLock scopedLock(*scene);
	//
	//	PxCapsuleController* capsuleCtrl = static_cast<PxCapsuleController*>(mController);
	//
	//	PxReal r = capsuleCtrl->getRadius();
	//	PxReal dh = 1.0f;
	//	PxCapsuleGeometry geom(r, dh*.5f);
	//
	//	PxExtendedVec3 position = mController->getPosition();
	//	PxVec3 pos((float)position.x, (float)position.y + 1.0f*.5f + r, (float)position.z);
	//	PxQuat orientation(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f));
	//
	//	PxOverlapBuffer hit;
	//	if (scene->overlap(geom, PxTransform(pos, orientation), hit, PxQueryFilterData(PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC))){
	//	}
	//}



	auto pos = mController->getFootPosition();
	auto p = XMVectorSet((FLOAT)pos.x, (FLOAT)pos.y, (FLOAT)pos.z, 1.0f);
	gameObject->mTransform->WorldPosition(p);
}
void CharacterControllerComponent::Teleport(const XMVECTOR& position){
	if (!mController)return;

	PxExtendedVec3 v(position.x, position.y, position.z);

	mController->setPosition(v);


	auto pos = mController->getFootPosition();
	auto p = XMVectorSet((FLOAT)pos.x, (FLOAT)pos.y, (FLOAT)pos.z, 1.0f);
	gameObject->mTransform->WorldPosition(p);
}
bool CharacterControllerComponent::IsGround(){
	if (!mController)return false;
	PxControllerState state;
	mController->getState(state);

	return (state.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN) == 0? false:true;
}


void CharacterControllerComponent::SetSlopeLimit(float degree){
	mSlopeLimitDigree = degree;
	if (!mController)return;
	//歩ける斜面
	float slopeLimit = cosf(XM_PI / 180.0f * mSlopeLimitDigree);
	mController->setSlopeLimit(slopeLimit);
}
void CharacterControllerComponent::SetStepOffset(float offset){
	mStepOffset = offset;
	if (!mController)return;

	//段差を登る高さ
	mController->setStepOffset(mStepOffset);
}

void CharacterControllerComponent::SetSize(float size){
	mSize = size;
	if (!mController)return;
	mController->resize(mSize);
}

void CharacterControllerComponent::SetPhysxLayer(int layer) {
	if (!mShape)return;
	PxFilterData  filterData;
	auto l = (Layer::Enum)(1 << layer);

	filterData.word0 = l;  //ワード0 =自分のID 
	mShape->setSimulationFilterData(filterData);
	mShape->setQueryFilterData(filterData);
}

void CharacterControllerComponent::OnEnabled() {
	mController = Game::GetPhysX()->CreateController();
	auto act = mController->getActor();
	mController->setUserData(gameObject.Get());
	act->userData = gameObject.Get();
	act->getShapes(&mShape, 1);
	mShape->userData = gameObject.Get();

	SetPhysxLayer(gameObject->GetLayer());

	SetSlopeLimit(mSlopeLimitDigree);
	SetStepOffset(mStepOffset);
	SetSize(mSize);

	auto wp = gameObject->mTransform->WorldPosition();
	PxExtendedVec3 setpos(wp.x, wp.y, wp.z);
	mController->setPosition(setpos);
}
void CharacterControllerComponent::OnDisabled() {
	if (mController) {
		auto act = mController->getActor();
		mController->setUserData(NULL);
		act->userData = NULL;
		act->getShapes(&mShape, 1);
		mShape->userData = NULL;

		mController->release();
		mController = NULL;
	}
	mShape = NULL;

}
