#include "Script.h"
//#include "Game\Game.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/PhysXComponent.h"
#include "Game/Component/TextComponent.h"
#include "Input/Input.h"
#include "MySTL/ptr.h"
#include "Game/Script/IGame.h"

#include <functional>
#include <map>

//#define INITGUID
#include <dinput.h>

#include "System\include"


class FuctorySetter{
public:
	FuctorySetter(){
#define _ADD(x) mFactory[typeid(x).name()] = [](){ return new x(); }
		
		//Ç±Ç±Ç…çÏê¨ÇµÇΩÉNÉâÉXÇí«â¡ÇµÇ‹Ç∑

		_ADD(CameraScript);
		_ADD(CoinScript);
		_ADD(CoinManagerScript);

#include "System\factory"

#undef _ADD
	}
	std::map<std::string, std::function<IDllScriptComponent*()>> mFactory;
};

FuctorySetter mFact;

IDllScriptComponent* CreateInstance(const char* ClassName){
	std::string name = "class ";
	name += ClassName;
	auto func = mFact.mFactory.find(name);
	if (func == mFact.mFactory.end())return NULL;
	return func->second();
}
void ReleseInstance(IDllScriptComponent* p){
	delete p;
}

void CameraScript::Start(){
	mRotateY = 0.0f;
	mLength = 5.0f;
	mClear = false;
}
void CameraScript::Update(){

	if (Input::Down(KeyCoord::Key_Q)){
		mRotateY -= 0.05f;
	}
	if (Input::Down(KeyCoord::Key_E)){
		mRotateY += 0.05f;
	}

	if (mClear){
		mRotateY += 0.01f;
	}

	auto rotate = XMQuaternionInverse(gameObject->mTransform->GetParent()->mTransform->Rotate());
	auto rotatey = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 1), mRotateY);
	gameObject->mTransform->Rotate(XMQuaternionMultiply(rotatey, rotate));
	//rotatey;
	auto back = XMVector3Normalize(gameObject->mTransform->Forward()) * -mLength;
	auto pos = XMVector3Rotate(XMVectorSet(0, 1, 0, 1) + back, rotate);
	gameObject->mTransform->Position(pos);
}

void CameraScript::GameClear(){
	mClear = true;
}



void CoinScript::Start(){
	mRotateY = 0.0f;
}

void CoinScript::Update(){
	mRotateY += 0.01f;
	gameObject->mTransform->Rotate(XMVectorSet(0, mRotateY, 0,1));
}

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

		auto c = game->FindActor("MainCamera");
		if (c){
			auto s = c->GetScript<CameraScript>();
			if (s)
			{
				s->GameClear();
			}
		}
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