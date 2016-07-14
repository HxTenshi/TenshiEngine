#include "PhysXColliderComponent.h"

#include "PhysXComponent.h"

#include "TransformComponent.h"

#include "Game/Game.h"
#include "Game/Actor.h"
#include "physx/physx3.h"

#include "Engine/AssetDataBase.h"

#include "Engine/AssetFile/Mesh/MeshFileData.h"
#include "Engine/AssetFile/Physx/PhysxMaterialFileData.h"

PhysXColliderComponent::PhysXColliderComponent(){
	mIsSphere = false;
	mShape = NULL;
	mIsShapeAttach = false;
	mIsTrigger = false;
	mPosition = XMVectorSet(0, 0, 0, 1);
	mScale = XMVectorSet(1, 1, 1, 1);
	mPhysicsMaterial = NULL;
}

PhysXColliderComponent::~PhysXColliderComponent(){

}
void PhysXColliderComponent::Initialize(){
	mIsParentPhysX = false;

	if (!mShape){
		CreateMesh(mMeshFile);
		
		if (!mShape){
			ChangeShape(mIsSphere);
		}
	}
	ChangeMaterial(mPhysicsMaterialFile);

	SetTransform(mPosition);
	SetScale(mScale);
}

void PhysXColliderComponent::Start(){
	SearchAttachPhysXComponent();
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
		if (mAttachPhysXComponent){
			mAttachPhysXComponent->RemoveShape(*mShape);
		}
		mShape->release();
	}
	else{
		if (mShape != shape && mShape){
			mShape->release();
		}
	}

	mShape = shape;
	if (mShape){
		if (mAttachPhysXComponent){
			mAttachPhysXComponent->AddShape(*mShape);
		}
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




	SetTransform(mPosition);

}


void PhysXColliderComponent::ChangeShape(bool flag){
	PxShape* shape = NULL;
	mIsSphere = flag;
	if (mIsSphere){
		shape = Game::GetPhysX()->CreateShapeSphere();
	}
	else{
		shape = Game::GetPhysX()->CreateShape();
	}

	ShapeAttach(shape);
}

void PhysXColliderComponent::ChangeMaterial(const PhysxMaterialAssetDataPtr& material){
	mPhysicsMaterialFile = "";
	mPhysicsMaterial = material;
	if (mPhysicsMaterialFile == "")return;
	AssetDataBase::Instance(mPhysicsMaterialFile.c_str(), mPhysicsMaterial);
	if (!mPhysicsMaterial)return;
	if (!mPhysicsMaterial->GetFileData())return;
	auto pxmate = mPhysicsMaterial->GetFileData()->GetMaterial();
	if (!pxmate)return;
	mShape->setMaterials(&pxmate, 1);
}
void PhysXColliderComponent::ChangeMaterial(const std::string& file){
	mPhysicsMaterialFile = file;
	mPhysicsMaterial = NULL;
	if (mPhysicsMaterialFile == "")return;
	AssetDataBase::Instance(mPhysicsMaterialFile.c_str(), mPhysicsMaterial);
	if (!mPhysicsMaterial)return;
	if (!mPhysicsMaterial->GetFileData())return;
	auto pxmate = mPhysicsMaterial->GetFileData()->GetMaterial();
	if (!pxmate)return;
	mShape->setMaterials(&pxmate, 1);
}
PhysxMaterialAssetDataPtr PhysXColliderComponent::GetMaterial(){
	return mPhysicsMaterial;
}

void PhysXColliderComponent::CreateMesh(const MeshAssetDataPtr& mesh){

	if (!mesh)return;
	auto shape = Game::GetPhysX()->CreateTriangleMesh(mesh->GetFileData()->GetPolygonsData());

	ShapeAttach(shape);
}
void PhysXColliderComponent::CreateMesh(const std::string& file){

	mMeshFile = file;
	if (file == "")return;
	MeshAssetDataPtr data;
	AssetDataBase::Instance(mMeshFile.c_str(), data);
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

bool PhysXColliderComponent::GetIsTrigger(){
	return mIsTrigger;
}


physx::PxShape* PhysXColliderComponent::GetShape(){
	return mShape;
}
#ifdef _ENGINE_MODE
void PhysXColliderComponent::CreateInspector() {
	auto data = Window::CreateInspector();
	BoolCollback collback = [&](bool value){
		ChangeShape(value);
	};
	std::function<void(std::string)> collbackpath = [&](std::string name){
		CreateMesh(name);
	};
	BoolCollback collbacktri = [&](bool value){
		SetIsTrigger(value);
	};
	std::function<void(std::string)> collbackmatepath = [&](std::string name){
		ChangeMaterial(name);
	};

	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Mesh", &mMeshFile, collbackpath), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("IsSphere", &mIsSphere, collback), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("IsTrigger", &mIsTrigger, collbacktri), data);
	Window::AddInspector(new InspectorVector3DataSet("Position", &mPosition.x, [&](float f){SetTransform(XMVectorSet(f, mPosition.y, mPosition.z, 1)); }, &mPosition.y, [&](float f){SetTransform(XMVectorSet(mPosition.x, f, mPosition.z, 1)); }, &mPosition.z, [&](float f){SetTransform(XMVectorSet(mPosition.x, mPosition.y, f, 1)); }), data);
	Window::AddInspector(new InspectorVector3DataSet("Scale", &mScale.x, [&](float f){SetScale(XMVectorSet(f, mScale.y, mScale.z, 1)); }, &mScale.y, [&](float f){SetScale(XMVectorSet(mScale.x, f, mScale.z, 1)); }, &mScale.z, [&](float f){SetScale(XMVectorSet(mScale.x, mScale.y, f, 1)); }), data);
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("PhysxMaterial", &mPhysicsMaterialFile, collbackmatepath), data);
	Window::ViewInspector("Collider", this, data);
}
#endif

void PhysXColliderComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mMeshFile);
	_KEY(mIsSphere);
	_KEY(mIsTrigger);
	_KEY(mPosition.x);
	_KEY(mPosition.y);
	_KEY(mPosition.z);
	_KEY(mPosition.w);
	_KEY(mScale.x);
	_KEY(mScale.y);
	_KEY(mScale.z);
	_KEY(mScale.w);
	_KEY(mPhysicsMaterialFile);

#undef _KEY
}

void PhysXColliderComponent::DrawMesh(ID3D11DeviceContext* context, const Material& material){
	auto g = mShape->getGeometry();
	if (g.getType() == PxGeometryType::eBOX){
		if (mDebugStr != "box"){
			mDebugStr = "box";
			mDebugDraw.Create("EngineResource/box.tesmesh");
		}
	}
	else if (g.getType() == PxGeometryType::eSPHERE){
		if (mDebugStr != "sphere"){
			mDebugStr = "sphere";
			mDebugDraw.Create("EngineResource/ball.tesmesh");
		}
	}
	else if (g.getType() == PxGeometryType::eTRIANGLEMESH){

		if (mDebugStr != mMeshFile){
			mDebugStr = mMeshFile;
			mDebugDraw.Create(mMeshFile.c_str());
		}
	}
	if (!mDebugDraw.IsCreate()){
		return;
	}

	auto transform = mShape->getLocalPose();
	
	Actor* par = gameObject;
	if (mAttachPhysXComponent){
		par = mAttachPhysXComponent->gameObject;
	}

	auto rot = par->mTransform->GetMatrix();
	rot.r[0] = XMVector3Normalize(rot.r[0]);
	rot.r[1] = XMVector3Normalize(rot.r[1]);
	rot.r[2] = XMVector3Normalize(rot.r[2]);
	auto quat = XMQuaternionRotationMatrix(rot);

	
	auto shmat = XMMatrixMultiply(
		XMMatrixRotationQuaternion(XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w)),
		XMMatrixTranslationFromVector(XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f)));

	auto wpos = par->mTransform->WorldPosition();

	auto Matrix = XMMatrixMultiply(
		XMMatrixMultiply(
		XMMatrixMultiply(
		XMMatrixScalingFromVector(mScale),
		shmat),
		XMMatrixRotationQuaternion(quat)),
		XMMatrixTranslationFromVector(wpos));

	//auto p = gameObject->mTransform->GetMatrix().r[3];
	mDebugDraw.mWorld = Matrix;
	mDebugDraw.Update();
	mDebugDraw.Draw(context, material);
}

void PhysXColliderComponent::SetTransform(const XMVECTOR& pos){
	mPosition = pos;


	XMVECTOR scale = XMVectorSet(1, 1, 1, 1);
	if (mIsParentPhysX){
		scale = gameObject->mTransform->GetParent()->mTransform->WorldScale();
	}

	auto pos_ = gameObject->mTransform->Position();
	auto transform = mShape->getLocalPose();
	if (!mIsParentPhysX){
		transform.p = PxVec3(mPosition.x, mPosition.y, mPosition.z);
	}
	else{
		scale = gameObject->mTransform->GetParent()->mTransform->LossyScale();
		transform.p = PxVec3(pos_.x*scale.x, pos_.y*scale.y, pos_.z*scale.z) + PxVec3(mPosition.x, mPosition.y, mPosition.z);
	}


	if (mIsParentPhysX){

		auto quat = gameObject->mTransform->Quaternion();
		transform.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
	}
	mShape->setLocalPose(transform);

}
const XMVECTOR& PhysXColliderComponent::GetTransform() const{
	return mPosition;
}

void PhysXColliderComponent::SetScale(const XMVECTOR& scale){
	mScale = scale;

	//PxBoxGeometry box(PxVec3(1.0f*scale.x, 1.0f*scale.y, 1.0f*scale.z));
	auto g = mShape->getGeometry();
	if (g.getType() == PxGeometryType::eBOX){
		g.box().halfExtents = PxVec3(PxVec3(abs(0.5f*scale.x), abs(0.5f*scale.y), abs(0.5f*scale.z)));
		mShape->setGeometry(g.box());
	}
	else if (g.getType() == PxGeometryType::eSPHERE){
		g.sphere().radius = abs(0.5f*scale.x);
		mShape->setGeometry(g.sphere());
	}
	else if (g.getType() == PxGeometryType::eTRIANGLEMESH){

		auto rotate = gameObject->mTransform->Quaternion();
		auto q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);

		g.triangleMesh().scale = PxMeshScale(PxVec3(scale.x, scale.y, scale.z), q);
		mShape->setGeometry(g.triangleMesh());
	}

}
const XMVECTOR& PhysXColliderComponent::GetScale() const{
	return mScale;
}