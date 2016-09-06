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
	mAttachTarget = NULL;
	mIsTrigger = false;
	mPosition = XMVectorSet(0, 0, 0, 1);
	mScale = XMVectorSet(1, 1, 1, 1);
	mPhysicsMaterial = NULL;
	mIsParentPhysX = false;
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

void PhysXColliderComponent::EngineUpdate(){
	Update();
}
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

		Actor* par = com->gameObject;
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
	Actor* par = gameObject;
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
	if (attach){
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
	if (attach){
		if (mAttachPhysXComponent->AddShape(*mShape)){
			mAttachTarget = (int)mAttachPhysXComponent->gameObject;
		}
	}
	else if (mAttachTarget > 0){
		if (mAttachTarget == (int)mAttachPhysXComponent->gameObject){
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
		shape->userData = gameObject;
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
		shape->userData = gameObject;
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
		shape->userData = gameObject;
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

	auto quat = par->mTransform->WorldQuaternion();
	if (par == gameObject){
		auto quat = XMQuaternionIdentity();
	}

	XMMATRIX shmat;
	if (mAttachTarget == -1){
		shmat = XMMatrixIdentity();
	}
	else{
		shmat = XMMatrixMultiply(
			XMMatrixRotationQuaternion(XMVectorSet(transform.q.x, transform.q.y, transform.q.z, transform.q.w)),
			XMMatrixTranslationFromVector(XMVectorSet(transform.p.x, transform.p.y, transform.p.z, 1.0f)));
	}

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

	if (mAttachTarget == -1){
		auto pos_ = gameObject->mTransform->WorldPosition();
		auto quat = gameObject->mTransform->WorldQuaternion();
		PxTransform transform;
		transform.p = PxVec3(pos_.x, pos_.y, pos_.z);
		transform.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
		mShape->setLocalPose(transform);
		return;
	}


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
		//f (mAttachPhysXComponent){
		//	Actor* par = mAttachPhysXComponent->gameObject;
		//	auto pq = par->mTransform->WorldQuaternion();
		//	pq = XMQuaternionInverse(pq);
		//	quat = XMQuaternionMultiply(pq, quat);
		//

		transform.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
	}
	else{
		auto quat = XMQuaternionIdentity();
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

void PhysXColliderComponent::SetPhysxLayer(int layer){
	if (!mShape)return;
	PxFilterData  filterData;
	auto l = (Layer::Enum)(1 << layer);

	filterData.word0 = l;  //ワード0 =自分のID 
	mShape->setSimulationFilterData(filterData);
	mShape->setQueryFilterData(filterData);
}