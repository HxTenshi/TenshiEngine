#include "TextureScroll.h"
#include "Game/Actor.h"
#include "Game/Component/MaterialComponent.h"
#include "Graphic/Material/Material.h"


//生成時に呼ばれます（エディター中も呼ばれます）
void TextureScroll::Initialize(){
}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void TextureScroll::Start(){

}

//毎フレーム呼ばれます
void TextureScroll::Update(){
	auto mate = gameObject->GetComponent<MaterialComponent>();
	if (mate){
		auto material = mate->GetMaterialPtr(0);
		material->mCBMaterial.mParam.MOffset.x += x;
		material->mCBMaterial.mParam.MOffset.y += y;
	}
}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void TextureScroll::Finish(){
	
}

//コライダーとのヒット時に呼ばれます
void TextureScroll::OnCollideBegin(Actor* target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void TextureScroll::OnCollideEnter(Actor* target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void TextureScroll::OnCollideExit(Actor* target){
	(void)target;
}