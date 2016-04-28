#include "$temp$.h"


//生成時に呼ばれます（エディター中も呼ばれます）
void $temp$::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void $temp$::Start(){

}

//毎フレーム呼ばれます
void $temp$::Update(){

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void $temp$::Finish(){

}

//コライダーとのヒット時に呼ばれます
void $temp$::OnCollideBegin(Actor* target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void $temp$::OnCollideEnter(Actor* target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void $temp$::OnCollideExit(Actor* target){
	(void)target;
}