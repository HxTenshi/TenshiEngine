#include "PhysXComponent.h"

#include "Game.h"
PhysXComponent::PhysXComponent(){

	mChengeTransformFlag = 0xFFFF;
	mRigidActor = Game::CreateRigitBody();
	auto r = (PxRigidDynamic*)mRigidActor;
	auto flag = r->getRigidDynamicFlags();
	mIsKinematic = flag & PxRigidDynamicFlag::eKINEMATIC;

}

PhysXComponent::~PhysXComponent(){
	Game::RemovePhysXActor(mRigidActor);
}
void PhysXComponent::Initialize(){
	auto t = mRigidActor->getGlobalPose();
	auto position = gameObject->mTransform->Position();
	t.p = physx::PxVec3(position.x, position.y, position.z);
	mRigidActor->setGlobalPose(t);

	SetKinematic(mIsKinematic);
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
	mRigidActor->detachShape(shape);
}

void PhysXComponent::CreateInspector() {

	std::function<void(bool)> collback = [&](bool value){
		SetKinematic(value);
	};

	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorLabelDataSet("PhysX"), data);
	Window::AddInspector(new InspectorBoolDataSet("Kinematic", &mIsKinematic, collback), data);
	Window::ViewInspector(data);
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

void PhysXComponent::AddForce(XMVECTOR& force){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addForce(PxVec3(force.x, force.y, force.z));
}
void PhysXComponent::AddTorque(XMVECTOR& force){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addTorque(PxVec3(force.x, force.y, force.z));
}