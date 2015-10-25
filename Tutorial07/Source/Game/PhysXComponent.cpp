#include "PhysXComponent.h"

#include "Game.h"
PhysXComponent::PhysXComponent(){

	mChengeTransform = true;
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


void PhysXComponent::SetPosition(XMVECTOR& position){
	auto t = mRigidActor->getGlobalPose();
	t.p = physx::PxVec3(position.x, position.y, position.z);
	mRigidActor->setGlobalPose(t);
}
void PhysXComponent::Update(){
	if (mChengeTransform){
		auto t = mRigidActor->getGlobalPose();
		auto position = gameObject->mTransform->Position();
		t.p = physx::PxVec3(position.x, position.y, position.z);
		mRigidActor->setGlobalPose(t);

		mChengeTransform = false;
	}
	auto t = mRigidActor->getGlobalPose();
	gameObject->SetTransform(&t);
	mChengeTransform = false;
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