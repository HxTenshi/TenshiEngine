#include "PhysXComponent.h"
#include "PhysXColliderComponent.h"

#include "physx/physx3.h"

#include "Game/Game.h"

#include "Engine/Inspector.h"

PhysXComponent::PhysXComponent(){
	mIsEngineMode = false;
	mIsKinematic = false;
	mIsGravity = true;
	mRigidActor = NULL;
}

PhysXComponent::~PhysXComponent(){
}
void PhysXComponent::Initialize(){
	mChengeTransformFlag = (char)PhysXChangeTransformFlag::Position | (char)PhysXChangeTransformFlag::Rotate;
	if (mIsEngineMode){
		mRigidActor = Game::CreateRigitBodyEngine();
	}
	else{
		mRigidActor = Game::CreateRigitBody();
	}

	if (mIsEngineMode){
		Game::GetPhysX()->AddActorEngine(mRigidActor);
	}
	else{
		Game::GetPhysX()->AddActor(mRigidActor);
	}

	if (!IsEnabled()){
		mRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	}

	SetKinematic(mIsKinematic);
	SetGravity(mIsGravity);

	if(gameObject)
	mRigidActor->userData = gameObject.Get();
}


void PhysXComponent::Start(){
	if (!mRigidActor || !gameObject)return;
	auto t = mRigidActor->getGlobalPose();
	auto position = gameObject->mTransform->WorldPosition();
	t.p = physx::PxVec3(position.x, position.y, position.z);
	
	//auto rotate = gameObject->mTransform->Rotate();
	//rotate = XMQuaternionRotationRollPitchYawFromVector(rotate);
	//t.q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);
	//if (mIsKinematic){
	//	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	//	a->setKinematicTarget(t);
	//	if (!Game::IsGamePlay()){
	//		mRigidActor->setGlobalPose(t);
	//	}
	//}
	//else{
	mRigidActor->setGlobalPose(t);
	//}
	{
		auto col = gameObject->GetComponent<PhysXColliderComponent>();
		if (col){
			col->AttachPhysxComponent(shared_from_this());
		}
	}
	ChildrenAttachShape(gameObject.Get());
}

void PhysXComponent::ChildrenAttachShape(Actor* actor){

	for (auto act : actor->mTransform->Children()){
		auto com = act->GetComponent<PhysXComponent>();
		if (com)continue;

		auto col = act->GetComponent<PhysXColliderComponent>();
		if (col){
			col->AttachPhysxComponent(shared_from_this());
		}

		ChildrenAttachShape(act.Get());
	}

}
void PhysXComponent::Finish(){
	if (mRigidActor || !gameObject){

		weak_ptr<PhysXComponent> physxCom = NULL;
		{
			auto par = gameObject->mTransform->GetParent();
			while (par){
				auto com = par->GetComponent<PhysXComponent>();
				if (com){
					physxCom = com;
					break;
				}
				par = par->mTransform->GetParent();
			}
		}

		//シェイプのアタッチを解放
		auto num = (int)mRigidActor->getNbShapes();
		for (int i = 0; i < num; i++){
			PxShape* temp = NULL;
			mRigidActor->getShapes(&temp,1);
			if (!temp)continue;
			auto act = (Actor*)temp->userData;
			if (!act)continue;
			auto com = act->GetComponent<PhysXColliderComponent>();
			if (!com)continue;
			com->AttachPhysxComponent(physxCom);
			
		}

		mRigidActor->userData = NULL;
		
		if (mIsEngineMode){
			Game::RemovePhysXActorEngine(mRigidActor);
		}
		else{
			Game::RemovePhysXActor(mRigidActor);
		}
		
		mRigidActor->release();
	}
	mRigidActor = NULL;
}

#include "Game/Actor.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

void PhysXComponent::Update(){
	//if (mChengeTransform){
	//	auto t = mRigidActor->getGlobalPose();
	//	auto position = gameObject->mTransform->Position();
	//	t.p = physx::PxVec3(position.x, position.y, position.z);
	//
	//	auto rotate = gameObject->mTransform->Rotate();
	//	rotate = XMQuaternionRotationRollPitchYawFromVector(rotate);
	//	t.q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);
	//	mRigidActor->setGlobalPose(t);
	//
	//	mChengeTransform = false;
	//}
}

void PhysXComponent::SetTransform(bool RebirthSet){
	if (!mRigidActor || !gameObject)return;
	auto t = mRigidActor->getGlobalPose();

	if (mChengeTransformFlag&(char)PhysXChangeTransformFlag::Position){
		auto position = gameObject->mTransform->WorldPosition();
		t.p = physx::PxVec3(position.x, position.y, position.z);
	}
	else if (RebirthSet){
		XMVECTOR pos = XMVectorSet(t.p.x, t.p.y, t.p.z, 1);
		gameObject->mTransform->WorldPosition(pos);
	}

	if (mChengeTransformFlag&(char)PhysXChangeTransformFlag::Rotate){
		auto qua = gameObject->mTransform->WorldQuaternion();
		t.q = physx::PxQuat(qua.x, qua.y, qua.z, qua.w);
	}
	else if (RebirthSet){
		auto q = XMVectorSet(t.q.x, t.q.y, t.q.z, t.q.w);
		gameObject->mTransform->WorldQuaternion(q);
		
	}
	if (mChengeTransformFlag){
		if (mIsKinematic){
			PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
			a->setKinematicTarget(t);
			//if (!Game::IsGamePlay() && !mIsEngineMode){
				mRigidActor->setGlobalPose(t);
			//}
		}
		else{
			mRigidActor->setGlobalPose(t);
		}
	}
	
	//gameObject->SetTransform(&t);
	{
		//physx::PxTransform& pT = t;
		//XMVECTOR pos = XMVectorSet(pT.p.x, pT.p.y, pT.p.z, 1);
		//gameObject->mTransform->WorldPosition(pos);
		//
		//auto q = XMVectorSet(pT.q.x, pT.q.y, pT.q.z, pT.q.w);
		//gameObject->mTransform->Quaternion(q);
	}

	mChengeTransformFlag = 0x0000;
}


bool PhysXComponent::AddShape(physx::PxShape& shape){
	if (!mRigidActor)return false;
	mRigidActor->attachShape(shape);
	return true;
}
void PhysXComponent::RemoveShape(physx::PxShape& shape){
	if (mRigidActor)
		mRigidActor->detachShape(shape);
}

#ifdef _ENGINE_MODE
void PhysXComponent::CreateInspector() {

	std::function<void(bool)> collback = [&](bool value){
		SetKinematic(value);
	};

	Inspector ins("PhysX",this);
	ins.AddEnableButton(this);
	ins.Add("Kinematic", &mIsKinematic, collback);
	ins.Add("UseGravity", &mIsGravity, [&](bool value){
		SetGravity(value);
	});
	ins.Complete();
}
#endif

void PhysXComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(mIsKinematic);
	_KEY(mIsGravity);

#undef _KEY
}

void PhysXComponent::SetKinematic(bool flag){
	mIsKinematic = flag;
	if (mRigidActor){
		auto r = (PxRigidDynamic*)mRigidActor;
		r->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, mIsKinematic);
	}
}

void PhysXComponent::SetGravity(bool flag){
	mIsGravity = flag;
	if (mRigidActor){
		auto r = (PxRigidDynamic*)mRigidActor;
		r->setActorFlag(PxActorFlag::eDISABLE_GRAVITY,!mIsGravity);
	}
}


XMVECTOR PhysXComponent::GetForceVelocity(){
	if (!mRigidActor)return XMVectorZero();
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	auto v = a->getLinearVelocity();
	return XMVectorSet(v.x, v.y, v.z, 1);
}

void PhysXComponent::SetForceVelocity(const XMVECTOR& v){
	if (!mRigidActor)return;
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->setLinearVelocity(PxVec3(v.x, v.y, v.z));
}

void PhysXComponent::AddForce(const XMVECTOR& force, ForceMode::Enum forceMode){
	if (!mRigidActor)return;
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addForce(PxVec3(force.x, force.y, force.z),(physx::PxForceMode::Enum)forceMode);
}

void PhysXComponent::AddTorque(const XMVECTOR& force, ForceMode::Enum forceMode){
	if (!mRigidActor)return;
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addTorque(PxVec3(force.x, force.y, force.z), (physx::PxForceMode::Enum)forceMode);
}


void PhysXComponent::OnEnabled(){
	if (!mRigidActor)return;

	mRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	//if (mIsEngineMode){
	//	Game::GetPhysX()->AddActorEngine(mRigidActor);
	//}
	//else{
	//	Game::GetPhysX()->AddActor(mRigidActor);
	//}
	//if(mIsKinematic)
	//	SetKinematic(mIsKinematic);
}
void PhysXComponent::OnDisabled(){
	if (!mRigidActor)return;

	mRigidActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION,true);
	//auto r = (PxRigidDynamic*)mRigidActor;
	//r->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);
	//if (mIsEngineMode){
	//	Game::GetPhysX()->RemoveActorEngine(mRigidActor);
	//}
	//else{
	//	Game::GetPhysX()->RemoveActor(mRigidActor);
	//}
}