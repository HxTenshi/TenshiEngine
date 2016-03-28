
#include "SelectActor.h"

#include <set>

#include "Device/DirectX11Device.h"

#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Game/Component/ModelComponent.h"
#include "Game/Component/MeshDrawComponent.h"
#include "Game/Component/PhysXColliderComponent.h"
#include "Game/Component/CameraComponent.h"

#include "Window/Window.h"

#include "Input/Input.h"


#include "Graphic/Model/Model.h"

#include "Game/EngineObject/EditorCamera.h"
#include "PhysX/PhysX3.h"


#include "Game/Game.h"

//選択時の移動オブジェクト
class Arrow : public Actor{
public:

	Arrow(){
		Name("arrow");
		mTransform = make_shared<TransformComponent>();
		mComponents.AddComponent<TransformComponent>(mTransform);
		auto model = make_shared<ModelComponent>();
		model->mFileName = "EngineResource/Arrow.pmx.tesmesh";
		mComponents.AddComponent<ModelComponent>(model);
		auto material = make_shared<MaterialComponent>();
		mComponents.AddComponent<MaterialComponent>(material);

		//mComponents.AddComponent<MeshDrawComponent>();

		mComponents.AddComponent<PhysXComponent>();
		auto phy = mComponents.GetComponent<PhysXComponent>();
		phy->mIsEngineMode = true;
		mComponents.AddComponent<PhysXColliderComponent>();
		auto col = mComponents.GetComponent<PhysXColliderComponent>();
		col->SetMesh("EngineResource/Arrow.pmx.tesmesh");

		phy->SetKinematic(true);
	}
	~Arrow(){
	}

	void Update(float deltaTime)override{
		(void)deltaTime;
	}
private:
};

Selects::Selects(){
	mLastVect = XMVectorZero();
}
Selects::~Selects(){
	CopyDelete();
}
void Selects::OneSelect(Actor* act){
	mSelects.clear();
	if (act){
		mSelects.push_back(act);
	}
}
void Selects::TriggerSelect(Actor* act){
	//ヌルは無視
	if (!act)return;
	int num = (int)mSelects.size();
	mSelects.remove(act);
	//削除されていれば
	if (num != (int)mSelects.size()){
		return;
	}
	//削除されなければ追加
	mSelects.push_back(act);
}
void Selects::MoveStart(XMVECTOR& vect){
	mLastVect = vect;
}
void Selects::MovePos(XMVECTOR& vect){

	auto move = vect - mLastVect;
	mLastVect = vect;

	for (auto& act : mSelects){
		auto pos = act->mTransform->Position();
		act->mTransform->Position(pos + move);
	}

}
void Selects::MoveEnd(XMVECTOR& vect){
	auto move = vect - mLastVect;
	mLastVect = vect;
	for (auto& act : mSelects){
		auto pos = act->mTransform->Position();
		act->mTransform->Position(pos + move);
		act->mTransform->SetUndo(pos + move);
	}

}

void Selects::Copy(){
	CopyDelete();
	for (auto& act : mSelects){
		auto data = new picojson::value();
		act->ExportData(*data);

		mCopy.push_back(data);
	}
}
void Selects::CopyDelete(){
	for (auto act : mCopy){
		delete act;
	}
	mCopy.clear();
}
void Selects::Paste(){
	for (auto& act : mCopy){
		auto postactor = new Actor();
		postactor->ImportDataAndNewID(*act);
		Game::AddObject(postactor);
	}
}

int Selects::SelectNum(){
	return (int)mSelects.size();
}
XMVECTOR Selects::GetPosition(){
	XMVECTOR pos = XMVectorZero();
	for (auto& act : mSelects){
		pos += act->mTransform->WorldPosition();
	}
	int num = (int)mSelects.size();
	if (num){
		pos = pos / (FLOAT)num;
	}
	return pos;
}

Actor* Selects::GetSelectOne(){
	int num = (int)mSelects.size();
	if (num){
		return *mSelects.begin();
	}
	return NULL;
}
std::list<Actor*>& Selects::GetSelects(){
	return mSelects;
}

SelectActor::SelectActor()
{
	mDragBox = -1;

	mVectorBox = NULL;

	mSelectWireMaterial.mDiffuse.x = 0.95f;
	mSelectWireMaterial.mDiffuse.y = 0.6f;
	mSelectWireMaterial.mDiffuse.z = 0.1f;
	mSelectWireMaterial.Create();
}

void SelectActor::Finish(){
	if (mVectorBox){
		for (int i = 0; i < 3; i++){

			mVectorBox[i].Finish();
		}
		delete[] mVectorBox;
		mVectorBox = NULL;
	}
}

void SelectActor::Initialize(){
	if (mVectorBox == NULL){
		mVectorBox = new Arrow[3];

		float s = 0.1f;

		for (int i = 0; i < 3; i++){
			Game::AddEngineObject(&mVectorBox[i]);

			mVectorBox[i].mTransform->Scale(XMVectorSet(s,s,s, 1));

		}

		mVectorBox[0].mTransform->Rotate(XMVectorSet(0, 0, -XM_PI / 2, 1));
		mVectorBox[1].mTransform->Rotate(XMVectorSet(0, 0, 0, 1));
		mVectorBox[2].mTransform->Rotate(XMVectorSet(XM_PI / 2, 0, 0, 1));

		auto mate = mVectorBox[0].GetComponent<MaterialComponent>();
		if (mate){
			auto m = mate->GetMaterial(0);

			m.Create("EngineResource/NoLighting.fx");
			auto& d = m.mCBMaterial.mParam.Diffuse;
			d.x = 1;
			d.y = 0;
			d.z = 0;
			d.w = 1;

			mate->SetMaterial(0, m);
		}
		mate = mVectorBox[1].GetComponent<MaterialComponent>();
		if (mate){
			auto m = mate->GetMaterial(0);
			m.Create("EngineResource/NoLighting.fx");
			auto& d = m.mCBMaterial.mParam.Diffuse;
			d.x = 0;
			d.y = 1;
			d.z = 0;
			d.w = 1;
			mate->SetMaterial(0, m);
		}
		mate = mVectorBox[2].GetComponent<MaterialComponent>();
		if (mate){
			auto m = mate->GetMaterial(0);
			m.Create("EngineResource/NoLighting.fx");
			auto& d = m.mCBMaterial.mParam.Diffuse;
			d.x = 0;
			d.y = 0;
			d.z = 1;
			d.w = 1;
			mate->SetMaterial(0, m);
		}
	}
}

Actor* SelectActor::GetSelectOne(){
	return mSelects.SelectNum()==1?mSelects.GetSelectOne():NULL;
}

void SelectActor::UpdateInspector(){
	if (mSelects.SelectNum() != 1)return;

	static unsigned long time_start = timeGetTime();
	unsigned long current_time = timeGetTime();
	unsigned long b = (current_time - time_start);
	if (b >= 16){
		time_start = timeGetTime();
		//処理が追いつかない場合がある
		Window::UpdateInspector();
	}
}
void SelectActor::Update(float deltaTime){
	//(void)deltaTime;
	//if (Window::GetTreeViewWindow()->IsActive()){
	//	void* s = (Actor*)Window::GetTreeViewWindow()->GetSelectItem();
	//	
	//	if (s){
	//		if (s != mTreeSelect){
	//			mTreeSelect = (Actor*)s;
	//			mSelect = mTreeSelect;
	//		}
	//	}
	//}

	if (!mSelects.SelectNum())return;

	if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_C)){
		mSelects.Copy();
	}
	if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_V)){
		mSelects.Paste();
	}


	auto tar = mSelects.GetPosition();
	mVectorBox[0].mTransform->Position(tar);
	mVectorBox[1].mTransform->Position(tar);
	mVectorBox[2].mTransform->Position(tar);

	auto cam = Game::GetMainCamera();
	if (cam){
		auto pos = cam->gameObject->mTransform->WorldPosition();
		auto l = XMVector3Length(tar - pos).x/100.0f;
		mVectorBox[0].mTransform->Scale(XMVectorSet(l, l, l, 1));
		mVectorBox[1].mTransform->Scale(XMVectorSet(l, l, l, 1));
		mVectorBox[2].mTransform->Scale(XMVectorSet(l, l, l, 1));
	}

	mVectorBox[0].UpdateComponent(deltaTime);
	mVectorBox[1].UpdateComponent(deltaTime);
	mVectorBox[2].UpdateComponent(deltaTime);

	static XMVECTOR vect = XMVectorZero();
	if (Input::Down(MouseCoord::Left)){
		int x, y;
		Input::MouseLeftDragVector(&x, &y);
		//float l = XMVector2Length(XMVectorSet(x,-y,0,1)).x;
		float p = -y *0.05f;
		vect = mDragPos;
		if (Input::Down(KeyCoord::Key_LSHIFT)){
			p += 0.5f;
			p = (float)(int)p;
		}

		if (mDragBox == 0){
			vect += XMVectorSet(p, 0.0f, 0.0f, 0.0f);
		}
		if (mDragBox == 1){
			vect += XMVectorSet(0.0f, p, 0.0f, 0.0f);
		}
		if (mDragBox == 2){
			vect += XMVectorSet(0.0f, 0.0f, p, 0.0f);
		}
	}

	if (Input::Up(MouseCoord::Left)){
		if (mDragBox != -1){
			mSelects.MoveEnd(vect);
		}
		mDragBox = -1;
	}
	if (mDragBox != -1){
		mSelects.MovePos(vect);
	}
	SelectActorDraw();
}
void SelectActor::SelectActorDraw(){
	for (auto select : mSelects.GetSelects()){
		Game::AddDrawList(DrawStage::Engine, std::function<void()>([&, select](){
			if (!select)return;
			auto mModel = select->GetComponent<ModelComponent>();
			if (!mModel)return;
			Model& model = *mModel->mModel;

			ID3D11DepthStencilState* pBackDS;
			UINT ref;
			Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

			D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
			descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			//descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
			ID3D11DepthStencilState* pDS = NULL;
			Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
			Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);

			D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
			descRS.CullMode = D3D11_CULL_BACK;
			descRS.FillMode = D3D11_FILL_WIREFRAME;

			ID3D11RasterizerState* pRS = NULL;
			Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

			Device::mpImmediateContext->RSSetState(pRS);


			model.Draw(mSelectWireMaterial);


			Device::mpImmediateContext->RSSetState(NULL);
			if (pRS)pRS->Release();

			Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
			pDS->Release();

			Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
			if (pBackDS)pBackDS->Release();
		}));
	}
	Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
	
		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);
	
		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		//descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		ID3D11DepthStencilState* pDS = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
		Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);
	
		D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		descRS.CullMode = D3D11_CULL_BACK;
		descRS.FillMode = D3D11_FILL_SOLID;
	
		ID3D11RasterizerState* pRS = NULL;
		Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);
	
		Device::mpImmediateContext->RSSetState(pRS);
	

		auto f = Game::GetMainCamera()->gameObject->mTransform->Forward();

		auto func = [](const std::pair<float, std::function<void()>>& p1, const std::pair<float, std::function<void()>>& p2)
		{
			return p1.first > p2.first;
		};
		std::set<std::pair<float, std::function<void()>>, std::function<bool(const std::pair<float, std::function<void()>>&, const std::pair<float, std::function<void()>>&)> > draws(func);


		for (int _i = 0; _i < 3; _i++){
			int i = _i;

			auto mModel = mVectorBox[i].GetComponent<ModelComponent>();
			Model& model = *mModel->mModel;

			auto au = mVectorBox[i].mTransform->Up();
			auto dot = XMVector3Dot(au, f);

			auto result = draws.insert(std::pair<float, std::function<void()>>((float)dot.x, [=](){
				mModel->SetMatrix();
				model.Draw(mVectorBox[i].GetComponent<MaterialComponent>());
			}));
			if (!result.second){
				dot.x -= 0.01f;
				draws.insert(std::pair<float, std::function<void()>>((float)dot.x, [=](){
					mModel->SetMatrix();
					model.Draw(mVectorBox[i].GetComponent<MaterialComponent>());
				}));
			}
		}
		//Game::GetMainCamera()->SetOrthographic();

		for (auto & d : draws){
			d.second();
		}

		//Game::GetMainCamera()->SetPerspective();

		//model1.Draw(mVectorBox[0].GetComponent<MaterialComponent>());
		//model2.Draw(mVectorBox[1].GetComponent<MaterialComponent>());
		//model3.Draw(mVectorBox[2].GetComponent<MaterialComponent>());
	
	
		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();
	
		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS->Release();
	
		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	}));
}

void SelectActor::SetSelect(Actor* select){
	mDragBox = -1;

	//存在しなければNULL
	if (!Game::FindActor(select)){
		select = NULL;
	}
	if (Input::Down(KeyCoord::Key_LCONTROL)){
		mSelects.TriggerSelect(select);
	}
	else{
		mSelects.OneSelect(select);
	}

	Window::ClearInspector();
	if (mSelects.SelectNum() == 1)mSelects.GetSelectOne()->CreateInspector();
}

void SelectActor::ReCreateInspector(){
	Window::ClearInspector();
	if (mSelects.SelectNum() == 1)mSelects.GetSelectOne()->CreateInspector();
}


bool SelectActor::ChackHitRay(PhysX3Main* physx, EditorCamera* camera){

	if (!mSelects.SelectNum())return false;

	int x, y;
	Input::MousePosition(&x, &y);
	XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
	XMVECTOR vect = camera->PointRayVector(point);
	XMVECTOR pos = camera->GetPosition();

	auto act = physx->EngineSceneRaycast(pos, vect);
	if (act){
		if (&mVectorBox[0] == act){
			mDragPos = mSelects.GetPosition();
			mSelects.MoveStart(mDragPos);
			mDragBox = 0;
			return true;
		}
		if (&mVectorBox[1] == act){
			mDragPos = mSelects.GetPosition();
			mSelects.MoveStart(mDragPos);
			mDragBox = 1;
			return true;
		}
		if (&mVectorBox[2] == act){
			mDragPos = mSelects.GetPosition();
			mSelects.MoveStart(mDragPos);
			mDragBox = 2;
			return true;
		}
	}
	return false;
}