#include "Bullet.h"

#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Input/Input.h"
#include "Game/Script/IGame.h"

//生成時に呼ばれます（エディター中も呼ばれます）
void Bullet::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void Bullet::Start(){

}

//毎フレーム呼ばれます
void Bullet::Update(){

	mDeadTime -= 0.016f;
	if (mDeadTime < 0){
		game->DestroyObject(gameObject);
	}
	auto pos = gameObject->mTransform->Position();
	auto s = gameObject->mTransform->LossyScale();
	auto vec = gameObject->mTransform->Forward() * mSpeed;
	vec.x *= s.x;
	vec.y *= s.y;
	vec.z *= s.z;
	gameObject->mTransform->Position(pos + vec);

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void Bullet::Finish(){

}

//コライダーとのヒット時に呼ばれます
void Bullet::OnCollideBegin(Actor* target){

	game->DestroyObject(gameObject);
}

//コライダーとのヒット中に呼ばれます
void Bullet::OnCollideEnter(Actor* target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void Bullet::OnCollideExit(Actor* target){
	(void)target;
}