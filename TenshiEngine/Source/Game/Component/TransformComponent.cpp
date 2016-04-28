
#include "TransformComponent.h"

#include "Window/Window.h"

#include "Game/Actor.h"
#include "Game/Game.h"

TransformComponent::TransformComponent()
	:mFixMatrixFlag(false)
	, mParent(NULL)
	, mParentUniqueID(NULL){
	mMatrix = XMMatrixIdentity();
	mScale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	mRotate = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	mPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);


	mInspectorRotateDegree = mRotate * (180.0f / XM_PI);
	mInspectorRotateDegree.w = 1;
}


TransformComponent::~TransformComponent(){
}

void TransformComponent::Initialize(){
	FlagSetChangeMatrix((PhysXChangeTransformFlag)0);
}

void TransformComponent::Start(){
	if (mParentUniqueID){
		mParent = Game::FindUID(mParentUniqueID);
	}
	SetParent(mParent);
}
void TransformComponent::Finish(){
	if (mParent)
		mParent->mTransform->Children().remove(gameObject);
}

void TransformComponent::EngineUpdate(){
	auto physx = gameObject->GetComponent<PhysXComponent>();
	if (physx){
		physx->SetTransform(false);
	}
}
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

inline XMMATRIX fromEulerXYZ(const XMVECTOR &v)
{
	XMMATRIX m;

	XMVECTOR r = XMVectorSet(v.x, v.y, v.z,1);

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

	r00 = cy * cz;
	r01 = -cy * sz;
	r02 = sy;

	r10 = cz * sx * sy + cx * sz;
	r11 = cx * cz - sx * sy * sz;
	r12 = -cy * sx;

	r20 = -cx * cz * sy + sx * sz;
	r21 = cz * sx + cx * sy * sz;
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
inline XMVECTOR toEulerXYZ(const XMMATRIX &m)
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

	const float &r20 = m._13;
	const float &r21 = m._23;
	const float &r22 = m._33;

	if (r02 < +1)
	{
		if (r02 > -1)
		{
			thetaY = asinf(r02);
			thetaX = atan2f(-r12, r22);
			thetaZ = atan2f(-r01, r00);
		}
		else     // r02 = -1
		{
			// Not a unique solution: thetaZ - thetaX = atan2f(r10,r11)
			thetaY = -XM_PI/2;
			thetaX = -atan2f(r10, r11);
			thetaZ = 0;
		}
	}
	else // r02 = +1
	{
		// Not a unique solution: thetaZ + thetaX = atan2f(r10,r11)
		thetaY = +XM_PI/2;
		thetaX = atan2f(r10, r11);
		thetaZ = 0;
	}

	//thetaX = ofRadToDeg(thetaX);
	//thetaY = ofRadToDeg(thetaY);
	//thetaZ = ofRadToDeg(thetaZ);

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

	mRotate = toEulerXYZ(mat);

	Rotate(mRotate);
}

const XMVECTOR TransformComponent::Quaternion() const{
	return XMQuaternionRotationMatrix(fromEulerXYZ(mRotate));
}

const XMVECTOR TransformComponent::LossyScale() const{
	auto m = GetMatrix();
	
	return XMVectorSet(1/XMVector3Length(m.r[0]).x, 1/XMVector3Length(m.r[1]).x, 1/XMVector3Length(m.r[2]).x, 1);
}
const XMVECTOR& TransformComponent::WorldPosition() const{
	return GetMatrix().r[3];
}
void TransformComponent::WorldPosition(const XMVECTOR& position){
	auto wp = mParent->mTransform->WorldPosition();
	mPosition = position - wp;
	FlagSetChangeMatrix(PhysXChangeTransformFlag::Position);
}

const XMVECTOR& TransformComponent::Forward() const{
	return GetMatrix().r[2];
}
const XMVECTOR& TransformComponent::Left() const{
	return GetMatrix().r[0];
}
const XMVECTOR& TransformComponent::Up() const{
	return GetMatrix().r[1];
}

const XMMATRIX& TransformComponent::GetMatrix() const{


	if (!mFixMatrixFlag){

		mFixMatrixFlag = true;


		auto physx = gameObject->GetComponent<PhysXComponent>();

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
				fromEulerXYZ(mRotate)),
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
			mMatrix = XMMatrixMultiply(mMatrix, mParent->mTransform->GetMatrix());
	}
	return mMatrix;
}
//このゲームオブジェクトより子のオブジェクトを全てデストロイする
void TransformComponent::AllChildrenDestroy(){
	while (mChildren.size()){
		Actor* child = Children().front();
		Children().pop_front();
		//親と子が同時に死ぬとエラーが出る
		child->mTransform->SetParent(NULL);
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


void TransformComponent::SetUndo(const XMVECTOR& pos){
	
	//コンポーネントロード時にやらないとおかしい
	static XMVECTOR mLastUndoSetPos = mPosition;

	// 外部に動かされた場合の ( 元いた場所 - 現在地 ) をUndoに追加
	if (!XMVector3NearEqual(mLastUndoSetPos, pos, XMVectorSet(0.1f, 0.1f, 0.1f, 1))){
		Game::SetUndo(new ChangeParamFuncCommand<XMVECTOR>(mLastUndoSetPos, mPosition, [&](const XMVECTOR& p){this->Position(p); }));
		mLastUndoSetPos = pos;
	}
	// ( 現在地 - 指定した位置 ) をUndoに追加
	if (!XMVector3NearEqual(pos, mPosition, XMVectorSet(0.01f, 0.01f, 0.01f, 1))){
		Game::SetUndo(new ChangeParamFuncCommand<XMVECTOR>(mPosition, pos, [&](const XMVECTOR& p){this->Position(p); }));
		mLastUndoSetPos = pos;
	}
}


void TransformComponent::CreateInspector(){


	std::function<void(float)> collbackpx = [&](float f){
		auto pos = this->Position();
		SetUndo(XMVectorSet(f, pos.x, pos.z, pos.w));
		this->Position(XMVectorSet(f, pos.y, pos.z, pos.w));
	};
	std::function<void(float)> collbackpy = [&](float f){
		auto pos = this->Position();
		SetUndo(XMVectorSet(pos.x, f, pos.z, pos.w));
		this->Position(XMVectorSet(pos.x, f, pos.z, pos.w));
	};
	std::function<void(float)> collbackpz = [&](float f){
		auto pos = this->Position();
		SetUndo(XMVectorSet(pos.x, pos.y, f, pos.w));
		this->Position(XMVectorSet(pos.x, pos.y, f, pos.w));
	};
	std::function<void(float)> collbackrx = [&](float f){
		//auto pos = this->Rotate();
		//this->Rotate(XMVectorSet(f*(180.0f / XM_PI), pos.y, pos.z, pos.w));

		mRotate.x = f * (XM_PI / 180.0f);
		FlagSetChangeMatrix(PhysXChangeTransformFlag::Rotate);

		mInspectorRotateDegree.x = f;
	};
	std::function<void(float)> collbackry = [&](float f){
		mRotate.y = f * (XM_PI / 180.0f);
		FlagSetChangeMatrix(PhysXChangeTransformFlag::Rotate);

		mInspectorRotateDegree.y = f;
	};
	std::function<void(float)> collbackrz = [&](float f){
		mRotate.z = f * (XM_PI/180.0f);
		FlagSetChangeMatrix(PhysXChangeTransformFlag::Rotate);

		mInspectorRotateDegree.z = f;
	};
	std::function<void(float)> collbacksx = [&](float f){
		auto pos = this->Scale();
		this->Scale(XMVectorSet(f, pos.y, pos.z, pos.w));
	};
	std::function<void(float)> collbacksy = [&](float f){
		auto pos = this->Scale();
		this->Scale(XMVectorSet(pos.x, f, pos.z, pos.w));
	};
	std::function<void(float)> collbacksz = [&](float f){
		auto pos = this->Scale();
		this->Scale(XMVectorSet(pos.x, pos.y, f, pos.w));
	};

	mInspectorRotateDegree = mRotate * (180.0f / XM_PI);
	mInspectorRotateDegree.w = 1;

	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorVector3DataSet("Position", &mPosition.x, collbackpx, &mPosition.y, collbackpy, &mPosition.z, collbackpz), data);
	Window::AddInspector(new InspectorVector3DataSet("Rotate", &mInspectorRotateDegree.x, collbackrx, &mInspectorRotateDegree.y, collbackry, &mInspectorRotateDegree.z, collbackrz), data);
	Window::AddInspector(new InspectorVector3DataSet("Scale", &mScale.x, collbacksx, &mScale.y, collbacksy, &mScale.z, collbacksz), data);
	Window::ViewInspector("Transform", NULL, data);

	//Window::GetInspectorWindow()->AddLabel("Transform");
	//Window::GetInspectorWindow()->AddParam(&mPosition.x, &mFixMatrixFlag);
	//Window::GetInspectorWindow()->AddParam(&mPosition.y, &mFixMatrixFlag);
	//Window::GetInspectorWindow()->AddParam(&mPosition.z, &mFixMatrixFlag);
}

void TransformComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mParentUniqueID);
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

std::list<Actor*>& TransformComponent::Children(){
	return mChildren;
}
Actor* TransformComponent::GetParent(){
	return mParent;
}
void TransformComponent::SetParent(Actor* parent){
	if (mParent)
		mParent->mTransform->Children().remove(gameObject);
	mParent = parent;
	mParentUniqueID = 0;
	if (parent){
		parent->mTransform->Children().push_back(gameObject);
		mParentUniqueID = parent->GetUniqueID();
	}
	FlagSetChangeMatrix((PhysXChangeTransformFlag)0);
}