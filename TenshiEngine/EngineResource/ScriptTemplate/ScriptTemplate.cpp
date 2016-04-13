//#include "$temp$.h"
#include "ScriptTemplate.h"


//生成時に呼ばれます（エディター中も呼ばれます）
void $temp$::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中は呼ばれません）
void $temp$::Start(){

}

//毎フレーム呼ばれます
void $temp$::Update(){

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）
void $temp$::Finish(){

}

//コライダーとのヒット時に呼ばれます
void $temp$::OnCollide(Actor* target){
	(void)target;
}