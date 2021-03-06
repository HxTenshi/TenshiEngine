#include "JointComponent.h"
#include "physx/physx3.h"
#include "Game/Game.h"
#include "Game/Actor.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "Engine/Inspector.h"

JointComponent::JointComponent(){
	mActorUID[0] = "";
	mActorUID[1] = "";
	mRigidActor[0] = NULL;
	mRigidActor[1] = NULL;
	mJoint = NULL;
}

JointComponent::~JointComponent(){
}
void JointComponent::Initialize(){
	mJoint = Game::GetPhysX()->CreateDistanceJoint();
	//mJoint->setLimit(PxJointAngularLimitPair(-PxPi / 4, PxPi / 4, 0.01f));
	//mJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
	//mJoint->setLocalPose(PxJointActorIndex::eACTOR0, PxTransform(PxVec3(2.0f, 0.0f, 0.0f), PxQuat(0, 0, 0, 1)));

	mJoint->setMaxDistance(3.0f);
	mJoint->setDistanceJointFlag (PxDistanceJointFlag::eMAX_DISTANCE_ENABLED,true);
	mJoint->setLocalPose(PxJointActorIndex::eACTOR0, PxTransform(PxVec3(0.5f, 0.5f, 0.5f), PxQuat(0, 0, 0, 1)));
	mJoint->setDamping(0.0f);
}	

void JointComponent::Start(){

	if (!mActorUID[0].IsNull()){
		auto act = Game::FindUID(mActorUID[0]);
		SetRigidActor(RigitActorID::Actor_0, act);
	}
	if (!mActorUID[1].IsNull()){
		auto act = Game::FindUID(mActorUID[1]);
		SetRigidActor(RigitActorID::Actor_1, act);
	}
	if(mGameObject){

		SetRigidActor(RigitActorID::Actor_1, mGameObject);
	}
}

void JointComponent::Finish(){
	mRigidActor[0] = NULL;
	mRigidActor[1] = NULL;
	mActorUID[0] = "";
	mActorUID[1] = "";
	if (mJoint){
		mJoint->release();
	}
	mJoint = NULL;
}


void JointComponent::Update(){
}

#ifdef _ENGINE_MODE
void JointComponent::CreateInspector() {

	


	Inspector ins("Joint",this);
	ins.AddEnableButton(this);

	static std::string actor0;
	ins.Add("RigidActor_0", &actor0, [&](std::string f){
		actor0 = f;
		auto act = Game::FindNameActor(f.c_str());
		mActorUID[0] = "";
		if (act){
			mActorUID[0] = act->GetUniqueID();
		}

		SetRigidActor(RigitActorID::Actor_0, act);
	});
	static std::string actor1;
	ins.Add("RigidActor_1", &actor1, [&](std::string f){
		actor1 = f;
		auto act = Game::FindNameActor(f.c_str());
		mActorUID[1] = "";
		if (act){
			mActorUID[1] = act->GetUniqueID();
		}

		SetRigidActor(RigitActorID::Actor_1, act);
	});

	ins.Add("GameObject", &mGameObject, [&]() {
		SetRigidActor(RigitActorID::Actor_1, mGameObject);
	});

	ins.Complete();
}
#endif

void JointComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
#define _KEY_GO(x) ioGameObjectHelper::func(&x, #x,io,&gameObject)

	_KEY(mActorUID[0]);
	_KEY(mActorUID[1]);
	_KEY_GO(mGameObject);

#undef _KEY
}


void JointComponent::SetRigidActor(RigitActorID id, GameObject actor){
	int ID = (int)id;
	if (ID != 0 && ID != 1){
		return;
	}

	mRigidActor[ID] = NULL;
	if (actor){
		auto physx = actor->GetComponent<PhysXComponent>();
		if (physx){
			auto rigidAct = physx->GetRigidActor();
			if (rigidAct){
				mRigidActor[ID] = rigidAct;
			}
		}
	}
	mJoint->setActors(mRigidActor[0], mRigidActor[1]);
}