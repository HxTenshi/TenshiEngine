#include "testhit.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Graphic/Material/Material.h"
#include "Input/Input.h"
#include "Game/Script/IGame.h"

#include "PhysX/IPhysXEngine.h"
#include "engine/DebugEngine.h"

//生成時に呼ばれます（エディター中も呼ばれます）
void testhit::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void testhit::Start(){

}

//毎フレーム呼ばれます
void testhit::Update(){

}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void testhit::Finish(){

}

//コライダーとのヒット時に呼ばれます
void testhit::OnCollideBegin(GameObject target){

	auto mate = target->GetComponent<MaterialComponent>();
	if (!mate)return;
	auto m = mate->mAlbedo;
	m.z=0;
	m.x=0;
	mate->SetAlbedoColor(m);

}

//コライダーとのヒット中に呼ばれます
void testhit::OnCollideEnter(GameObject target){
}

//コライダーとのロスト時に呼ばれます
void testhit::OnCollideExit(GameObject target){
	auto mate = target->GetComponent<MaterialComponent>();
	if (!mate)return;
	auto m = mate->mAlbedo;
	m.z=1;
	m.x=1;
	mate->SetAlbedoColor(m);


}