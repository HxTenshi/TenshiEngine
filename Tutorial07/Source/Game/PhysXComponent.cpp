#include "PhysXComponent.h"

#include "Game.h"
PhysXComponent::PhysXComponent(){

	mChengeTransform = true;
	mRigidActor = Game::CreateRigitBody();
}

PhysXComponent::~PhysXComponent(){
	Game::RemovePhysXActor(mRigidActor);
}
void PhysXComponent::Initialize(){
	auto t = mRigidActor->getGlobalPose();
	auto position = gameObject->mTransform->Position();
	t.p = physx::PxVec3(position.x, position.y, position.z);
	mRigidActor->setGlobalPose(t);

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

void PhysXComponent::CreateInspector() {
	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorLabelDataSet("PhysX"), data);
	Window::ViewInspector(data);
}

void PhysXComponent::ExportData(File& f) {
	ExportClassName(f);
}
void PhysXComponent::ImportData(File& f) {

}

void PhysXComponent::AddForce(XMVECTOR& force){
	PxRigidDynamic* a = (PxRigidDynamic*)mRigidActor;
	a->addForce(PxVec3(force.x, force.y, force.z));
}