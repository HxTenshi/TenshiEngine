#include "ThunderLight.h"
#include "Game/Actor.h"
#include "Game/Component/DirectionalLightComponent.h"


//生成時に呼ばれます（エディター中も呼ばれます）
void ThunderLight::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void ThunderLight::Start(){
	auto dir = gameObject->GetComponent<DirectionalLightComponent>();
	if (!dir)return;

	mColor = dir->GetColor();
}

//毎フレーム呼ばれます
void ThunderLight::Update(){
	auto dir = gameObject->GetComponent<DirectionalLightComponent>();
	if (!dir)return;

	float noize = (rand() % 100)/2000.0f;
	XMFLOAT3 noize3(noize, noize, noize);
	if (mThounderPow < 0.2){
		if (rand() % 2000 <= 10){
			mThounderPow = (rand() % 100) / 100.0f*0.5 + 0.5;
		}
	}
	mThounderPow -= 0.1;
	mThounderPow = max(mThounderPow, 0);
	XMFLOAT4 col = mColor;
	col.x += mThounderPow + noize;
	col.y += mThounderPow + noize;
	col.z += mThounderPow + noize;
	dir->SetColor(col);

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void ThunderLight::Finish(){

}

//コライダーとのヒット時に呼ばれます
void ThunderLight::OnCollideBegin(GameObject target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void ThunderLight::OnCollideEnter(GameObject target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void ThunderLight::OnCollideExit(GameObject target){
	(void)target;
}