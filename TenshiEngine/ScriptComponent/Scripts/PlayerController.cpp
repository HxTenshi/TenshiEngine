#include "PlayerController.h"

#include "../h_standard.h"
#include "../h_component.h"
#include "Game/Component/CharacterControllerComponent.h"

//生成時に呼ばれます（エディター中も呼ばれます）
void PlayerController::Initialize(){
	mJump = XMVectorZero();
	mGravity = XMVectorSet(0, -9.81f, 0,1);
	mRotate = XMVectorZero();
	m_IsDoge = false;
	m_IsGround = false;
}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void PlayerController::Start(){

	Hx::System()->LockCursorPositionToWindowCenter(true);

	m_CharacterControllerComponent = gameObject->GetComponent<CharacterControllerComponent>();

}

//毎フレーム呼ばれます
void PlayerController::Update(){

	if (Input::Down(KeyCode::Key_ESCAPE)) {
		Hx::System()->LockCursorPositionToWindowCenter(false);
	}
	if (Input::Down(MouseCode::Left)) {
		Hx::System()->LockCursorPositionToWindowCenter(true);
	}


	if (!m_CharacterControllerComponent) {
		m_CharacterControllerComponent = gameObject->GetComponent<CharacterControllerComponent>();
		if (!m_CharacterControllerComponent)return;
	}

	if (m_CharacterControllerComponent->IsGround()) {
		m_IsGround = true;
		m_IsDoge = false;
	}

	if(!m_IsDoge)
		move();

	moveUpdate();

	if (m_IsGround) {
		doge();
	}

	rotate();

	if (!m_WeaponHand)return;
	if (Input::Down(KeyCode::Key_F)) {
		guard();
	}
	if (Input::Down(MouseCode::Left)) {
		attack();
	}
}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void PlayerController::Finish(){

}

//コライダーとのヒット時に呼ばれます
void PlayerController::OnCollideBegin(GameObject target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void PlayerController::OnCollideEnter(GameObject target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void PlayerController::OnCollideExit(GameObject target){
	(void)target;
}

void PlayerController::move()
{

	float time = Hx::DeltaTime()->GetDeltaTime();
	float speed = m_MoveSpeed;
	float x = 0, y = 0;
	if (Input::Down(KeyCode::Key_W)) {
		y = 1.0f;
	}
	if (Input::Down(KeyCode::Key_S)) {
		y = -1.0f;
	}
	if (Input::Down(KeyCode::Key_D)) {
		x = 1.0f;
	}
	if (Input::Down(KeyCode::Key_A)) {
		x = -1.0f;
	}


	m_IsGround = m_CharacterControllerComponent->IsGround();

	XMVECTOR pos = gameObject->mTransform->WorldPosition();
	if (m_IsGround) {
		auto d = XMVectorSet(0, -1, 0, 1);
		RaycastHit hit;
		if (Hx::PhysX()->RaycastHit(pos, d, 100.0f, &hit)) {
			auto dot = XMVector3Dot(hit.normal, XMVectorSet(0, 1, 0, 1)).x;
			auto angle = dot;

			auto deg = m_CharacterControllerComponent->GetSlopeLimit();
			float slopeLimit = cosf(XM_PI / 180.0f * deg);
			if (slopeLimit > angle) {
				m_IsGround = false;

				hit.normal.y = 0.0f;
				hit.normal = XMVector3Normalize(hit.normal);
				auto v2 = mJump + hit.normal * speed * 0.2f;
				v2.y = 0.0f;
				auto s = min(max(XMVector3Length(v2).x, -speed), speed);
				v2 = XMVector3Normalize(v2)*s;
				mJump.x = v2.x;
				mJump.z = v2.z;
			}
		}
	}

	auto xy = XMVector2Normalize(XMVectorSet(x, y, 0, 1));
	auto v = XMVectorZero();
	v += xy.y * gameObject->mTransform->Forward();
	v += xy.x * gameObject->mTransform->Left();

	if (m_IsGround) {
		mJump = XMVectorZero();
		if (Input::Trigger(KeyCode::Key_SPACE)) {
			mJump.y += m_JumpPower;
		}
		v *= speed;
	}
	else {

		mJump.x -= mJump.x * 6.0f * time;
		mJump.z -= mJump.z * 6.0f * time;
		auto v2 = mJump + v * speed * 0.1f;
		v2.y = 0.0f;
		auto s = min(max(XMVector3Length(v2).x, -speed), speed);
		v = XMVector3Normalize(v2)*s;

	}

	mJump.x = v.x;
	mJump.z = v.z;

}

void PlayerController::moveUpdate()
{
	float time = Hx::DeltaTime()->GetDeltaTime();
	mJump += mGravity * time;
	auto p = XMVectorZero();
	p += mJump * time;

	m_CharacterControllerComponent->Move(p);

	if (m_IsGround && !m_CharacterControllerComponent->IsGround() && mJump.y <= 0.0f) {
		XMVECTOR donw = XMVectorSet(0, -m_CharacterControllerComponent->GetStepOffset(), 0, 1);
		m_CharacterControllerComponent->Move(donw);
		if (!m_CharacterControllerComponent->IsGround()) {
			m_CharacterControllerComponent->Move(-donw);
		}
	}
}

void PlayerController::rotate()
{
	
	int mx, my;
	Input::MousePosition(&mx, &my);
	auto p = Hx::System()->GetLockCursorPosition();
	float _mx = mx - p.x;
	float _my = my - p.y;
	if (abs(_mx) < 1.1f)_mx = 0.0f;
	if (abs(_my) < 1.1f)_my = 0.0f;
	mRotate.y += _mx / 200.0f;
	mRotate.x += _my / 200.0f;


	float MAX = XM_PI / 2 - 0.1f;
	mRotate.x = min(max(mRotate.x, -MAX), MAX);

	auto left = gameObject->mTransform->Left();

	auto qx = XMQuaternionRotationRollPitchYaw(mRotate.x, 0, 0);
	//auto qx = XMQuaternionRotationAxis(left, mRotate.x);
	//qx = XMQuaternionNormalize(qx);
	auto qy = XMQuaternionRotationRollPitchYaw(0, mRotate.y, 0);
	qx = XMQuaternionMultiply(qx, qy);
	qx = XMQuaternionNormalize(qx);

	gameObject->mTransform->WorldQuaternion(qx);
}

void PlayerController::doge()
{
	if (Input::Trigger(KeyCode::Key_LSHIFT)) {
		
		float x = 0, y = 0;
		if (Input::Down(KeyCode::Key_W)) {
			y = 1.0f;
		}
		if (Input::Down(KeyCode::Key_S)) {
			y = -1.0f;
		}
		if (Input::Down(KeyCode::Key_D)) {
			x = 1.0f;
		}
		if (Input::Down(KeyCode::Key_A)) {
			x = -1.0f;
		}

		if (abs(x) == 0 && abs(y) == 0) {
			return;
		}


		auto v = XMVectorZero();
		v += y * gameObject->mTransform->Forward();
		v += x * gameObject->mTransform->Left();
		v.y = 0.0f;
		v = XMVector3Normalize(v);

		mJump += v * m_MoveSpeed * 2;
		mJump.y += m_JumpPower/2.0f;
		auto p = mJump * Hx::DeltaTime()->GetDeltaTime();
		m_CharacterControllerComponent->Move(p);
		m_IsDoge = true;
	}
}

#include "WeaponHand.h"
void PlayerController::guard()
{
	auto weaponHand = m_WeaponHand->GetScript<WeaponHand>();
	if (weaponHand) {
		weaponHand->Guard();
	}
}

void PlayerController::attack()
{
	auto weaponHand = m_WeaponHand->GetScript<WeaponHand>();
	if (weaponHand) {
		weaponHand->Attack();
	}
}
