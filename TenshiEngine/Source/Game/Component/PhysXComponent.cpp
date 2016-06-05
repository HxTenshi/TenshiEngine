#include "PhysXComponent.h"

#include "Game/Game.h"
PhysXComponent::PhysXComponent(){
	mIsEngineMode = false;
	mIsKinematic = false;
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

	SetKinematic(mIsKinematic);

	mRigidActor->userData = gameObject;
}


void PhysXComponent::Start(){
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
}
void PhysXComponent::Finish(){
	if (mRigidActor){
		mRigidActor->userData = NULL;
		if (mIsEngineMode){
			Game::RemovePhysXActorEngine(mRigidActor);
		}
		else{
			Game::RemovePhysXActor(mRigidActor);
		}
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
		auto qua = gameObject->mTransform->Quaternion();
		t.q = physx::PxQuat(qua.x, qua.y, qua.z, qua.w);
	}
	else if (RebirthSet){
		auto q = XMVectorSet(t.q.x, t.q.y, t.q.z, t.q.w);
		gameObject->mTransform->Quaternion(q);
		
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


void PhysXComponent::AddShape(physx::PxShape& shape){
	mRigidActor->attachShape(shape);
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

	auto data = Window::CreateInspector();
	Window::AddInspector(new TemplateInspectorDataSet<bool>("Kinematic", &mIsKinematic, collback), data);
	Window::ViewInspector("PhysX", this, data);
}
#endif

void PhysXComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mIsKinematic);

#undef _KEY
}

void PhysXComponent::SetKinematic(bool flag){
	mIsKinematic = flag;
	if (mRigidActor){
		auto r = (PxRigidDynamic*)mRigidActor;
		r->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, mIsKinematic);
	}
}

XMVECTOR PhysXComponent::GetForceVelocity(){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	auto v = a->getLinearVelocity();
	return XMVectorSet(v.x, v.y, v.z, 1);
}

void PhysXComponent::SetForceVelocity(const XMVECTOR& v){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->setLinearVelocity(PxVec3(v.x, v.y, v.z));
}

void PhysXComponent::AddForce(const XMVECTOR& force, ForceMode::Enum forceMode){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addForce(PxVec3(force.x, force.y, force.z),(physx::PxForceMode::Enum)forceMode);
}

void PhysXComponent::AddTorque(const XMVECTOR& force, ForceMode::Enum forceMode){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addTorque(PxVec3(force.x, force.y, force.z), (physx::PxForceMode::Enum)forceMode);
}