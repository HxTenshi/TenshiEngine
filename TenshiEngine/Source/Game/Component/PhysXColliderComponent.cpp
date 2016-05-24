#include "PhysXColliderComponent.h"

#include "PhysXComponent.h"

#include "TransformComponent.h"

#include "Game/Game.h"
#include "Game/Actor.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "Engine/AssetDataBase.h"

#include "Engine/AssetFile/Mesh/MeshFileData.h"
#include "Engine/AssetFile/Physx/PhysxMaterialFileData.h"

PhysXColliderComponent::PhysXColliderComponent(){
	mIsSphere = false;
	mShape = NULL;
	mIsShapeAttach = false;
	mIsTrigger = false;
}

PhysXColliderComponent::~PhysXColliderComponent(){

}
void PhysXColliderComponent::Initialize(){
	mIsParentPhysX = false;

	if (!mShape){
		if (mMeshFile != ""){
			CreateMesh();
		}
		if (!mShape){
			ChangeShape();
		}
	}
	if (mPhysicsMaterialFile != ""){
		ChangeMaterial();
	}
}

void PhysXColliderComponent::Start(){
	SearchAttachPhysXComponent();

	if (mMeshFile != ""){
		CreateMesh();
	}
	else{
		ChangeShape();
	}
	mScale = XMVectorZero();
	SetIsTrigger(mIsTrigger);
}

void PhysXColliderComponent::Finish(){
	ShapeAttach(NULL);
	mAttachPhysXComponent = NULL;
}

void PhysXColliderComponent::EngineUpdate(){
	Update();
}
void PhysXColliderComponent::Update(){
	if (!mIsShapeAttach){
		if (!mAttachPhysXComponent)
			if (!SearchAttachPhysXComponent())return;
		ShapeAttach(mShape);
		if (!mIsShapeAttach)return;
	}

	UpdatePose();



	//Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
	//	
	//	auto mModel = gameObject->GetComponent<ModelComponent>();
	//	if (!mModel)return;
	//
	//	if (!mModel->mModel)return;
	//
	//	auto mMaterial = gameObject->GetComponent<MaterialComponent>();
	//	if (!mMaterial)return;
	//	auto pT = mShape->getLocalPose();
	//	auto Position = XMVectorSet(pT.p.x, pT.p.y, pT.p.z, 1);
	//	auto Rotate = XMVectorSet(pT.q.x, pT.q.y, pT.q.z, pT.q.w);
	//	auto Matrix = XMMatrixMultiply(
	//		XMMatrixMultiply(
	//		XMMatrixScalingFromVector(gameObject->mTransform->Scale()),
	//		XMMatrixRotationQuaternion(Rotate)),
	//		XMMatrixTranslationFromVector(Position));
	//	mModel->mModel->mWorld = Matrix;
	//	mModel->Update();
	//
	//	Model& model = *mModel->mModel;
	//
	//	model.Draw(mMaterial);
	//}));
}

bool PhysXColliderComponent::SearchAttachPhysXComponent(){
	Actor* par = gameObject;
	while (par){
		mAttachPhysXComponent = par->GetComponent<PhysXComponent>();
		if (mAttachPhysXComponent){
			mIsParentPhysX = (par != gameObject)?true:false;
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
		if (mShape != shape && mShape){
			mShape->release();
		}
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


	//bool isKinem = false;
	//Actor* par = gameObject;
	//while (par){
	//	auto com = par->GetComponent<PhysXComponent>();
	//	if (com){
	//		isKinem = com->GetKinematic();
	//		break;
	//	}
	//	par = par->mTransform->GetParent();
	//}
	//
	//
	//if (!par){
	//	return;
	//}
	//
	////if (!isKinem)return;
	//XMVECTOR v;
	//auto physxW2L = XMMatrixInverse(&v,par->mTransform->GetMatrix());
	//
	//auto mat = gameObject->mTransform->GetMatrix();
	//auto localmat = XMMatrixMultiply(physxW2L, mat);
	//
	//auto x = XMVector3Length(localmat.r[0]);
	//auto y = XMVector3Length(localmat.r[1]);
	//auto z = XMVector3Length(localmat.r[2]);
	//auto scale = XMVectorSet(x.x, y.x, z.x, 1);
	//auto pos = localmat.r[3];
	//
	//
	//auto transform = mShape->getLocalPose();
	//
	//transform.p = PxVec3(pos.x,pos.y, pos.z);
	//
	//mShape->setLocalPose(transform);
	//
	//
	//
	//if (mScale.x == scale.x&&mScale.y == scale.y&&mScale.z == scale.z)return;
	//mScale = scale;
	////PxBoxGeometry box(PxVec3(1.0f*scale.x, 1.0f*scale.y, 1.0f*scale.z));
	//auto g = mShape->getGeometry();
	//if (g.getType() == PxGeometryType::eBOX){
	//	g.box().halfExtents = PxVec3(PxVec3(abs(0.5f*scale.x), abs(0.5f*scale.y), abs(0.5f*scale.z)));
	//	mShape->setGeometry(g.box());
	//}
	//else if (g.getType() == PxGeometryType::eSPHERE){
	//	g.sphere().radius = (abs(0.5f*scale.x) + abs(0.5f*scale.y) + abs(0.5f*scale.z)) / 3.0f;
	//	mShape->setGeometry(g.sphere());
	//}
	//else if (g.getType() == PxGeometryType::eTRIANGLEMESH){
	//
	//	auto rotate = gameObject->mTransform->Quaternion();
	//	auto q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);
	//
	//	g.triangleMesh().scale = PxMeshScale(PxVec3(scale.x, scale.y, scale.z), q);
	//	mShape->setGeometry(g.triangleMesh());
	//}

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
	if(mIsParentPhysX){
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
	
	if (mScale.x == scale.x&&mScale.y == scale.y&&mScale.z == scale.z)return;
	mScale = scale;
	//PxBoxGeometry box(PxVec3(1.0f*scale.x, 1.0f*scale.y, 1.0f*scale.z));
	auto g = mShape->getGeometry();
	if (g.getType() == PxGeometryType::eBOX){
		g.box().halfExtents = PxVec3(PxVec3(abs(0.5f*scale.x), abs(0.5f*scale.y), abs(0.5f*scale.z)));
		mShape->setGeometry(g.box());
	}
	else if (g.getType() == PxGeometryType::eSPHERE){
		g.sphere().radius = (abs(0.5f*scale.x) + abs(0.5f*scale.y) + abs(0.5f*scale.z)) / 3.0f;
		mShape->setGeometry(g.sphere());
	}
	else if (g.getType() == PxGeometryType::eTRIANGLEMESH){

		auto rotate = gameObject->mTransform->Quaternion();
		auto q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);

		g.triangleMesh().scale = PxMeshScale(PxVec3(scale.x, scale.y, scale.z), q);
		mShape->setGeometry(g.triangleMesh());
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
void PhysXColliderComponent::ChangeMaterial(){
	
	if (mPhysicsMaterialFile == "")return;
	PhysxMaterialAssetDataPtr mate;
	AssetDataBase::Instance(mPhysicsMaterialFile.c_str(), mate);
	if (!mate->GetFileData())return;
	auto pxmate = mate->GetFileData()->GetMaterial();
	if (!pxmate)return;
	mShape->setMaterials(&pxmate, 1);
}

void PhysXColliderComponent::CreateMesh(){

	MeshAssetDataPtr data;
	AssetDataBase::Instance(mMeshFile.c_str(),data);
	if (!data)return;
	auto shape = Game::GetPhysX()->CreateTriangleMesh(data->GetFileData()->GetPolygonsData());

	ShapeAttach(shape);
}

void PhysXColliderComponent::SetIsTrigger(bool flag){
	mIsTrigger = flag;
	//PxShapeFlag::eTRIGGER_SHAPE または　PxShapeFlag::eSIMULATION_SHAPE 

	PxShapeFlags flags = mShape->getFlags();

	//一旦オンにして
	flags |= PxShapeFlag::eTRIGGER_SHAPE;
	flags |= PxShapeFlag::eSIMULATION_SHAPE;
	//不要な方を反転
	if (mIsTrigger){
		flags ^= PxShapeFlag::eSIMULATION_SHAPE;
	}
	else{
		flags ^= PxShapeFlag::eTRIGGER_SHAPE;
	}

	mShape->setFlags(flags);
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
	BoolCollback collbacktri = [&](bool value){
		//mIsTrigger = value;
		SetIsTrigger(value);
	};
	std::function<void(std::string)> collbackmatepath = [&](std::string name){
		mPhysicsMaterialFile = name;
		ChangeMaterial();
	};

	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Mesh", &mMeshFile, collbackpath), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("IsSphere", &mIsSphere, collback), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("IsTrigger", &mIsTrigger, collbacktri), data);
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("PhysxMaterial", &mPhysicsMaterialFile, collbackmatepath), data);
	Window::ViewInspector("Collider", this, data);
}

void PhysXColliderComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mMeshFile);
	_KEY(mIsSphere);
	_KEY(mIsTrigger);
	_KEY(mPhysicsMaterialFile);

#undef _KEY
}