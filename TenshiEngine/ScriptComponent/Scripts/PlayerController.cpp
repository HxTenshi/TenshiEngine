#include "PlayerController.h"

#include "../h_standard.h"
#include "../h_component.h"

//生成時に呼ばれます（エディター中も呼ばれます）
void PlayerController::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void PlayerController::Start(){
}

//毎フレーム呼ばれます
void PlayerController::Update(){


	float speed = 10;
	float x = 0, y = 0;
	if(Input::Down(KeyCoord::Key_W)){
		y = 1.0f;
	}
	if (Input::Down(KeyCoord::Key_S)){
		y = -1.0f;
	}
	if (Input::Down(KeyCoord::Key_D)){
		x = -1.0f;
	}
	if (Input::Down(KeyCoord::Key_A)){
		x = 1.0f;
	}
	auto xy = XMVector2Normalize(XMVectorSet(x, y, 0, 1)) * speed * game->DeltaTime()->GetDeltaTime();
	auto pos = gameObject->mTransform->WorldPosition();
	pos += xy.y * gameObject->mTransform->Forward();
	pos += xy.x * gameObject->mTransform->Left();
	gameObject->mTransform->WorldPosition(pos);

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void PlayerController::Finish(){

}

//コライダーとのヒット時に呼ばれます
void PlayerController::OnCollideBegin(Actor* target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void PlayerController::OnCollideEnter(Actor* target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void PlayerController::OnCollideExit(Actor* target){
	(void)target;
}