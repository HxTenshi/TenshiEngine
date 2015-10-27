#include "Script.h"
#include "MySTL\File.h"
//#include "Game\Game.h"
#include "Game\Actor.h"
#include "Game\TransformComponent.h"
#include "Game\PhysXComponent.h"
#include "Input\Input.h"
#include "MySTL\ptr.h"

#ifdef _DEBUG
#pragma comment(lib, "Debug/MyInput.lib")
#else
#pragma comment(lib, "Release/MyInput.lib")
#endif

#pragma comment( lib , "lib/dinput8.lib" )
#pragma comment( lib , "lib/dxguid.lib" )

IDllScriptComponent* CreateInstance(){
	return new DllScriptComponent();
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

	if (Input::Down(KeyCoord::Key_A)){
		auto rotate = This->mTransform->Rotate();
		rotate.z += speed;
		This->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_D)){
		auto rotate = This->mTransform->Rotate();
		rotate.z -= speed;
		This->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_W)){
		auto rotate = This->mTransform->Rotate();
		rotate.x += speed;
		This->mTransform->Rotate(rotate);
	}
	if (Input::Down(KeyCoord::Key_S)){
		auto rotate = This->mTransform->Rotate();
		rotate.x -= speed;
		This->mTransform->Rotate(rotate);
	}
}