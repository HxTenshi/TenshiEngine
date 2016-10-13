#include "WeaponHand.h"


//生成時に呼ばれます（エディター中も呼ばれます）
void WeaponHand::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void WeaponHand::Start(){

}

//毎フレーム呼ばれます
void WeaponHand::Update(){

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void WeaponHand::Finish(){

}

//コライダーとのヒット時に呼ばれます
void WeaponHand::OnCollideBegin(GameObject target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void WeaponHand::OnCollideEnter(GameObject target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void WeaponHand::OnCollideExit(GameObject target){
	(void)target;
}

void WeaponHand::SetWeapon(GameObject weapon)
{
	if (mWeapon) {
		Hx::DestroyObject(mWeapon);
	}
	mWeapon = weapon;
	mWeapon->mTransform->SetParent(gameObject);
	mWeapon->mTransform->Position(XMVectorZero());
	mWeapon->mTransform->Rotate(XMVectorZero());
	mWeapon->RemoveComponent<PhysXComponent>();
}
