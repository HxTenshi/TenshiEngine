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

	float s = 0.05f;
	if (Input::Down(KeyCoord::Key_B)){
	
		gameObject->GetComponent<MaterialComponent>()->SetAlbedoColor(XMFLOAT4(0, 0, 1, 1));
		game->Debug()->Log("log");
	}
	if (Input::Down(KeyCoord::Key_V)){

		volatile float i = 0;
		i = 10.0f / i;
	}
	if (Input::Down(KeyCoord::Key_C)){

		volatile int i = 0;
		i = 10.0f / i;
	}
	//if (mate){
	//	auto m = mate->GetMaterial(0);
	//	m.mDiffuse.x = 1;
	//}
	auto children = gameObject->mTransform->Children();

	weak_ptr<MaterialComponent> testmaterial;

	for (auto& child : children){
		if (child->Name() == "LS"){
			auto tar = *child->mTransform->Children().begin();
			auto p = Input::Analog(PAD_DS4_Velo2Coord::Velo2_LStick);
			p *= 0.5;
			p += XMVectorSet(0, 0, -0.2f,0);
			tar->mTransform->Position(p);
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_L3)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "RS"){
			auto tar = *child->mTransform->Children().begin();
			auto p = Input::Analog(PAD_DS4_Velo2Coord::Velo2_RStick);
			p *= 0.5;
			p += XMVectorSet(0, 0, -0.2f, 0);
			tar->mTransform->Position(p);
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_R3)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}

		}
		if (child->Name() == "R1"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_R1)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "L1"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_L1)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "L2"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			auto l = Input::Analog(PAD_DS4_LevelCoord::Level_L2);
			if (Input::Down(PAD_DS4_KeyCoord::Button_L2)){
				mate->SetAlbedoColor(XMFLOAT4(1, l.x, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.0, l.x, 0.0, 1));
			}
		}
		if (child->Name() == "R2"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			auto l = Input::Analog(PAD_DS4_LevelCoord::Level_R2);
			if (Input::Down(PAD_DS4_KeyCoord::Button_R2)){
				mate->SetAlbedoColor(XMFLOAT4(1, l.x, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.0, l.x, 0.0, 1));
			}
		}
		if (child->Name() == "TRI"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_TRIANGLE)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "SQR"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_SQUARE)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "X"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_CROSS)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "CIR"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_CIRCLE)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "OPS"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_OPTIONS)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "SHR"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_SHARE)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "UP"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_UP)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "LEFT"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_LEFT)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "RIGHT"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_RIGHT)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "DOWN"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_DOWN)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "PS"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_PS)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}


			testmaterial = mate;
		}
		if (child->Name() == "TOUCH"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_TOUCHPAD)){
				mate->SetAlbedoColor(XMFLOAT4(0.5, 0.5, 0.5, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.2, 0.2, 0.2, 1));
			}
		}
		if (child->Name() == "1TOUCH"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_1stTOUCH)){
				mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.5, 0.0, 0.0, 1));
			}

			auto p = Input::Analog(PAD_DS4_Velo2Coord::Velo2_1stTouch);
			p -= XMVectorSet(0.5, 0.5, 0.5, 0);
			tar->mTransform->Position(p);
		}
		if (child->Name() == "2TOUCH"){
			auto tar = *child->mTransform->Children().begin();
			auto mate = tar->GetComponent<MaterialComponent>();
			if (Input::Down(PAD_DS4_KeyCoord::Button_2ndTOUCH)){
				mate->SetAlbedoColor(XMFLOAT4(0, 1, 0, 1));
			}
			else{
				mate->SetAlbedoColor(XMFLOAT4(0.0, 0.5, 0.0, 1));
			}
			auto p = Input::Analog(PAD_DS4_Velo2Coord::Velo2_2ndTouch);
			p -= XMVectorSet(0.5, 0.5, 0.5, 0);
			tar->mTransform->Position(p);
		}


		XMVECTOR accelQuat;
		{
			auto p = Input::Analog(PAD_DS4_Velo3Coord::Velo3_Acceleration);
			auto pback = p;
			p.z = pback.y;
			p.y = -pback.z;
			p.x = pback.x;
			p = XMVector3Normalize(p);

			auto l = XMVector3Length(pback);
			float g = l.x * 0x7fff;
			g /= 100000.0f;
			if (g<9.81f){
				bool flag = false;

				auto radius = XMVector3AngleBetweenNormals(XMVectorSet(0, 0, 1, 1), p);
				auto cross = XMVector3Cross(XMVectorSet(0, 0, 1, 1), p);
				if (XMVector3Equal(cross, XMVectorZero())){

				}else
				if (XMVector3IsInfinite(cross)){
					
				}
				else{

					auto q = XMQuaternionRotationAxis(cross, radius.x);
					accelQuat = q;

					auto rotate = gameObject->mTransform->Rotate();
					if (abs(XMVector3Dot(p, XMVectorSet(1, 0, 0, 1)).x) > 0.95){
						//rotate.y = accelQuat.y;
						//rotate.z = accelQuat.z;
						//rotate.w = accelQuat.w;
					}
					else
						if (abs(XMVector3Dot(p, XMVectorSet(0, 1, 0, 1)).x) > 0.95){
						//rotate.y = 0;
						//rotate.x = (XMVector3Dot(p, XMVectorSet(0, 1, 0, 1)).x >0 ) ? -0.5f:0.5f;
						//rotate.z = 0;
						//rotate.z = accelQuat.z;
						}
						else
							if (abs(XMVector3Dot(p, XMVectorSet(0, 0, 1, 1)).x) > 0.95){
						//rotate.x = accelQuat.x;
						//rotate.y = accelQuat.y;
						//rotate.w = accelQuat.w;
							}
							else{
								flag = false;
							}

							if (flag){
								if (testmaterial)
									testmaterial->SetAlbedoColor(XMFLOAT4(1, 1, 1, 1));
								rotate = XMQuaternionNormalize(rotate);
								gameObject->mTransform->Rotate(rotate);
								//gameObject->mTransform->Rotate(accelQuat);
							}
				}
			}

		}

		XMVECTOR jyroQuat;
		{
			auto p = Input::Analog(PAD_DS4_Velo3Coord::Velo3_Angular) / 40.0f;
			auto pback = p;
			p.y = pback.z;
			p.z = pback.y;
			p.x = -pback.x;
			p = XMQuaternionRotationRollPitchYawFromVector(p);
			jyroQuat = p;
			
		}
		auto rot = gameObject->mTransform->Quaternion();
		rot = XMQuaternionMultiply(jyroQuat, rot);
		rot = XMQuaternionNormalize(rot);
		//rot.x = 0.0f;
		//rot.y = 0.0f;
		//rot.z = 0.0f;
		//rot.w = 1.0f;
		gameObject->mTransform->Quaternion(rot);

		//auto r = Input::Analog(PAD_DS4_Velo3Coord::Velo3_Angular);
		//
		//gameObject->mTransform->Rotate(XMQuaternionRotationRollPitchYawFromVector(r*3.14));
	}
}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void testhit::Finish(){

}

//コライダーとのヒット時に呼ばれます
void testhit::OnCollideBegin(Actor* target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void testhit::OnCollideEnter(Actor* target){
	auto mate = gameObject->GetComponent<MaterialComponent>();
	mate->SetAlbedoColor(XMFLOAT4(1, 0, 0, 1));
}

//コライダーとのロスト時に呼ばれます
void testhit::OnCollideExit(Actor* target){
	(void)target;
}