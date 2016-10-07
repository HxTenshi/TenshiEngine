#include "TPSCamera.h"

TPSCamera::TPSCamera(){
	mDistance = 5.0f;
	mUp = 1.0f;
	mTarget = NULL;
}

//生成時に呼ばれます（エディター中も呼ばれます）
void TPSCamera::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void TPSCamera::Start(){
}

//毎フレーム呼ばれます
void TPSCamera::Update(){

	if (mTarget){
		auto pos = mTarget->mTransform->WorldPosition();
		auto back = -mTarget->mTransform->Forward();
		pos.y += mUp;
		auto campos = pos + back * mDistance;
		gameObject->mTransform->WorldPosition(campos);
		if (XMVector3Equal(pos, campos)){

			pos += mTarget->mTransform->Forward();
			auto rotate = XMMatrixTranspose(XMMatrixLookAtLH(campos, pos, XMVectorSet(0, 1, 0, 1)));
			auto q = XMQuaternionRotationMatrix(rotate);
			gameObject->mTransform->WorldQuaternion(q);
		}
		else{
			auto rotate = XMMatrixTranspose(XMMatrixLookAtLH(campos, pos, XMVectorSet(0, 1, 0, 1)));
			auto q = XMQuaternionRotationMatrix(rotate);
			gameObject->mTransform->WorldQuaternion(q);
		}
	}

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void TPSCamera::Finish(){

}

//コライダーとのヒット時に呼ばれます
void TPSCamera::OnCollideBegin(GameObject target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void TPSCamera::OnCollideEnter(GameObject target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void TPSCamera::OnCollideExit(GameObject target){
	(void)target;
}