#include "PhysXComponent.h"

#include "Game.h"
PhysXComponent::PhysXComponent(){
	mIsEngineMode = false;
	mIsKinematic = false;
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
	auto position = gameObject->mTransform->Position();
	t.p = physx::PxVec3(position.x, position.y, position.z);
	mRigidActor->setGlobalPose(t);
}
void PhysXComponent::Finish(){
	if (mRigidActor){
		if (mIsEngineMode){
			Game::RemovePhysXActorEngine(mRigidActor);
		}
		else{
			Game::RemovePhysXActor(mRigidActor);
		}
	}
	mRigidActor = NULL;
}

#include "Actor.h"
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

void PhysXComponent::SetTransform(){

	auto t = mRigidActor->getGlobalPose();

	if (mChengeTransformFlag&(char)PhysXChangeTransformFlag::Position){
		auto position = gameObject->mTransform->Position();
		t.p = physx::PxVec3(position.x, position.y, position.z);
	}

	if (mChengeTransformFlag&(char)PhysXChangeTransformFlag::Rotate){
		auto rotate = gameObject->mTransform->Rotate();
		//rotate = XMQuaternionRotationRollPitchYawFromVector(rotate);
		t.q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);
	}
	if (mChengeTransformFlag){
		mRigidActor->setGlobalPose(t);
	}
	gameObject->SetTransform(&t);
	mChengeTransformFlag = 0x0000;
}
void PhysXComponent::AddShape(physx::PxShape& shape){
	mRigidActor->attachShape(shape);
}
void PhysXComponent::RemoveShape(physx::PxShape& shape){
	if (mRigidActor)
		mRigidActor->detachShape(shape);
}

void PhysXComponent::CopyData(Component* post, Component* base){
	auto Post = (PhysXComponent*)post;
	auto Base = (PhysXComponent*)base;

	Post->mIsKinematic = Base->mIsKinematic;
}

void PhysXComponent::CreateInspector() {

	std::function<void(bool)> collback = [&](bool value){
		SetKinematic(value);
	};

	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorBoolDataSet("Kinematic", &mIsKinematic, collback), data);
	Window::ViewInspector("PhysX",data);
}

void PhysXComponent::ExportData(File& f) {
	ExportClassName(f);
	f.Out(mIsKinematic);
}
void PhysXComponent::ImportData(File& f) {

	f.In(&mIsKinematic);
}

void PhysXComponent::SetKinematic(bool flag){
	mIsKinematic = flag;
	auto r = (PxRigidDynamic*)mRigidActor;
	r->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, mIsKinematic);
}

void PhysXComponent::AddForce(const XMVECTOR& force){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addForce(PxVec3(force.x, force.y, force.z));
}
void PhysXComponent::AddTorque(const XMVECTOR& force){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addTorque(PxVec3(force.x, force.y, force.z));
}