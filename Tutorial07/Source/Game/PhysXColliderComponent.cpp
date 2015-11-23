#include "PhysXColliderComponent.h"

#include "PhysXComponent.h"

#include "Game.h"
#include "Actor.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

PhysXColliderComponent::PhysXColliderComponent(){
	mIsSphere = false;
	mShape = NULL;
	mIsShapeAttach = false;
}

PhysXColliderComponent::~PhysXColliderComponent(){

}
void PhysXColliderComponent::Initialize(){
	mIsParentPhysX = false;
}

void PhysXColliderComponent::Start(){
	SearchAttachPhysXComponent();

	if (mMeshFile != ""){
		CreateMesh();
	}
	if (!mShape){
		ChangeShape();
	}

}

void PhysXColliderComponent::Finish(){
	ShapeAttach(NULL);
	mAttachPhysXComponent = NULL;
}

void PhysXColliderComponent::Update(){
	if (!mIsShapeAttach){
		if (!mAttachPhysXComponent)
			if (!SearchAttachPhysXComponent())return;
		ShapeAttach(mShape);
		if (!mIsShapeAttach)return;
	}

	UpdatePose();

	Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
		
		auto mModel = gameObject->GetComponent<ModelComponent>();
		if (!mModel)return;

		if (!mModel->mModel)return;

		auto mMaterial = gameObject->GetComponent<MaterialComponent>();
		if (!mMaterial)return;
		auto pT = mShape->getLocalPose();
		auto Position = XMVectorSet(pT.p.x, pT.p.y, pT.p.z, 1);
		auto Rotate = XMVectorSet(pT.q.x, pT.q.y, pT.q.z, pT.q.w);
		auto Matrix = XMMatrixMultiply(
			XMMatrixMultiply(
			XMMatrixScalingFromVector(gameObject->mTransform->Scale()),
			XMMatrixRotationQuaternion(Rotate)),
			XMMatrixTranslationFromVector(Position));
		mModel->mModel->mWorld = Matrix;
		mModel->Update();

		Model& model = *mModel->mModel;

		model.Draw(mMaterial);
	}));
}

bool PhysXColliderComponent::SearchAttachPhysXComponent(){
	Actor* par = gameObject;
	while (par){
		mAttachPhysXComponent = par->GetComponent<PhysXComponent>();
		if (mAttachPhysXComponent){
			mIsParentPhysX = par != gameObject;
			return true;
		}
		par = par->mTransform->GetParent();
	}
	return false;
}

void PhysXColliderComponent::ShapeAttach(PxShape* shape){
	//アタッチ失敗
	if (!mAttachPhysXComponent){
		mIsShapeAttach = false;
		mShape = shape;
		return;
	}
	//アタッチしていれば
	if (mIsShapeAttach){
		mAttachPhysXComponent->RemoveShape(*mShape);
		mShape->release();
	}
	else{
		if (mShape != shape && mShape){
			mShape->release();
		}
	}

	mShape = shape;
	if (mShape){
		mAttachPhysXComponent->AddShape(*mShape);
		mIsShapeAttach = true;
	}
	else{
		mIsShapeAttach = false;
	}
}
void PhysXColliderComponent::UpdatePose(){


	//auto scale = gameObject->mTransform->Scale();
	//auto mat = gameObject->mTransform->GetMatrix();
	//Actor* act = gameObject;
	//if (mIsParentPhysX){
	//	act = gameObject->mTransform->GetParent();
	//}
	//
	//auto mat = act->mTransform->GetMatrix();
	//mat._14 = 0.0f;
	//mat._24 = 0.0f;
	//mat._34 = 0.0f;
	//
	//XMVECTOR scale = XMVector3Transform(gameObject->mTransform->Scale(), mat);
	//XMVECTOR scale = XMVectorSet(mat._11, mat._22, mat._33, 1);
	XMVECTOR scale = XMVectorSet(1,1,1,1);
	if (mIsParentPhysX){
		//scale = XMVectorMultiply(scale, gameObject->mTransform->GetParent()->mTransform->Scale());
		auto mat = gameObject->mTransform->GetParent()->mTransform->GetMatrix();
		auto x = XMVector3Length(mat.r[0]);
		auto y = XMVector3Length(mat.r[1]);
		auto z = XMVector3Length(mat.r[2]);
		scale = XMVectorSet(x.x, y.x, z.x, 1);


	}

	auto pos = gameObject->mTransform->Position();
	auto transform = mShape->getLocalPose();
	if (!mIsParentPhysX){
		transform.p = PxVec3(0, 0, 0);
	}
	else{
		transform.p = PxVec3(pos.x*scale.x, pos.y*scale.y, pos.z*scale.z);
	}
	mShape->setLocalPose(transform);

	scale = XMVectorMultiply(scale, gameObject->mTransform->Scale());


	static XMVECTOR gscale = XMVectorZero();
	if (gscale.x == scale.x&&gscale.y == scale.y&&gscale.z == scale.z)return;
	gscale = scale;
	//PxBoxGeometry box(PxVec3(1.0f*scale.x, 1.0f*scale.y, 1.0f*scale.z));
	auto g = mShape->getGeometry();
	if (g.getType() == PxGeometryType::eBOX){
		g.box().halfExtents = PxVec3(PxVec3(abs(0.5f*scale.x), abs(0.5f*scale.y), abs(0.5f*scale.z)));
		mShape->setGeometry(g.box());
	}
	if (g.getType() == PxGeometryType::eSPHERE){
		g.sphere().radius = (abs(0.5f*scale.x) + abs(0.5f*scale.y) + abs(0.5f*scale.z))/3.0f;
		mShape->setGeometry(g.sphere());
	}

}


void PhysXColliderComponent::ChangeShape(){
	PxShape* shape = NULL;
	if (mIsSphere){
		shape = Game::GetPhysX()->CreateShapeSphere();
	}
	else{
		shape = Game::GetPhysX()->CreateShape();
	}

	ShapeAttach(shape);
}

void PhysXColliderComponent::CreateMesh(){

	AssetLoader loader;
	auto data = loader.LoadAsset(mMeshFile);
	if (!data)return;
	auto shape = Game::GetPhysX()->CreateTriangleMesh(data->m_Polygons.Vertexs, data->m_Polygons.Indices);

	ShapeAttach(shape);

}
physx::PxShape* PhysXColliderComponent::GetShape(){
	return mShape;
}

void PhysXColliderComponent::CreateInspector() {
	auto data = Window::CreateInspector();
	BoolCollback collback = [&](bool value){
		mIsSphere = value;
		ChangeShape();
	};
	std::function<void(std::string)> collbackpath = [&](std::string name){
		mMeshFile = name;
		CreateMesh();
	};
	Window::AddInspector(new InspectorStringDataSet("Mesh", &mMeshFile, collbackpath), data);
	Window::AddInspector(new InspectorBoolDataSet("IsSphere", &mIsSphere, collback), data);
	Window::ViewInspector("Collider",data);
}

void PhysXColliderComponent::ExportData(File& f) {
	ExportClassName(f);
}
void PhysXColliderComponent::ImportData(File& f) {

}
