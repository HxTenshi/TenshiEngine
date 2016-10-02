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
	GameObject camera = NULL;
	for (auto act : list){
		if (act->Name() == "MainCamera"){
			camera = act;
		}
	}

	if (!camera)return;
	float speed = 50.0f;
	if (Input::Down(PAD_DS4_KeyCoord::Button_UP)){
		//gameObject->mTransform->AddTorque(camera->mTransform->Forward()*speed);
	}

	if (Input::Down(KeyCoord::Key_RIGHT)){
		gameObject->mTransform->AddTorque(camera->mTransform->Forward()*-speed);
	}
	if (Input::Down(KeyCoord::Key_UP)){
		gameObject->mTransform->AddTorque(camera->mTransform->Left()*speed);
	}
	if (Input::Down(KeyCoord::Key_DOWN)){
		gameObject->mTransform->AddTorque(camera->mTransform->Left()*-speed);
	}
	if (Input::Down(KeyCoord::Key_Z)){
		gameObject->mTransform->AddForce(camera->mTransform->Forward() * 10);
	}
	if (Input::Down(KeyCoord::Key_SPACE)){

	}

}

void PlayerScript::OnCollideBegin(GameObject target){

}