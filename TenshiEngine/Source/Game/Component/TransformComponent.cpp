
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
	Update();
}
void TransformComponent::Update(){
	auto physx = gameObject->GetComponent<PhysXComponent>();
	if (physx){
		physx->SetTransform();
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
}
void TransformComponent::Position(const XMVECTOR& position){
	mPosition = position;
	FlagSetChangeMatrix(PhysXChangeTransformFlag::Position);
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

		//クォータニオンで計算
		if (mRotate.w != 1.0f){
			mMatrix = XMMatrixMultiply(
				XMMatrixMultiply(
				XMMatrixScalingFromVector(mScale),
				XMMatrixRotationQuaternion(mRotate)),
				XMMatrixTranslationFromVector(mPosition));

		}
		//ロール、ピッチ、ヨウで計算
		else{
			mMatrix = XMMatrixMultiply(
				XMMatrixMultiply(
				XMMatrixScalingFromVector(mScale),
				//XMMatrixRotationQuaternion(mRotate)),
				XMMatrixRotationRollPitchYawFromVector(mRotate)),
				XMMatrixTranslationFromVector(mPosition));
		}
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

void TransformComponent::CopyData(Component* post, Component* base){
	auto Post = (TransformComponent*)post;
	auto Base = (TransformComponent*)base;

	//Post->mChildren.clear();
	Post->mPosition = Base->mPosition;
	Post->mRotate = Base->mRotate;
	Post->mScale = Base->mScale;
	Post->mParent = Base->mParent;
	Post->FlagSetChangeMatrix((PhysXChangeTransformFlag)0);
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
		auto pos = this->Rotate();
		this->Rotate(XMVectorSet(f, pos.y, pos.z, pos.w));
	};
	std::function<void(float)> collbackry = [&](float f){
		auto pos = this->Rotate();
		this->Rotate(XMVectorSet(pos.x, f, pos.z, pos.w));
	};
	std::function<void(float)> collbackrz = [&](float f){
		auto pos = this->Rotate();
		this->Rotate(XMVectorSet(pos.x, pos.y, f, pos.w));
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
	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorVector3DataSet("Position", &mPosition.x, collbackpx, &mPosition.y, collbackpy, &mPosition.z, collbackpz), data);
	Window::AddInspector(new InspectorVector3DataSet("Rotate", &mRotate.x, collbackrx, &mRotate.y, collbackry, &mRotate.z, collbackrz), data);
	Window::AddInspector(new InspectorVector3DataSet("Scale", &mScale.x, collbacksx, &mScale.y, collbacksy, &mScale.z, collbacksz), data);
	Window::ViewInspector("Transform", this, data);

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


void TransformComponent::AddForce(const XMVECTOR& force){
	auto c = gameObject->GetComponent<PhysXComponent>();
	if (c)
		c->AddForce(force);
}
void TransformComponent::AddTorque(const XMVECTOR& force){
	auto c = gameObject->GetComponent<PhysXComponent>();
	if (c)
		c->AddTorque(force);
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