#include "PlayerController.h"

#include "../h_standard.h"
#include "../h_component.h"
#include "Game/Component/CharacterControllerComponent.h"

//生成時に呼ばれます（エディター中も呼ばれます）
void PlayerController::Initialize(){
	mJump = XMVectorZero();
	mGravity = XMVectorSet(0, -9.81f, 0,1);
	mRotate = XMVectorZero();
}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void PlayerController::Start(){

	Hx::System()->LockCursorPositionToWindowCenter(true);

}

//毎フレーム呼ばれます
void PlayerController::Update(){

	if (Input::Down(KeyCoord::Key_ESCAPE)) {
		Hx::System()->LockCursorPositionToWindowCenter(false);
	}
	if (Input::Down(MouseCoord::Left)) {
		Hx::System()->LockCursorPositionToWindowCenter(true);
	}

	auto cc = gameObject->GetComponent<CharacterControllerComponent>();
	if (!cc)return;
	
	float time = Hx::DeltaTime()->GetDeltaTime();
	
	float speed = 10.0f;
	float x = 0, y = 0;
	if (Input::Down(KeyCoord::Key_W) || Input::Down(PAD_X_KeyCoord::Button_UP)){
		y = 1.0f;
	}
	if (Input::Down(KeyCoord::Key_S)){
		y = -1.0f;
	}
	if (Input::Down(KeyCoord::Key_D)){
		x = 1.0f;
	}
	if (Input::Down(KeyCoord::Key_A)){
		x = -1.0f;
	}

	
	bool isGround = cc->IsGround();

	XMVECTOR pos = gameObject->mTransform->WorldPosition();
	if (isGround){
		auto d = XMVectorSet(0, -1, 0, 1);
		RaycastHit hit;
		if (Hx::PhysX()->RaycastHit(pos, d, 100.0f, &hit)){
			auto dot = XMVector3Dot(hit.normal,XMVectorSet(0,1,0,1)).x;
			auto angle = dot;

			auto deg = cc->GetSlopeLimit();
			float slopeLimit = cosf(XM_PI / 180.0f * deg);
			if (slopeLimit > angle){
				isGround = false;

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

	if (isGround){
		mJump = XMVectorZero();
		if (Input::Trigger(KeyCoord::Key_SPACE) || Input::Down(PAD_X_KeyCoord::Button_A)){
  			mJump.y += 3.5f;
		}
		v *= speed;
	}
	else{

		mJump.x -= mJump.x * 6.0f * time;
		mJump.z -= mJump.z * 6.0f * time;
		auto v2 = mJump + v * speed * 0.1f;
		v2.y = 0.0f;
		auto s = min(max(XMVector3Length(v2).x, -speed), speed);
		v = XMVector3Normalize(v2)*s;

	}

	mJump.x = v.x;
	mJump.z = v.z;
	mJump += mGravity * time;
	
	auto p = XMVectorZero();
	p += mJump * time;
	
	cc->Move(p);

	if (isGround && !cc->IsGround() && mJump.y <= 0.0f){
		XMVECTOR donw = XMVectorSet(0, -cc->GetStepOffset(), 0, 1);
		cc->Move(donw);
		if (!cc->IsGround()){
			cc->Move(-donw);
		}
	}
	
	
	if (false){
		float rotY = 0.0f;
		float rotX = 0.0f;
		if (Input::Down(KeyCoord::Key_Q)){
			rotY = -1.0f;
		}
		if (Input::Down(KeyCoord::Key_E)){
			rotY = 1.0f;
		}
		rotY *= Hx::DeltaTime()->GetDeltaTime();

		XMVECTOR normal = XMVectorSet(0, 1, 0, 1);
		if (isGround){
			//auto pos = gameObject->mTransform->WorldPosition();
			auto d = XMVectorSet(0, -1, 0, 1);
			RaycastHit hit;
			if (Hx::PhysX()->RaycastHit(pos, d, 5.0f, &hit)){
				normal = hit.normal;
				normal = XMVector3Normalize(normal);
			}
		}

		{
			auto up = gameObject->mTransform->Up();
			up = XMVector3Normalize(up);
			auto dot = XMVector3Dot(normal, up).x;
			auto angle = acos(dot);
			auto n = XMVector3Cross(up, normal);
			n = XMVector3Normalize(n);
			n.w = 1.0f;
			angle = min(angle, XM_PI * time);
			auto q = XMQuaternionRotationNormal(n, angle);
			auto wq = gameObject->mTransform->WorldQuaternion();
			q = XMQuaternionMultiply(wq, q);
			q = XMQuaternionNormalize(q);
			if (XMQuaternionIsInfinite(q) || XMQuaternionIsNaN(q)){
				return;
			}


			auto rotateQ = XMQuaternionRotationRollPitchYaw(rotX, rotY, 0);
			q = XMQuaternionMultiply(rotateQ, q);
			gameObject->mTransform->WorldQuaternion(q);
		}
	}
	else{

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
			}


		float MAX = XM_PI / 2 - 0.1f;
		mRotate.x = min(max(mRotate.x, -MAX),MAX);

		auto left = gameObject->mTransform->Left();

		auto qx = XMQuaternionRotationRollPitchYaw(mRotate.x, 0, 0);
		//auto qx = XMQuaternionRotationAxis(left, mRotate.x);
		//qx = XMQuaternionNormalize(qx);
		auto qy = XMQuaternionRotationRollPitchYaw(0, mRotate.y, 0);
		qx = XMQuaternionMultiply(qx, qy);
		qx = XMQuaternionNormalize(qx);
		
		gameObject->mTransform->WorldQuaternion(qx);
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