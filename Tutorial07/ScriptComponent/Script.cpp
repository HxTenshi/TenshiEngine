#include "Script.h"
#include "MySTL\File.h"
//#include "Game\Game.h"
#include "Game\Actor.h"
#include "Game\TransformComponent.h"
#include "Game\PhysXComponent.h"
#include "Input\Input.h"
#include "MySTL\ptr.h"

//#ifdef _DEBUG
//#pragma comment(lib, "Debug/MyInput.lib")
//#else
//#pragma comment(lib, "Release/MyInput.lib")
//#endif

//#pragma comment( lib , "lib/dinput8.lib" )
//#pragma comment( lib , "lib/dxguid.lib" )


class FuctorySetter{
public:
	FuctorySetter(){
		mFactory[typeid(DllScriptComponent).name()] = [](){ return new DllScriptComponent(); };
		mFactory[typeid(CameraScript).name()] = [](){ return new CameraScript(); };
		mFactory[typeid(PlayerScript).name()] = [](){ return new PlayerScript(); };
	}
	std::map<std::string, std::function<IDllScriptComponent*()>> mFactory;
};

IDllScriptComponent* CreateInstance(const char* ClassName){
	FuctorySetter mFact;
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
void DllScriptComponent::Update(Actor* This){
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
		auto rotate = This->mTransform->Position();
		rotate.x += speed;
		This->mTransform->Position(rotate);
	}
	if (Input::Down(KeyCoord::Key_K)){
		auto rotate = This->mTransform->Position();
		rotate.x -= speed;
		This->mTransform->Position(rotate);
	}

	if (Input::Down(KeyCoord::Key_A)){
		auto rotate = This->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(0, 0, speed));
		This->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_D)){
		auto rotate = This->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(0, 0, -speed));
		This->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_W)){
		auto rotate = This->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(speed, 0, 0));
		This->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_S)){
		auto rotate = This->mTransform->Rotate();
		rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(-speed, 0, 0));
		This->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_U)){
		This->mTransform->AddTorque(XMVectorSet(speed*100, 0, 0,1));
	}
}

CameraScript::CameraScript(){
	mRotateY = 0.0f;
}
void CameraScript::Update(Actor* This){
	//auto pos = par->mTransform->Position();

	if (Input::Down(KeyCoord::Key_Q)){
		mRotateY -= 0.05f;
	}
	if (Input::Down(KeyCoord::Key_E)){
		mRotateY += 0.05f;
	}

	auto rotate = XMQuaternionInverse(This->mTransform->GetParent()->mTransform->Rotate());
	auto rotatey = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 1), mRotateY);
	This->mTransform->Rotate(XMQuaternionMultiply(rotatey, rotate));
	//rotatey;
	auto back = XMVector3Normalize(This->mTransform->Forward()) * -5;
	auto pos = XMVector3Rotate(XMVectorSet(0, 1, 0, 1) + back, rotate);
	This->mTransform->Position(pos);
}

PlayerScript::PlayerScript(){
	mRotateY = 0.0f;
}
void PlayerScript::Update(Actor* This){
	
	auto list = This->mTransform->Children();
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
	float speed = 10.0f;
	if (Input::Down(KeyCoord::Key_A)){
		This->mTransform->AddTorque(camera->mTransform->Forward()*speed);
	}
	if (Input::Down(KeyCoord::Key_D)){
		This->mTransform->AddTorque(camera->mTransform->Forward()*-speed);
	}
	if (Input::Down(KeyCoord::Key_W)){
		This->mTransform->AddTorque(camera->mTransform->Left()*speed);
	}
	if (Input::Down(KeyCoord::Key_S)){
		This->mTransform->AddTorque(camera->mTransform->Left()*-speed);
	}
	//auto wp = ball->mTransform->WorldPosition() + XMVectorSet(0, 0, 0, 1);
	//This->mTransform->Position(wp);

}