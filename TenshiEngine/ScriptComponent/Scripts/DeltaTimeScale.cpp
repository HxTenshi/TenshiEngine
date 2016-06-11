#include "DeltaTimeScale.h"
#include "Game/Script/IGame.h"
#include "Game/DeltaTime.h"

DeltaTimeScale::DeltaTimeScale(){
	mDeltaTimeScale = 1.0f;

}

//生成時に呼ばれます（エディター中も呼ばれます）
void DeltaTimeScale::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void DeltaTimeScale::Start(){

}

//毎フレーム呼ばれます
void DeltaTimeScale::Update(){
	game->DeltaTime()->SetTimeScale(mDeltaTimeScale);
}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void DeltaTimeScale::Finish(){

}

//コライダーとのヒット時に呼ばれます
void DeltaTimeScale::OnCollideBegin(Actor* target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void DeltaTimeScale::OnCollideEnter(Actor* target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void DeltaTimeScale::OnCollideExit(Actor* target){
	(void)target;
}