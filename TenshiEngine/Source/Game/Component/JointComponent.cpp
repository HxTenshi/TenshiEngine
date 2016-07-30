#include "JointComponent.h"
#include "physx/physx3.h"
#include "Game/Game.h"
#include "Game/Actor.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

JointComponent::JointComponent(){

	mRigidActor[0] = NULL;
	mRigidActor[1] = NULL;
	mJoint = NULL;
}

JointComponent::~JointComponent(){
}
void JointComponent::Initialize(){
	mJoint = Game::GetPhysX()->CreateJoint();
}

void JointComponent::Start(){

}

void JointComponent::Finish(){
	mRigidActor[0] = NULL;
	mRigidActor[1] = NULL;
	if (mJoint){
		mJoint->release();
	}
	mJoint = NULL;
}


void JointComponent::Update(){
}

#ifdef _ENGINE_MODE
void JointComponent::CreateInspector() {

	auto data = Window::CreateInspector();
	Window::ViewInspector("Joint", this, data);
}
#endif

void JointComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)

#undef _KEY
}