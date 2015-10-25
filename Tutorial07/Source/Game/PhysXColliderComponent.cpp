#include "PhysXColliderComponent.h"

#include "PhysXComponent.h"

#include "Game.h"
#include "Actor.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

PhysXColliderComponent::PhysXColliderComponent(){
	mIsParentPhysX = false;
	mShape = Game::CreateShape();
}

PhysXColliderComponent::~PhysXColliderComponent(){
	if (mAttachPhysXComponent)
		mAttachPhysXComponent->RemoveShape(*mShape);
}
void PhysXColliderComponent::Initialize(){

	Actor* par = gameObject;
	while (par){
		mAttachPhysXComponent = par->GetComponent<PhysXComponent>();
		if (mAttachPhysXComponent){
			mAttachPhysXComponent->AddShape(*mShape);
			mIsParentPhysX = par != gameObject;
			break;
		}
		par = par->mTransform->GetParent();
	}
}

void PhysXColliderComponent::Update(){
	if (!mShape)return;
	if (!mAttachPhysXComponent)Initialize();
	UpdatePose();
}


void PhysXColliderComponent::UpdatePose(){


	//auto scale = gameObject->mTransform->Scale();
	auto mat = gameObject->mTransform->GetMatrix();
	XMVECTOR scale = XMVectorSet(mat._11, mat._22, mat._33, 1);

	auto pos = gameObject->mTransform->Position();
	auto transform = mShape->getLocalPose();
	if (!mIsParentPhysX){
		transform.p = PxVec3(0, 0, 0);
	}
	else{
		transform.p = PxVec3(pos.x*scale.x, pos.y*scale.y, pos.z*scale.z);
	}
	mShape->setLocalPose(transform);


	static XMVECTOR gscale = XMVectorZero();
	if (gscale.x == scale.x&&gscale.y == scale.y&&gscale.z == scale.z)return;
	gscale = scale;
	//PxBoxGeometry box(PxVec3(1.0f*scale.x, 1.0f*scale.y, 1.0f*scale.z));
	PxBoxGeometry box;
	mShape->getBoxGeometry(box);
	box.halfExtents = PxVec3(PxVec3(1.0f*scale.x, 1.0f*scale.y, 1.0f*scale.z));
	mShape->setGeometry(box);
}

physx::PxShape* PhysXColliderComponent::GetShape(){
	return mShape;
}

void PhysXColliderComponent::CreateInspector() {
	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorLabelDataSet("Collider"), data);
	Window::ViewInspector(data);
}

void PhysXColliderComponent::ExportData(File& f) {
	ExportClassName(f);
}
void PhysXColliderComponent::ImportData(File& f) {

}
