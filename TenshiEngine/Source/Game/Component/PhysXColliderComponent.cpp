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
#include "Engine/AssetLoad.h"

PhysXColliderComponent::PhysXColliderComponent(){
	mGeometryType = GeometryType::Box;
	mShape = NULL;
	mAttachTarget = NULL;
	mIsTrigger = false;
	mPosition = XMVectorSet(0, 0, 0, 1);
	mScale = XMVectorSet(1, 1, 1, 1);
	mMeshAsset.Free();
	mPhysicsMaterialAsset.Free();
	//mIsParentPhysX = false;


	mGameObjectPosition = XMVectorSet(1, 1, 1, 1);
	mGameObjectRotate = XMVectorSet(1, 1, 1, 1);
	mGameObjectScale = XMVectorSet(1, 1, 1, 1);

	mShapeMatrix = XMMatrixIdentity();
}

PhysXColliderComponent::~PhysXColliderComponent(){

}
void PhysXColliderComponent::Initialize(){

	AssetLoad::Instance(mMeshAsset.m_Hash, mMeshAsset);
	AssetLoad::Instance(mPhysicsMaterialAsset.m_Hash, mPhysicsMaterialAsset);

	if (!mShape){
		switch (mGeometryType)
		{
		case  GeometryType::Box:
			ChangeShapeBox();
			break;
		case  GeometryType::Sphere:
			ChangeShapeSphere();
			break;
		case  GeometryType::Mesh:
			ChangeShapeMesh(mMeshAsset);
			break;
		case  GeometryType::Capsule:
			ChangeShapeCapsule();
			break;
		case  GeometryType::ConvexMesh:
			ChangeShapeConvexMesh(mMeshAsset);
			break;
		default:
			break;
		}
	}

	SetTransform(mPosition,true);
	SetScale(mScale);
}

void PhysXColliderComponent::Start(){
	if (!mAttachPhysXComponent){
		SearchAttachPhysXComponent();
		ShapeAttach(mShape);
	}
}

#include "ScriptComponent.h"
void PhysXColliderComponent::Finish(){

	//int layer = gameObject->GetLayer();
	//auto l = (Layer::Enum)(1 << layer);
	//auto hit = Game::Get()->GetPhysX()->GetLayerCollideFlag(l);

	//Game::GetPhysX()->OverlapHitMultiple(gameObject->GetComponent<PhysXColliderComponent>(), [&](auto obj) {
	//	auto script0 = obj->GetComponent<ScriptComponent>();
	//	auto script1 = gameObject->GetComponent<ScriptComponent>();
	//	if (script0)
	//		script0->LostCollide(gameObject.Get());
	//	if (script1)
	//		script1->LostCollide(obj.Get());
	//}, (Layer::Enum)hit);

	ShapeAttach(NULL);
	mAttachPhysXComponent = NULL;
	//mIsParentPhysX = false;

	mMeshAsset.Free();
	mPhysicsMaterialAsset.Free();
#ifdef _ENGINE_MODE
	mDebugDraw.Release();
#endif
}

#ifdef _ENGINE_MODE
void PhysXColliderComponent::EngineUpdate(){
	Update();
}
#endif
void PhysXColliderComponent::Update(){

	SetTransform(mPosition);

	//int layer = gameObject->GetLayer();
	//auto l = (Layer::Enum)(1 << layer);
	//auto hit = Game::Get()->GetPhysX()->GetLayerCollideFlag(l);

	//Game::GetPhysX()->OverlapHitMultiple(gameObject->GetComponent<PhysXColliderComponent>(), [&](auto obj) {
	//	auto script0 = obj->GetComponent<ScriptComponent>();
	//	auto script1 = gameObject->GetComponent<ScriptComponent>();
	//	if (script0)
	//		script0->LostCollide(gameObject.Get());
	//	if (script1)
	//		script1->LostCollide(obj.Get());
	//}, (Layer::Enum)hit);
}

void PhysXColliderComponent::ChangeParentCallback(){
	//対象が違った場合のみ当て直し
	auto back = mAttachPhysXComponent;
	SearchAttachPhysXComponent();
	if (back != mAttachPhysXComponent) {
		mAttachPhysXComponent = back;
		ReleaseAttach();
		SearchAttachPhysXComponent();
		ShapeAttach(mShape);
	}
	SetTransform(mPosition, true);
}


void PhysXColliderComponent::AttachPhysxComponent(weak_ptr<PhysXComponent> com){
	//解放
	ReleaseAttach();

	if (com){
		mAttachPhysXComponent = com;

		auto par = com->gameObject;
		//mIsParentPhysX = (par != gameObject) ? true : false;
		AttachRigidDynamic(true);
	}
	else {
		mAttachPhysXComponent = NULL;
		//mIsParentPhysX = false;
		AttachRigidStatic(true);
	}
}

bool PhysXColliderComponent::SearchAttachPhysXComponent(){
	mAttachPhysXComponent = NULL;
	auto par = gameObject;
	while (par){
		auto com = par->GetComponent<PhysXComponent>();
		if (com){
			mAttachPhysXComponent = com;
			//mIsParentPhysX = (par != gameObject)?true:false;
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

		mShape = shape;
		ShapeReSettings();

	}
	if(mShape)mShape->userData = gameObject.Get();

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
		mShape->userData = NULL;
		mShape->release();
		mShape = NULL;
	}
}
void PhysXColliderComponent::ReleaseAttach(){
	AttachRigidDynamic(false);
	AttachRigidStatic(false);
}

void PhysXColliderComponent::ChangeShapeBox(){
	mMeshAsset.Free();
	mGeometryType = GeometryType::Box;
	if (!IsEnabled())return;
	PxShape* shape = Game::GetPhysX()->CreateShape();

	ShapeAttach(shape);
}
void PhysXColliderComponent::ChangeShapeSphere() {
	mMeshAsset.Free();
	mGeometryType = GeometryType::Sphere;
	if (!IsEnabled())return;
	PxShape* shape = Game::GetPhysX()->CreateShapeSphere();

	ShapeAttach(shape);
}
void PhysXColliderComponent::ChangeShapeCapsule()
{
	mMeshAsset.Free();
	mGeometryType = GeometryType::Capsule;
	if (!IsEnabled())return;
	PxShape* shape = Game::GetPhysX()->CreateShapeCapsule();

	ShapeAttach(shape);
}
void PhysXColliderComponent::ChangeShapeMesh(const MeshAsset& mesh) {
	mMeshAsset = mesh;
	mGeometryType = GeometryType::Mesh;
	if (!IsEnabled())return;
	if (!mMeshAsset.IsLoad()) {
		ShapeAttach(NULL);
		return;
	}
	auto shape = Game::GetPhysX()->CreateTriangleMesh(mMeshAsset.Get()->GetPolygonsData());

	ShapeAttach(shape);
}

void PhysXColliderComponent::ChangeShapeConvexMesh(const MeshAsset & mesh)
{
	mMeshAsset = mesh;
	mGeometryType = GeometryType::ConvexMesh;
	if (!IsEnabled())return;
	if (!mMeshAsset.IsLoad()) {
		ShapeAttach(NULL);
		return;
	}
	auto shape = Game::GetPhysX()->CreateConvexMesh(mMeshAsset.Get()->GetPolygonsData());

	ShapeAttach(shape);
}

void PhysXColliderComponent::ChangeMaterial(const PhysxMaterialAsset& material){
	mPhysicsMaterialAsset = material;
	if (!mPhysicsMaterialAsset.IsLoad())return;
	auto pxmate = mPhysicsMaterialAsset.Get()->GetMaterial();
	if (!pxmate)return;

	if (!mShape)return;
	mShape->setMaterials(&pxmate, 1);
}
PhysxMaterialAsset PhysXColliderComponent::GetMaterial(){
	return mPhysicsMaterialAsset;
}

void PhysXColliderComponent::SetIsTrigger(bool flag){
	mIsTrigger = flag;
	//PxShapeFlag::eTRIGGER_SHAPE または　PxShapeFlag::eSIMULATION_SHAPE 

	if (!mShape)return;
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

	Inspector ins("Collider",this);
	ins.AddEnableButton(this);
	static bool mIni = false;
	static std::vector<std::string> types;
	if (!mIni) {
		mIni = true;
		types.push_back("Box");
		types.push_back("Sphere");
		types.push_back("Mesh");
		types.push_back("Capsule");
		types.push_back("ConvexMesh");
	}
	ins.AddSelect("GeometryType", &mGeometryType, types, [&](int value) {
		mGeometryType = value;
		switch (mGeometryType)
		{
			case  GeometryType::Box:
				ChangeShapeBox();
				break;
			case  GeometryType::Sphere:
				ChangeShapeSphere();
				break;
			case  GeometryType::Mesh:
				ChangeShapeMesh(mMeshAsset);
				break;
			case  GeometryType::Capsule:
				ChangeShapeCapsule();
				break;
			case  GeometryType::ConvexMesh:
				ChangeShapeConvexMesh(mMeshAsset);
				break;
			default:
				break;
		}
	});
	ins.Add("Mesh", &mMeshAsset, [&]() {
		if (mGeometryType == GeometryType::ConvexMesh) {
			ChangeShapeConvexMesh(mMeshAsset);
		}
		else {
			ChangeShapeMesh(mMeshAsset);
		}
	});
	ins.Add("IsTrigger", &mIsTrigger, [&](bool value) {
		SetIsTrigger(value);
	});
	auto p = Vector3(mPosition);
	auto s = Vector3(mScale);

	ins.Add("Position", &p, [&](Vector3 f){SetTransform(XMVectorSet(f.x, f.y, f.z, 1)); }
	);
	ins.Add("Scale", &s, [&](Vector3 f){SetScale(XMVectorSet(f.x, f.y, f.z, 1)); });
	ins.Add("PhysxMaterial", &mPhysicsMaterialAsset, [&]() {
		ChangeMaterial(mPhysicsMaterialAsset);
	});

	ins.Complete();
}
#endif

void PhysXColliderComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(mMeshAsset);
	_KEY(mGeometryType);
	_KEY(mIsTrigger);
	_KEY(mPosition.x);
	_KEY(mPosition.y);
	_KEY(mPosition.z);
	_KEY(mPosition.w);
	_KEY(mScale.x);
	_KEY(mScale.y);
	_KEY(mScale.z);
	_KEY(mScale.w);
	_KEY(mPhysicsMaterialAsset);

#undef _KEY
}

void PhysXColliderComponent::DrawMesh(ID3D11DeviceContext* context, const Material& material){

#ifdef _ENGINE_MODE
	if (!mShape || !gameObject)return;
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
	else if (g.getType() == PxGeometryType::eCAPSULE) {
		if (mDebugStr != "box") {
			mDebugStr = "box";
			mDebugDraw.Create("EngineResource/box.tesmesh");
		}
	}
	else if (g.getType() == PxGeometryType::eTRIANGLEMESH){

		if (mDebugStr != mMeshAsset.m_Name){
			mDebugStr = mMeshAsset.m_Name;
			mDebugDraw.Create(mDebugStr.c_str());
		}
	}
	else if (g.getType() == PxGeometryType::eCONVEXMESH) {

		if (mDebugStr != mMeshAsset.m_Name) {
			mDebugStr = mMeshAsset.m_Name;
			mDebugDraw.Create(mDebugStr.c_str());
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
#endif
}

void PhysXColliderComponent::SetTransform(const XMVECTOR& pos,bool force){
	mPosition = pos;

	if (!mShape || !gameObject)return;
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
			
			if (!change && !force)return;
	
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
	if (!mShape)return;

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
	else if (g.getType() == PxGeometryType::eCAPSULE) {
		g.capsule().halfHeight = abs(0.5f*s.x);
		g.capsule().radius = abs(0.5f*s.y);
		mShape->setGeometry(g.capsule());
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
	else if (g.getType() == PxGeometryType::eCONVEXMESH) {
		physx::PxQuat q = physx::PxQuat(0, 0, 0, 1);
		if (gameObject) {
			auto rotate = gameObject->mTransform->Quaternion();
			q = physx::PxQuat(rotate.x, rotate.y, rotate.z, rotate.w);
		}

		g.convexMesh().scale = PxMeshScale(PxVec3(s.x, s.y, s.z), q);
		mShape->setGeometry(g.convexMesh());
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

void PhysXColliderComponent::ShapeReSettings()
{
	if (!mShape || !gameObject)return;
	mShape->userData = gameObject.Get();

	ChangeMaterial(mPhysicsMaterialAsset);
	SetPhysxLayer(gameObject->GetLayer());
	SetIsTrigger(mIsTrigger);
}
