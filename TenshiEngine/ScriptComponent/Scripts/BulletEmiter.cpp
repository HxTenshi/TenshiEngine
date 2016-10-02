#include "BulletEmiter.h"

#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Input/Input.h"
#include "Game/Script/IGame.h"

//生成時に呼ばれます（エディター中も呼ばれます）
void BulletEmiter::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void BulletEmiter::Start(){

}

//毎フレーム呼ばれます
void BulletEmiter::Update(){
	if (Input::Trigger(KeyCoord::Key_Z)){
		auto actor = Hx::Instance(mEmitObject.c_str());
		if (!actor)return;
		actor->mTransform->Position(gameObject->mTransform->Position());
		actor->mTransform->Rotate(gameObject->mTransform->Rotate());
	}
}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void BulletEmiter::Finish(){

}

//コライダーとのヒット時に呼ばれます
void BulletEmiter::OnCollideBegin(GameObject target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void BulletEmiter::OnCollideEnter(GameObject target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void BulletEmiter::OnCollideExit(GameObject target){
	(void)target;
}