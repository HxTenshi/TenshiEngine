#include "Script.h"
#include "MySTL\File.h"
//#include "Game\Game.h"
#include "Game\Actor.h"
#include "Game\TransformComponent.h"
#include "Game\PhysXComponent.h"
#include "Input\Input.h"
#include "MySTL\ptr.h"
#include "Game\IGame.h"

#include <functional>
#include <map>

//#define INITGUID
#include <dinput.h>


class FuctorySetter{
public:
	FuctorySetter(){
		mFactory[typeid(DllScriptComponent).name()] = [](){ return new DllScriptComponent(); };
		mFactory[typeid(CameraScript).name()] = [](){ return new CameraScript(); };
		mFactory[typeid(PlayerScript).name()] = [](){ return new PlayerScript(); };
		mFactory[typeid(CoinScript).name()] = [](){ return new CoinScript(); };
		mFactory[typeid(CoinManagerScript).name()] = [](){ return new CoinManagerScript(); };
	}
	std::map<std::string, std::function<IDllScriptComponent*()>> mFactory;
};

FuctorySetter mFact;

IDllScriptComponent* CreateInstance(const char* ClassName){
	//return new DllScriptComponent();
	std::string name = "class ";
	name += ClassName;
	auto func = mFact.mFactory.find(name);
	if (func == mFact.mFactory.end())return NULL;
	return func->second();
}
void ReleseInstance(IDllScriptComponent* p){
	delete p;
}

DllScriptComponent::DllScriptComponent(){
	timer = 0;
	timer2 = 0;
}
void DllScriptComponent::Update(){
	timer++;
	timer = timer % 30;
	if (timer2 != 0){
		timer2++;
		if (timer2 == 60){
			timer2 = 0;
		}
	}
	//if (Input::Down(KeyCoord::Key_SPACE)){
	//	if (timer2 == 0){
	//
	//		timer2++;
	//	}
	//}
	float speed = 0.1f;
	//if (Input::Down(KeyCoord::Key_LSHIFT)){
	//	speed = 1.0f;
	//}
	//if (Input::Down(KeyCoord::Key_W)){
	//	This->mTransform->Position(This->mTransform->Position() + XMVectorSet(0.0f, 0.0f, -speed, 0.0f));
	//}
	//if (Input::Down(KeyCoord::Key_A)){
	//	This->mTransform->Position(This->mTransform->Position() + XMVectorSet(speed, 0.0f, 0.0f, 0.0f));
	//}
	//if (Input::Down(KeyCoord::Key_S)){
	//	This->mTransform->Position(This->mTransform->Position() + XMVectorSet(0.0f, 0.0f, speed, 0.0f));
	//}
	//if (Input::Down(KeyCoord::Key_D)){
	//	This->mTransform->Position(This->mTransform->Position() + XMVectorSet(-speed, 0.0f, 0.0f, 0.0f));
	//}
	//This->mTransform->Position(This->mTransform->Position() + XMVectorSet(0.0f, sin(timer2 / 60.0f*3.14f) - This->mTransform->Position().y, 0.0f, 0.0f));
	//

	//if (Input::Down(KeyCoord::Key_M))This->mTransform->Position(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	//
	//if (Input::Down(KeyCoord::Key_A)){
	//	This->mTransform->AddForce(XMVectorSet(speed, 0.0f, 0.0f, 0.0f));
	//}
	//if (Input::Down(KeyCoord::Key_D)){
	//	This->mTransform->AddForce(XMVectorSet(-speed, 0.0f, 0.0f, 0.0f));
	//}
	//if (Input::Down(KeyCoord::Key_T)){
	//	This->mTransform->AddForce(XMVectorSet(0.0f, speed * 4, 0.0f, 0.0f));
	//}
	if (Input::Down(KeyCoord::Key_J)){
		auto rotate = gameObject->mTransform->Position();
		rotate.x += speed;
		gameObject->mTransform->Position(rotate);
	}
	if (Input::Down(KeyCoord::Key_K)){
		auto rotate = gameObject->mTransform->Position();
		rotate.x -= speed;
		gameObject->mTransform->Position(rotate);
	}

	if (Input::Down(KeyCoord::Key_A)){
		auto rotate = gameObject->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(0, 0, speed));
		gameObject->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_D)){
		auto rotate = gameObject->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(0, 0, -speed));
		gameObject->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_W)){
		auto rotate = gameObject->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(speed, 0, 0));
		gameObject->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_S)){
		auto rotate = gameObject->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(-speed, 0, 0));
		gameObject->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_U)){
		gameObject->mTransform->AddTorque(XMVectorSet(speed * 100, 0, 0, 1));
	}
}
void CameraScript::Start(){
	mRotateY = 0.0f;
	mLength = 5.0f;
}
void CameraScript::Update(){
	//auto pos = par->mTransform->Position();

	if (Input::Down(KeyCoord::Key_Q)){
		mRotateY -= 0.05f;
	}
	if (Input::Down(KeyCoord::Key_E)){
		mRotateY += 0.05f;
	}

	auto rotate = XMQuaternionInverse(gameObject->mTransform->GetParent()->mTransform->Rotate());
	auto rotatey = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 1), mRotateY);
	gameObject->mTransform->Rotate(XMQuaternionMultiply(rotatey, rotate));
	//rotatey;
	auto back = XMVector3Normalize(gameObject->mTransform->Forward()) * -mLength;
	auto pos = XMVector3Rotate(XMVectorSet(0, 1, 0, 1) + back, rotate);
	gameObject->mTransform->Position(pos);
}

PlayerScript::PlayerScript(){
	mRotateY = 0.0f;
}
void PlayerScript::Update(){
	
	auto list = gameObject->mTransform->Children();
	Actor* camera = NULL;
	for (auto act : list){
		if (act->Name() == "MainCamera"){
			camera = act;
		}
	}

	//if (Input::Down(KeyCoord::Key_Q)){
	//	mRotateY += 0.01f;
	//}
	//if (Input::Down(KeyCoord::Key_E)){
	//	mRotateY -= 0.01f;
	//}
	//This->mTransform->Rotate(XMVectorSet(0, mRotateY, 0, 1));
	//auto ball = This;
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
		gameObject->mTransform->AddForce(camera->mTransform->Up()*100);
	}
	if (Input::Down(KeyCoord::Key_SPACE)){
		//auto a = game->CreateActor("Assets/coin.prefab");
		//game->AddObject(a);
		auto coin = game->FindActor("coin");

		if (coin){

		auto scr = coin->GetScript<PlayerScript>();
		if (scr){
			scr->Start();

			game->DestroyObject(coin);
		}
		}

	}
	//auto wp = ball->mTransform->WorldPosition() + XMVectorSet(0, 0, 0, 1);
	//This->mTransform->Position(wp);

}

void PlayerScript::OnCollide(Actor* target){
	if (target->Name() == "coin"){
		game->DestroyObject(target);

		auto gm = game->FindActor("CoinManager");
		if (!gm)return;
		auto gms = gm->GetScript<CoinManagerScript>();
		if (!gms)return;
		gms->GetCoin();
	}
}

void CoinScript::Start(){
	mRotateY = 0.0f;
}

void CoinScript::Update(){
	mRotateY += 0.01f;
	gameObject->mTransform->Rotate(XMVectorSet(0, mRotateY, 0,1));
}


#include "Game\TextComponent.h"

void CoinManagerScript::Start(){
	mGetCoinCount = 0;
	mMaxCoin = 0;
	isClear = false;

	auto obj = game->FindActor("GetCoinText");
	if (!obj)return;
	auto text = obj->GetComponent<TextComponent>();
	if (!text)return;
	text->ChangeText("GetCoin:0");
}
void CoinManagerScript::Update(){
	mMaxCoin = gameObject->mTransform->Children().size();

	if (mMaxCoin == 0 && !isClear){
		auto a = game->CreateActor("Assets/gameclear.prefab");
		if (a)game->AddObject(a);
		isClear = true;
	}

}


void CoinManagerScript::GetCoin(){
	mGetCoinCount++;

	auto obj = game->FindActor("GetCoinText");
	if (!obj)return;
	auto text = obj->GetComponent<TextComponent>();
	if (!text)return;
	text->ChangeText("GetCoin:"+std::to_string(mGetCoinCount));
}