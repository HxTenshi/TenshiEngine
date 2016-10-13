#include "WeaponHand.h"
#include "h_standard.h"

//生成時に呼ばれます（エディター中も呼ばれます）
void WeaponHand::Initialize(){
	m_AtackTime = 0.0f;
	m_IsGuard = false;
}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void WeaponHand::Start(){

}

//毎フレーム呼ばれます
void WeaponHand::Update(){

	if (m_AtackTime != 0.0f) {
		m_AtackTime -= Hx::DeltaTime()->GetDeltaTime();

		mWeapon->mTransform->Rotate(XMVectorSet(m_AtackTime,0,0,1));
	}

	if (m_IsGuard) {
		if(m_GuardPos)
			mWeapon->mTransform->SetParent(m_GuardPos);
	}
	else {
		mWeapon->mTransform->SetParent(gameObject);
	}

	m_IsGuard = false;
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

void WeaponHand::Atack()
{
	m_AtackTime = 1.0f;
}

void WeaponHand::Guard()
{
	m_IsGuard = true;

}
