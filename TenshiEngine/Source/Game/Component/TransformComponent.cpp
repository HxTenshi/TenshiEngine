
#include "TransformComponent.h"

#include "Window/Window.h"

#include "Game/Actor.h"
#include "Game/Game.h"

#include "Engine/Inspector.h"

TransformComponent::TransformComponent()
	:mFixMatrixFlag(false)
	, mParent(NULL)
	, mIsEndInitialize(false){
	mParentUniqueHashID.clear();
	mMatrix = XMMatrixIdentity();
	mScale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	mRotate = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	mPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);


	mInspectorRotateDegree = mRotate * (180.0f / XM_PI);
	mInspectorRotateDegree.w = 1;

#ifdef _ENGINE_MODE
	m_EngineObject = false;
#endif
}


TransformComponent::~TransformComponent(){
}

void TransformComponent::Initialize(){
	//TransformはInitializeが２回よばれるかも
	if (!mIsEndInitialize) {
		if (!mParent) {
			GameObject par = NULL;
			if (!mParentUniqueHashID.IsNull()) {
				//if (mParent && mParent->GetUniqueID() == mParentUniqueHashID)return;
				par = Game::FindUID(mParentUniqueHashID);
			}
			SetParent(par);
		}
		FlagSetChangeMatrix((PhysXChangeTransformFlag)0);
	}
	mIsEndInitialize = true;
}

void TransformComponent::Start(){

}
void TransformComponent::Finish(){
	if (mParent) {
		auto children = mParent->mTransform->ChildrenRef();
		children->remove_if([&](GameObject act) {
			return gameObject.Get() == act.Get();
		});

		mParent = NULL;
	}
	mParentUniqueHashID.clear();

	mIsEndInitialize = false;
}

#ifdef _ENGINE_MODE
void TransformComponent::EngineUpdate(){
	auto physx = gameObject->GetComponent<PhysXComponent>();
	if (physx){
		physx->SetTransform(false);
	}
}
#endif
void TransformComponent::Update(){
	auto physx = gameObject->GetComponent<PhysXComponent>();
	if (physx){
		physx->SetTransform(true);
	}
}

const XMVECTOR& TransformComponent::Scale() const{
	return mScale;
}
const XMVECTOR& TransformComponent::Rotate() const{
	return mRotate;
}

const XMVECTOR& TransformComponent::Position() const{
	return mPosition;
}

void TransformComponent::Scale(const XMVECTOR& scale){
	mScale = scale;
	FlagSetChangeMatrix((PhysXChangeTransformFlag)0);
}
void TransformComponent::Rotate(const XMVECTOR& rotate){
	mRotate = rotate;
	FlagSetChangeMatrix(PhysXChangeTransformFlag::Rotate);

	mInspectorRotateDegree = mRotate * (180.0f / XM_PI);
	mInspectorRotateDegree.w = 1;
}
void TransformComponent::Position(const XMVECTOR& position){
	mPosition = position;
	FlagSetChangeMatrix(PhysXChangeTransformFlag::Position);
}

const XMVECTOR TransformComponent::DegreeRotate() const{
	return mInspectorRotateDegree;
}

void TransformComponent::DegreeRotate(const XMVECTOR& rotate){
	mRotate = rotate * (XM_PI / 180.0f);
	FlagSetChangeMatrix(PhysXChangeTransformFlag::Rotate);

	mRotate.w = 1;

	mInspectorRotateDegree = rotate;
	mInspectorRotateDegree.w = 1;
}



//inline XMMATRIX fromEulerZXY(const XMVECTOR &v)
//{
//	XMMATRIX m;
//
//	XMVECTOR r = XMVectorSet(v.x, v.y, v.z, 1);
//
//	const float cx = cosf(r.x);
//	const float cy = cosf(r.y);
//	const float cz = cosf(r.z);
//
//	const float sx = sinf(r.x);
//	const float sy = sinf(r.y);
//	const float sz = sinf(r.z);
//
//	float &r00 = m._11;
//	float &r01 = m._21;
//	float &r02 = m._31;
//
//	float &r10 = m._12;
//	float &r11 = m._22;
//	float &r12 = m._32;
//
//	float &r20 = m._13;
//	float &r21 = m._23;
//	float &r22 = m._33;
//
//	r00 = cy * cz - sx * sy * sz;
//	r01 = -cx * sz;
//	r02 = cz * sy + cy * sx * sz;
//
//	r10 = cz * sx * sy + cy * sz;
//	r11 = cx * cz;
//	r12 = -cy * cz * sx + sy * sz;
//
//	r20 = -cx * sy;
//	r21 = sx;
//	r22 = cx * cy;
//
//	m._41 = 0;
//	m._42 = 0;
//	m._43 = 0;
//	m._14 = 0;
//	m._24 = 0;
//	m._34 = 0;
//	m._44 = 1;
//
//	return m;
//}
//inline XMVECTOR toEulerZXY(const XMMATRIX &m)
//{
//	XMVECTOR v;
//	v.w = 1;
//	float &thetaX = v.x;
//	float &thetaY = v.y;
//	float &thetaZ = v.z;
//
//	const float &r00 = m._11;
//	const float &r01 = m._21;
//	const float &r02 = m._31;
//
//	const float &r10 = m._12;
//	const float &r11 = m._22;
//	const float &r12 = m._32;
//
//	const float &r20 = m._13;
//	const float &r21 = m._23;
//	const float &r22 = m._33;
//
//	if (r21 < +1)
//	{
//		if (r21 > -1)
//		{
//			thetaX = asinf(r21);
//			thetaZ = atan2f(-r01, r11);
//			thetaY = atan2f(-r20, r22);
//		}
//		else // r21 = -1
//		{
//			// Not a unique solution: thetaY - thetaZ = atan2f(r02,r00)
//			thetaX = -XM_PI / 2;
//			thetaZ = -atan2f(r02, r00);
//			thetaY = 0;
//		}
//	}
//	else // r21 = +1
//	{
//		// Not a unique solution: thetaY + thetaZ = atan2f(r02,r00)
//		thetaX = +XM_PI / 2;
//		thetaZ = atan2f(r02, r00);
//		thetaY = 0;
//	}
//
//	return v;
//}
//
//inline XMMATRIX fromEulerXYZ(const XMVECTOR &v)
//{
//	XMMATRIX m;
//
//	XMVECTOR r = XMVectorSet(v.x, v.y, v.z,1);
//
//	const float cx = cosf(r.x);
//	const float cy = cosf(r.y);
//	const float cz = cosf(r.z);
//
//	const float sx = sinf(r.x);
//	const float sy = sinf(r.y);
//	const float sz = sinf(r.z);
//
//	float &r00 = m._11;
//	float &r01 = m._21;
//	float &r02 = m._31;
//
//	float &r10 = m._12;
//	float &r11 = m._22;
//	float &r12 = m._32;
//
//	float &r20 = m._13;
//	float &r21 = m._23;
//	float &r22 = m._33;
//
//	r00 = cy * cz;
//	r01 = -cy * sz;
//	r02 = sy;
//
//	r10 = cz * sx * sy + cx * sz;
//	r11 = cx * cz - sx * sy * sz;
//	r12 = -cy * sx;
//
//	r20 = -cx * cz * sy + sx * sz;
//	r21 = cz * sx + cx * sy * sz;
//	r22 = cx * cy;
//
//	m._41 = 0;
//	m._42 = 0;
//	m._43 = 0;
//	m._14 = 0;
//	m._24 = 0;
//	m._34 = 0;
//	m._44 = 1;
//	return m;
//}
//inline XMVECTOR toEulerXYZ(const XMMATRIX &m)
//{
//	XMVECTOR v;
//	v.w = 1;
//	float &thetaX = v.x;
//	float &thetaY = v.y;
//	float &thetaZ = v.z;
//
//	const float &r00 = m._11;
//	const float &r01 = m._21;
//	const float &r02 = m._31;
//
//	const float &r10 = m._12;
//	const float &r11 = m._22;
//	const float &r12 = m._32;
//
//	const float &r20 = m._13;
//	const float &r21 = m._23;
//	const float &r22 = m._33;
//
//	if (r02 < +1)
//	{
//		if (r02 > -1)
//		{
//			thetaY = asinf(r02);
//			thetaX = atan2f(-r12, r22);
//			thetaZ = atan2f(-r01, r00);
//		}
//		else     // r02 = -1
//		{
//			// Not a unique solution: thetaZ - thetaX = atan2f(r10,r11)
//			thetaY = -XM_PI/2;
//			thetaX = -atan2f(r10, r11);
//			thetaZ = 0;
//		}
//	}
//	else // r02 = +1
//	{
//		// Not a unique solution: thetaZ + thetaX = atan2f(r10,r11)
//		thetaY = +XM_PI/2;
//		thetaX = atan2f(r10, r11);
//		thetaZ = 0;
//	}
//
//	//thetaX = ofRadToDeg(thetaX);
//	//thetaY = ofRadToDeg(thetaY);
//	//thetaZ = ofRadToDeg(thetaZ);
//
//	return v;
//}


inline XMMATRIX fromEulerYXZ(const XMVECTOR &v)
{
	XMMATRIX m;

	XMVECTOR r = XMVectorSet(v.x, v.y, v.z, 1);

	const float cx = cosf(r.x);
	const float cy = cosf(r.y);
	const float cz = cosf(r.z);

	const float sx = sinf(r.x);
	const float sy = sinf(r.y);
	const float sz = sinf(r.z);

	float &r00 = m._11;
	float &r01 = m._21;
	float &r02 = m._31;

	float &r10 = m._12;
	float &r11 = m._22;
	float &r12 = m._32;

	float &r20 = m._13;
	float &r21 = m._23;
	float &r22 = m._33;

	r00 = cy * cz + sx * sy * sz;
	r01 = cz * sx * sy - cy * sz;
	r02 = cx * sy;

	r10 = cx * sz;
	r11 = cx * cz;
	r12 = -sx;

	r20 = -cz * sy + cy * sx * sz;
	r21 = cy * cz * sx + sy * sz;
	r22 = cx * cy;
	m._41 = 0;
	m._42 = 0;
	m._43 = 0;
	m._14 = 0;
	m._24 = 0;
	m._34 = 0;
	m._44 = 1;
	return m;
}
inline XMVECTOR toEulerYXZ(const XMMATRIX &m)
{
	XMVECTOR v;
	v.w = 1;
	float &thetaX = v.x;
	float &thetaY = v.y;
	float &thetaZ = v.z;

	const float &r00 = m._11;
	const float &r01 = m._21;
	const float &r02 = m._31;

	const float &r10 = m._12;
	const float &r11 = m._22;
	const float &r12 = m._32;

//	const float &r20 = m._13;
//	const float &r21 = m._23;
	const float &r22 = m._33;

	if (r12 < +1)
	{
		if (r12 > -1)
		{
			thetaX = asinf(-r12);
			thetaY = atan2f(r02, r22);
			thetaZ = atan2f(r10, r11);
		}
		else // r12 = -1
		{
			// Not a unique solution: thetaZ - thetaY = atan2f(-r01,r00)
			thetaX = +XM_PI / 2;
			thetaY = -atan2f(-r01, r00);
			thetaZ = 0;
		}
	}
	else // r12 = +1
	{
		// Not a unique solution: thetaZ + thetaY = atan2f(-r01,r00)
		thetaX = -XM_PI / 2;
		thetaY = atan2f(-r01, r00);
		thetaZ = 0;
	}

	return v;
}
void TransformComponent::Quaternion(const XMVECTOR& Quaternion){

	//auto gyro = Quaternion;
	//float yaw = atan2(2 * gyro.x * gyro.y + 2 * gyro.w * gyro.z, gyro.w * gyro.w + gyro.x * gyro.x - gyro.y * gyro.y - gyro.z * gyro.z);
	//float pitch = -asin(2 * gyro.w * gyro.y - 2 * gyro.x * gyro.z);
	//float roll = -atan2(2 * gyro.y * gyro.z + 2 * gyro.w * gyro.x, -gyro.w * gyro.w + gyro.x * gyro.x + gyro.y * gyro.y - gyro.z * gyro.z);
	//mRotate.x = roll;
	//mRotate.y = pitch;
	//mRotate.z = yaw;
	//mRotate.w = 1;


	//float x = Quaternion.x;
	//float y = Quaternion.y;
	//float z = Quaternion.z;
	//float w = Quaternion.w;
	//float xAngle = atan2(2 * y*w - 2 * x*z, 1 - 2 * y*y - 2 * z*z);
	//float yAngle = atan2(2 * x*w - 2 * y*z, 1 - 2 * x*x - 2 * z*z);
	//float zAngle = -asin(2 * x*y + 2 * z*w);
	//mRotate.x = xAngle;
	//mRotate.y = yAngle;
	//mRotate.z = zAngle;
	//mRotate.w = 1;
	

	XMMATRIX mat = XMMatrixRotationQuaternion(Quaternion);

	mRotate = toEulerYXZ(mat);

	Rotate(mRotate);
}

const XMVECTOR TransformComponent::Quaternion() const{
	return XMQuaternionRotationMatrix(fromEulerYXZ(mRotate));
}
const XMVECTOR TransformComponent::WorldScale() const{
	auto m = GetMatrix();

	return XMVectorSet(XMVector3Length(m.r[0]).x, XMVector3Length(m.r[1]).x, XMVector3Length(m.r[2]).x, 1);
}

const XMVECTOR TransformComponent::LossyScale() const{
	auto m = GetMatrix();
	
	return XMVectorSet(1/XMVector3Length(m.r[0]).x, 1/XMVector3Length(m.r[1]).x, 1/XMVector3Length(m.r[2]).x, 1);
}
const XMVECTOR TransformComponent::WorldQuaternion() const{

	auto m = GetMatrix();
	m.r[0] = XMVector3Normalize(m.r[0]);
	m.r[1] = XMVector3Normalize(m.r[1]);
	m.r[2] = XMVector3Normalize(m.r[2]);
	return XMQuaternionRotationMatrix(m);
}
const XMVECTOR& TransformComponent::WorldPosition() const{
	return GetMatrix().r[3];
}
void TransformComponent::WorldPosition(const XMVECTOR& position){
	auto mat = mParent?mParent->mTransform->GetMatrix():XMMatrixIdentity();
	XMVECTOR null;
	mat = XMMatrixInverse(&null, mat);
	mat = XMMatrixTranslationFromVector(position) * mat;
	Position(mat.r[3]);
}
void TransformComponent::WorldQuaternion(const XMVECTOR& quaternion){

	auto wq = mParent?mParent->mTransform->WorldQuaternion():XMQuaternionIdentity();
	auto q = XMQuaternionMultiply(quaternion, XMQuaternionInverse(wq));
	Quaternion(q);
}
void TransformComponent::WorldScale(const XMVECTOR& scale){
	//Quaternion();
	auto mat = GetMatrix();
	XMFLOAT3 w(mat.r[0].w, mat.r[1].w, mat.r[2].w);
	mat.r[0] = XMVector3Normalize(mat.r[0]) * scale.x;
	mat.r[1] = XMVector3Normalize(mat.r[1]) * scale.y;
	mat.r[2] = XMVector3Normalize(mat.r[2]) * scale.z;
	mat.r[0].w = w.x;
	mat.r[1].w = w.y;
	mat.r[2].w = w.z;
	auto m = mParent? mParent->mTransform->GetMatrix() :XMMatrixIdentity();
	//m = fromEulerYXZ(mRotate) * m;
	XMVECTOR null;
	m = XMMatrixInverse(&null, m);
	m = XMMatrixMultiply(mat, m);
	auto s = XMVectorSet(XMVector3Length(m.r[0]).x, XMVector3Length(m.r[1]).x, XMVector3Length(m.r[2]).x, 1);
	//auto s = XMVector3Rotate(scale, XMQuaternionInverse(WorldQuaternion()));
	////auto s = scale;
	//m = XMMatrixScalingFromVector(s) * m;
	//s = XMVectorSet(XMVector3Length(m.r[0]).x, XMVector3Length(m.r[1]).x, XMVector3Length(m.r[2]).x, 1);
	//s = XMVector3Rotate(s, WorldQuaternion());
	//s.x = abs(s.x);
	//s.y = abs(s.y);
	//s.z = abs(s.z);
	//s.w = 1.0f;

	//auto sm = XMMatrixScalingFromVector(scale);
	//auto rm = XMMatrixInverse(&null, fromEulerYXZ(mRotate));
	//sm = sm * rm;
	//m = sm * m;
	//auto s = XMVectorSet(XMVector3Length(m.r[0]).x, XMVector3Length(m.r[1]).x, XMVector3Length(m.r[2]).x, 1);

	Scale(s);
}

void TransformComponent::WorldMatrix(const XMMATRIX & matrix)
{
	WorldPosition(matrix.r[3]);
	mFixMatrixFlag = true;

	auto m = XMMatrixIdentity();
	m.r[0] = XMVector3Normalize(matrix.r[0]);
	m.r[1] = XMVector3Normalize(matrix.r[1]);
	m.r[2] = XMVector3Normalize(matrix.r[2]);
	WorldQuaternion(XMQuaternionRotationMatrix(m));
	mFixMatrixFlag = true;

	WorldScale(XMVectorSet(XMVector3Length(matrix.r[0]).x, XMVector3Length(matrix.r[1]).x, XMVector3Length(matrix.r[2]).x, 1));
}

const XMVECTOR TransformComponent::Forward() const{
	return XMVector3Normalize(GetMatrix().r[2]);
}
const XMVECTOR TransformComponent::Left() const{
	return XMVector3Normalize(GetMatrix().r[0]);
}
const XMVECTOR TransformComponent::Up() const{
	return XMVector3Normalize(GetMatrix().r[1]);
}

const XMMATRIX& TransformComponent::GetMatrix() const{


	if (!mFixMatrixFlag){

		mFixMatrixFlag = true;


		//auto physx = gameObject->GetComponent<PhysXComponent>();

		//auto gyro = mQuaternion;
		//float yaw = atan2(2 * gyro.x * gyro.y + 2 * gyro.w * gyro.z, gyro.w * gyro.w + gyro.x * gyro.x - gyro.y * gyro.y - gyro.z * gyro.z);
		//float pitch = asin(2 * gyro.w * gyro.y - 2 * gyro.x * gyro.z);
		//float roll = atan2(2 * gyro.y * gyro.z + 2 * gyro.w * gyro.x, -gyro.w * gyro.w + gyro.x * gyro.x + gyro.y * gyro.y - gyro.z * gyro.z);
		//
		//
		//auto q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

		//クォータニオンで計算
		//if (physx || mRotate.w != 1){


			mMatrix = XMMatrixMultiply(
				XMMatrixMultiply(
				XMMatrixScalingFromVector(mScale),
				fromEulerYXZ(mRotate)),
				XMMatrixTranslationFromVector(mPosition));

		//}
		//ロール、ピッチ、ヨウで計算
		//else{
		//	mMatrix = XMMatrixMultiply(
		//		XMMatrixMultiply(
		//		XMMatrixScalingFromVector(mScale),
		//		//XMMatrixRotationQuaternion(mRotate)),
		//		XMMatrixRotationRollPitchYawFromVector(mRotate)),
		//		XMMatrixTranslationFromVector(mPosition));
		//}
		//テスクチャー用（仮）
		//mMatrix._44 = mPosition.w;


		if (mParent)
			mMatrix = XMMatrixMultiply(mMatrix,mParent->mTransform->GetMatrix());
	}
	return mMatrix;
}
//このゲームオブジェクトより子のオブジェクトを全てデストロイする
void TransformComponent::AllChildrenDestroy(){
	while (mChildren.size()){
		auto child = mChildren.front();
		mChildren.pop_front();
		if (!child)continue;
		//親と子が同時に死ぬとエラーが出る
		Game::DestroyObject(child);
		child->mTransform->AllChildrenDestroy();
	}
}

void TransformComponent::FlagSetChangeMatrix(PhysXChangeTransformFlag flag){
	mFixMatrixFlag = false;
	UpdatePhysX(flag);
	for (auto& child : mChildren){
		child->mTransform->FlagSetChangeMatrix(flag);
	}
}

#ifdef _ENGINE_MODE

void TransformComponent::CreateInspector(){


	std::function<void(Vector3)> collbackp = [&](Vector3 f){
		auto pos = this->Position();
		this->Position(XMVectorSet(f.x, f.y, f.z, pos.w));
		Game::SetUndo(gameObject.Get());
	};
	std::function<void(Vector3)> collbackr = [&](Vector3 f){
		mRotate.x = f.x * (XM_PI / 180.0f);
		mRotate.y = f.y * (XM_PI / 180.0f);
		mRotate.z = f.z * (XM_PI / 180.0f);
		FlagSetChangeMatrix(PhysXChangeTransformFlag::Rotate);

		mInspectorRotateDegree.x = f.x;
		mInspectorRotateDegree.y = f.y;
		mInspectorRotateDegree.z = f.z;
		Game::SetUndo(gameObject.Get());
	};
	std::function<void(Vector3)> collbacks = [&](Vector3 f){
		auto pos = this->Scale();
		this->Scale(XMVectorSet(f.x, f.y, f.z, pos.w));
		Game::SetUndo(gameObject.Get());
	};

	mInspectorRotateDegree = mRotate * (180.0f / XM_PI);
	mInspectorRotateDegree.w = 1;

	Inspector ins("Transform", this);
	auto p = Vector3(mPosition);
	auto r = Vector3(mInspectorRotateDegree);
	auto s = Vector3(mScale);
	ins.Add("Position", &p, collbackp);
	ins.Add("Rotate", &r, collbackr);
	ins.Add("Scale", &s, collbacks);

	ins.Complete();
}
#endif

void TransformComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(mParentUniqueHashID);
	_KEY(mScale.x);
	_KEY(mScale.y);
	_KEY(mScale.z);
	_KEY(mScale.w);
	_KEY(mRotate.x);
	_KEY(mRotate.y);
	_KEY(mRotate.z);
	_KEY(mRotate.w);
	_KEY(mPosition.x);
	_KEY(mPosition.y);
	_KEY(mPosition.z);
	_KEY(mPosition.w);

#undef _KEY
}

void TransformComponent::UpdatePhysX(PhysXChangeTransformFlag flag){
	//Physicsと競合するためポジションを動かした場合コチラを優先
	if (!gameObject)return;
	auto c = gameObject->GetComponent<PhysXComponent>();
	if (c)
		c->SetChengeTransform(flag);
}


void TransformComponent::AddForce(const XMVECTOR& force, ForceMode::Enum forceMode){
	auto c = gameObject->GetComponent<PhysXComponent>();
	if (c)
		c->AddForce(force, forceMode);
}
void TransformComponent::AddTorque(const XMVECTOR& force, ForceMode::Enum forceMode){
	auto c = gameObject->GetComponent<PhysXComponent>();
	if (c)
		c->AddTorque(force, forceMode);
}

std::list<GameObject> TransformComponent::Children(){
	return mChildren;
}
GameObject TransformComponent::GetParent(){
	return mParent;
}

void TransformComponent::SetParentUniqueID(UniqueID id){
	mParentUniqueHashID = id;
}
void TransformComponent::SetParent(GameObject parent){
	if (!parent) {
		parent = Game::GetRootActor();
	}

	auto p = parent;
#ifdef _ENGINE_MODE
	if (!m_EngineObject)
#endif
	for (;;) {
		if (p == Game::GetRootActor())break;
		if (!p || p == gameObject)return;
		p = p->mTransform->GetParent();
	}
	if (mParent == parent)return;
	if (gameObject == parent)return;
	if (!gameObject)return;
	if (mParent){
		auto children = mParent->mTransform->ChildrenRef();
		children->remove_if([&](GameObject act){
			return gameObject.Get() == act.Get();
		});
	}
	mParent = parent;
	mParentUniqueHashID.clear();
	if (parent){
		parent->mTransform->ChildrenRef()->push_back(gameObject);
		mParentUniqueHashID = parent->GetUniqueID();

		if(mIsEndInitialize)
			gameObject->RunChangeParentCallback();
	}
	FlagSetChangeMatrix((PhysXChangeTransformFlag)0);

#ifdef _ENGINE_MODE
	//エラーで落ちる
	if(!m_EngineObject)
	Game::Get()->WindowParentSet(gameObject->shared_from_this());
#endif
}

void TransformComponent::SetParentWorld(GameObject parent){

	if (!gameObject)return;

	auto pos = WorldPosition();
	auto quat = WorldQuaternion();
	auto scale = WorldScale();

	SetParent(parent);

	WorldPosition(pos);
	mFixMatrixFlag = true;
	WorldQuaternion(quat);
	mFixMatrixFlag = true;
	WorldScale(scale);
}

