#include "PhysXColliderComponent.h"

#include "PhysXComponent.h"

#include "TransformComponent.h"

#include "Game/Game.h"
#include "Game/Actor.h"
#include "physx/physx3.h"

#include "Engine/AssetDataBase.h"

#include "Engine/AssetFile/Mesh/MeshFileData.h"
#include "Engine/AssetFile/Physx/PhysxMaterialFileData.h"


#include "Engine/Inspector.h"

PhysXColliderComponent::PhysXColliderComponent(){
	mIsSphere = false;
	mShape = NULL;
	mAttachTarget = NULL;
	mIsTrigger = false;
	mPosition = XMVectorSet(0, 0, 0, 1);
	mScale = XMVectorSet(1, 1, 1, 1);
	mPhysicsMaterial = NULL;
	mIsParentPhysX = false;


	mGameObjectPosition = XMVectorSet(1, 1, 1, 1);
	mGameObjectRotate = XMVectorSet(1, 1, 1, 1);
	mGameObjectScale = XMVectorSet(1, 1, 1, 1);

	mShapeMatrix = XMMatrixIdentity();
}

PhysXColliderComponent::~PhysXColliderComponent(){

}
void PhysXColliderComponent::Initialize(){

	if (!mShape){
		CreateMesh(mMeshFile);
		
		if (!mShape){
			ChangeShape(mIsSphere);
		}
	}
	ChangeMaterial(mPhysicsMaterialFile);

	SetTransform(mPosition);
	SetScale(mScale);

	SetPhysxLayer(gameObject->GetLayer());
}

void PhysXColliderComponent::Start(){
	SetIsTrigger(mIsTrigger);
	if (!mAttachPhysXComponent){
		SearchAttachPhysXComponent();
		ShapeAttach(mShape);
	}
}

void PhysXColliderComponent::Finish(){
	ShapeAttach(NULL);
	mAttachPhysXComponent = NULL;
	mIsParentPhysX = false;
}

#ifdef _ENGINE_MODE
void PhysXColliderComponent::EngineUpdate(){
	Update();
}
#endif
void PhysXColliderComponent::Update(){

	SetTransform(mPosition);
}

void PhysXColliderComponent::ChangeParentCallback(){
	ReleaseAttach();
	SearchAttachPhysXComponent();
	ShapeAttach(mShape);
}


void PhysXColliderComponent::AttachPhysxComponent(weak_ptr<PhysXComponent> com){
	//解放
	ReleaseAttach();

	if (com){
		mAttachPhysXComponent = com;

		auto par = com->gameObject;
		mIsParentPhysX = (par != gameObject) ? true : false;
		AttachRigidDynamic(true);
	}
	else {
		mAttachPhysXComponent = NULL;
		mIsParentPhysX = false;
		AttachRigidStatic(true);
	}
}

bool PhysXColliderComponent::SearchAttachPhysXComponent(){
	auto par = gameObject;
	while (par){
		auto com = par->GetComponent<PhysXComponent>();
		if (com){
			mAttachPhysXComponent = com;
			mIsParentPhysX = (par != gameObject)?true:false;
			return true;
		}
		par = par->mTransform->GetParent();
	}
	return false;
}

void PhysXColliderComponent::ShapeAttach(PxShape* shape){

	//解放
	ReleaseAttach();
	if (mShape != shape){
		ReleaseShape();
	}
	mShape = shape;


	//リジッドダイナミックがあれば
	if (mAttachPhysXComponent){
		AttachRigidDynamic(true);
	}
	else{
		AttachRigidStatic(true);
	}
}

//シェイプをリジッドスタティックにアタッチするか削除する
void PhysXColliderComponent::AttachRigidStatic(bool attach){
	if (!mShape){
		mAttachTarget = NULL;
		return;
	}
	if (attach && IsEnabled()){
		mAttachTarget = -1;
		Game::GetPhysX()->AddStaticShape(mShape);
	}
	else if (mAttachTarget == -1){
		mAttachTarget = NULL;
		Game::GetPhysX()->RemoveStaticShape(mShape);
	}
}
//シェイプをリジッドダイナミックにアタッチするか削除する
void PhysXColliderComponent::AttachRigidDynamic(bool attach){
	if (!mShape){
		mAttachTarget = NULL;
		return;
	}
	if (!mAttachPhysXComponent){
		return;
	}
	if (attach && IsEnabled()){
		if (mAttachPhysXComponent->AddShape(*mShape)){
			mAttachTarget = (int)mAttachPhysXComponent->gameObject.Get();
		}
	}
	else if (mAttachTarget > 0){
		if (mAttachTarget == (int)mAttachPhysXComponent->gameObject.Get()){
			mAttachPhysXComponent->RemoveShape(*mShape);
		}
		mAttachTarget = NULL;
	}
}

void PhysXColliderComponent::ReleaseShape(){
	if (mShape){
		mShape->release();
		mShape = NULL;
	}
}
void PhysXColliderComponent::ReleaseAttach(){
	AttachRigidDynamic(false);
	AttachRigidStatic(false);
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

	if (shape){
		shape->userData = gameObject.Get();
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

	if (shape){
		shape->userData = gameObject.Get();
	}
	ShapeAttach(shape);
}
void PhysXColliderComponent::CreateMesh(const std::string& file){

	mMeshFile = file;
	if (file == "")return;
	MeshAssetDataPtr data;
	AssetDataBase::Instance(mMeshFile.c_str(), data);
	if (!data)return;
	auto shape = Game::GetPhysX()->CreateTriangleMesh(data->GetFileData()->GetPolygonsData());
	if (shape){
		shape->userData = gameObject.Get();
	}
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

	Inspector ins("Collider",this);
	ins.AddEnableButton(this);
	ins.Add("Mesh", &mMeshFile, collbackpath);
	ins.Add("IsSphere", &mIsSphere, collback);
	ins.Add("IsTrigger", &mIsTrigger, collbacktri);
	auto p = Vector3(mPosition);
	auto s = Vector3(mScale);

	ins.Add("Position", &p, [&](Vector3 f){SetTransform(XMVectorSet(f.x, f.y, f.z, 1)); }
	);
	ins.Add("Scale", &s, [&](Vector3 f){SetScale(XMVectorSet(f.x, f.y, f.z, 1)); });
	ins.Add("PhysxMaterial", &mPhysicsMaterialFile, collbackmatepath);

	ins.Complete();
}
#endif

void PhysXColliderComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

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


	auto scale = mScale;
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
			scale.y = scale.x;
			scale.z = scale.x;
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
	
	auto par = gameObject;
	if (mAttachPhysXComponent){
		par = mAttachPhysXComponent->gameObject;
	}

	//auto quat = par->mTransform->WorldQuaternion();
	//if (par.Get() == gameObject.Get()){
	//	quat = XMQuaternionIdentity();
	//}

	//XMMATRIX shmat;
	//if (mAttachTarget == -1){
	//	shmat = XMMatrixRotationQuaternion(XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
	//}
	//else
	//{
	//	shmat = XMMatrixMultiply(
	//		XMMatrixRotationQuaternion(XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w)),
	//		XMMatrixTranslationFromVector(XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f)));
	//}

	//auto wpos = par->mTransform->WorldPosition();
	//wpos += XMVector3Rotate(mPosition * mGameObjectScale, mGameObjectRotate);
	//auto Matrix = XMMatrixMultiply(
	//	XMMatrixMultiply(
	//	XMMatrixMultiply(
	//	XMMatrixScalingFromVector(scale),
	//	shmat),
	//	XMMatrixRotationQuaternion(quat)),
	//	XMMatrixTranslationFromVector(wpos));


	XMMATRIX ParentMatrix;

	if (mAttachPhysXComponent){
		ParentMatrix = par->mTransform->GetMatrix();
		ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
		ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
		ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);
	}
	else{
		ParentMatrix = XMMatrixIdentity();
	}
		scale *= mGameObjectScale;

	auto LocalMatrix = XMMatrixMultiply(
		XMMatrixRotationQuaternion(XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w)),
		XMMatrixTranslationFromVector(XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f)));
	auto ScaleMatrix = XMMatrixScalingFromVector(scale);

	auto Matrix = XMMatrixMultiply(
		XMMatrixMultiply(
		ScaleMatrix,
		mShapeMatrix),
		ParentMatrix);

	//auto p = gameObject->mTransform->GetMatrix().r[3];
	mDebugDraw.mWorld = Matrix;
	mDebugDraw.Update();
	mDebugDraw.Draw(context, material);
}

void PhysXColliderComponent::SetTransform(const XMVECTOR& pos){
	mPosition = pos;
	if (mAttachTarget == -1){

		auto scale = gameObject->mTransform->WorldScale();
		if (scale.x != mGameObjectScale.x ||
			scale.y != mGameObjectScale.y ||
			scale.z != mGameObjectScale.z){
			mGameObjectScale = scale;
			SetScale(mScale);
		}
	
		bool change = false;
		auto quat = gameObject->mTransform->WorldQuaternion();
		if (quat.x != mGameObjectRotate.x ||
			quat.y != mGameObjectRotate.y ||
			quat.z != mGameObjectRotate.z ||
			quat.w != mGameObjectRotate.w){
			mGameObjectRotate = quat;
			change = true;
		}
	
	
		auto pos_ = gameObject->mTransform->WorldPosition();
		pos_ += XMVector3Rotate(mPosition * mGameObjectScale, mGameObjectRotate);
		if (pos_.x != mGameObjectPosition.x ||
			pos_.y != mGameObjectPosition.y ||
			pos_.z != mGameObjectPosition.z){
			mGameObjectPosition = pos_;
			change = true;
		}
	
		if (!mAttachPhysXComponent){
			if (!change)return;
	
			PxTransform transform;
			transform.p = PxVec3(pos_.x, pos_.y, pos_.z);
			transform.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
			mShape->setLocalPose(transform);
			mShapeMatrix = XMMatrixMultiply(
				XMMatrixRotationQuaternion(XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w)),
				XMMatrixTranslationFromVector(XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f)));
			return;
		}
	}

	auto par = gameObject;
	if (mAttachPhysXComponent){
		par = mAttachPhysXComponent->gameObject;
	}

	XMVECTOR null_v;
	auto mat = par->mTransform->GetMatrix();
	auto parLossyScale = par->mTransform->LossyScale();
	auto inv = XMMatrixInverse(&null_v, mat);

	auto WorldScale = gameObject->mTransform->WorldScale();
	auto addMatrix = XMMatrixMultiply(XMMatrixScalingFromVector(mScale * WorldScale * parLossyScale), XMMatrixTranslationFromVector(mPosition * WorldScale * parLossyScale));
	auto Matrix = gameObject->mTransform->GetMatrix();
	Matrix = XMMatrixMultiply(addMatrix, Matrix);
	Matrix = XMMatrixMultiply(Matrix, inv);

	Matrix.r[3] /= parLossyScale;
	auto pxPos = PxVec3(Matrix.r[3].x, Matrix.r[3].y, Matrix.r[3].z);
	//XMVECTOR scale;
	//scale.x = XMVector3Length(Matrix.r[0]).x;
	//scale.y = XMVector3Length(Matrix.r[1]).x;
	//scale.z = XMVector3Length(Matrix.r[2]).x;
	//mGameObjectScale = scale;
	mGameObjectScale = gameObject->mTransform->WorldScale();

	Matrix.r[0] = XMVector3Normalize(Matrix.r[0]);
	Matrix.r[1] = XMVector3Normalize(Matrix.r[1]);
	Matrix.r[2] = XMVector3Normalize(Matrix.r[2]);
	auto rotQuat = XMQuaternionRotationMatrix(Matrix);


	//auto posMatrix = XMMatrixTranslationFromVector(mGameObjectPosition);
	//auto rotMatrix = XMMatrixRotationQuaternion(mGameObjectRotate);
	//auto sclMatrix = XMMatrixScalingFromVector(mGameObjectScale);
	//posMatrix = XMMatrixMultiply(posMatrix, inv);
	//rotMatrix = XMMatrixMultiply(rotMatrix, inv);
	//sclMatrix = XMMatrixMultiply(sclMatrix, mat);
	//sclMatrix = XMMatrixMultiply(sclMatrix, inv);
	//
	//rotMatrix.r[3] = XMVectorSet(0, 0, 0, 1);
	//auto rotQuat = XMQuaternionRotationMatrix(rotMatrix);

	physx::PxTransform transform;
	transform.p = pxPos;
	transform.q = PxQuat(rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w);

	//XMVECTOR scale;
	//mGameObjectScale.x = XMVector3Length(sclMatrix.r[0]).x;
	//mGameObjectScale.y = XMVector3Length(sclMatrix.r[1]).x;
	//mGameObjectScale.z = XMVector3Length(sclMatrix.r[2]).x;
	//scale.w = 1.0f;
	//mGameObjectScale *= scale;
	SetScale(mScale);

	//XMVECTOR scale = XMVectorSet(1, 1, 1, 1);
	//if (mIsParentPhysX){
	//	scale = gameObject->mTransform->GetParent()->mTransform->WorldScale();
	//}
	//
	//auto pos_ = gameObject->mTransform->Position();
	//auto transform = mShape->getLocalPose();
	//if (!mIsParentPhysX){
	//	transform.p = PxVec3(mPosition.x, mPosition.y, mPosition.z);
	//}
	//else{
	//	scale = gameObject->mTransform->GetParent()->mTransform->LossyScale();
	//	transform.p = PxVec3(pos_.x*scale.x, pos_.y*scale.y, pos_.z*scale.z) + PxVec3(mPosition.x, mPosition.y, mPosition.z);
	//}
	//
	//
	//if (mIsParentPhysX){
	//
	//	auto quat = gameObject->mTransform->Quaternion();
	//	//f (mAttachPhysXComponent){
	//	//	Actor* par = mAttachPhysXComponent->gameObject;
	//	//	auto pq = par->mTransform->WorldQuaternion();
	//	//	pq = XMQuaternionInverse(pq);
	//	//	quat = XMQuaternionMultiply(pq, quat);
	//	//
	//
	//	transform.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
	//}
	//else{
	//	auto quat = XMQuaternionIdentity();
	//	transform.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
	//}
	mShape->setLocalPose(transform);
	


	mShapeMatrix = XMMatrixMultiply(
		XMMatrixRotationQuaternion(XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w)),
		XMMatrixTranslationFromVector(XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f)));


}
const XMVECTOR& PhysXColliderComponent::GetTransform() const{
	return mPosition;
}

void PhysXColliderComponent::SetScale(const XMVECTOR& scale){
	mScale = scale;
	auto s = mScale * mGameObjectScale;

	//PxBoxGeometry box(PxVec3(1.0f*scale.x, 1.0f*scale.y, 1.0f*scale.z));
	auto g = mShape->getGeometry();
	if (g.getType() == PxGeometryType::eBOX){
		g.box().halfExtents = PxVec3(PxVec3(abs(0.5f*s.x), abs(0.5f*s.y), abs(0.5f*s.z)));
		mShape->setGeometry(g.box());
	}
	else if (g.getType() == PxGeometryType::eSPHERE){
		g.sphere().radius = abs(0.5f*s.x);
		mShape->setGeometry(g.sphere());
	}
	else if (g.getType() == PxGeometryType::eTRIANGLEMESH){
		physx::PxQuat q = physx::PxQuat(0,0,0,1);
		if (gameObject){
			auto rotate = gameObject->mTransform->Quaternion();
			q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);
		}

		g.triangleMesh().scale = PxMeshScale(PxVec3(s.x, s.y, s.z), q);
		mShape->setGeometry(g.triangleMesh());
	}

}
const XMVECTOR& PhysXColliderComponent::GetScale() const{
	return mScale;
}

void PhysXColliderComponent::SetPhysxLayer(int layer){
	if (!mShape)return;
	PxFilterData  filterData;
	auto l = (Layer::Enum)(1 << layer);

	filterData.word0 = l;  //ワード0 =自分のID 
	mShape->setSimulationFilterData(filterData);
	mShape->setQueryFilterData(filterData);
}


void PhysXColliderComponent::OnEnabled(){

	ShapeAttach(mShape);
}
void PhysXColliderComponent::OnDisabled(){

	ReleaseAttach();
}