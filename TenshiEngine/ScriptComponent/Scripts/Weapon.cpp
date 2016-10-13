#include "Weapon.h"


//生成時に呼ばれます（エディター中も呼ばれます）
void Weapon::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void Weapon::Start(){

}

//毎フレーム呼ばれます
void Weapon::Update(){

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void Weapon::Finish(){

}

//コライダーとのヒット時に呼ばれます
void Weapon::OnCollideBegin(GameObject target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void Weapon::OnCollideEnter(GameObject target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void Weapon::OnCollideExit(GameObject target){
	(void)target;
}