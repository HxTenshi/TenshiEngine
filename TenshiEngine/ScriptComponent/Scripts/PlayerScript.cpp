#include "PlayerScript.h"

#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/PhysXComponent.h"
#include "Game/Component/TextComponent.h"
#include "Input/Input.h"
#include "Game/Script/IGame.h"

#include "Script.h"

PlayerScript::PlayerScript(){
	mRotateY = 0.0f;
}
void PlayerScript::Update(){

	auto& list = gameObject->mTransform->Children();
	Actor* camera = NULL;
	for (auto act : list){
		if (act->Name() == "MainCamera"){
			camera = act;
		}
	}

	if (!camera)return;
	float speed = 50.0f;
	if (Input::Down(KeyCoord::Key_A)){
		gameObject->mTransform->AddTorque(camera->mTransform->Forward()*speed);
	}
	if (Input::Down(KeyCoord::Key_D)){
		gameObject->mTransform->AddTorque(camera->mTransform->Forward()*-speed);
	}
	if (Input::Down(KeyCoord::Key_W)){
		gameObject->mTransform->AddTorque(camera->mTransform->Left()*speed);
	}
	if (Input::Down(KeyCoord::Key_S)){
		gameObject->mTransform->AddTorque(camera->mTransform->Left()*-speed);
	}
	if (Input::Down(KeyCoord::Key_Z)){
		gameObject->mTransform->AddForce(camera->mTransform->Forward() * 10);
	}
	if (Input::Down(KeyCoord::Key_SPACE)){

	}

}

void PlayerScript::OnCollide(Actor* target){
	if (target->Name() == "coin"){
		game->DestroyObject(target);

		auto cm = game->FindActor("CoinManager");
		if (!cm)return;
		auto cms = cm->GetScript<CoinManagerScript>();
		if (!cms)return;
		cms->GetCoin();
	}
}