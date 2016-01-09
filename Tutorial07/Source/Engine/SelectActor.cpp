
#include "SelectActor.h"


#include "Device/DirectX11Device.h"

#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Game/Component/ModelComponent.h"
#include "Game/Component/MeshDrawComponent.h"
#include "Window/Window.h"

#include "Input/Input.h"


#include "Graphic/Model/Model.h"

#include "Game/EngineObject/EditorCamera.h"
#include "PhysX/PhysX3.h"


#include "Game/Game.h"

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
		material->LoadAssetResource("EngineResource/Arrow.pmx.txt");
		mComponents.AddComponent<MeshDrawComponent>();
	}
	~Arrow(){
	}

	void Update(float deltaTime)override{
		(void)deltaTime;
	}
private:
};

SelectActor::SelectActor()
	:mSelect(NULL)
{
	mDragBox = -1;

	mVectorBox = new Arrow[3];

	mVectorBox[0].mTransform->Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f));
	mVectorBox[1].mTransform->Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f));
	mVectorBox[2].mTransform->Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f));
	auto mate = mVectorBox[0].GetComponent<MaterialComponent>();
	if (mate){
		auto m = mate->GetMaterial(0);
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
		auto& d = m.mCBMaterial.mParam.Diffuse;
		d.x = 0;
		d.y = 0;
		d.z = 1;
		d.w = 1;
		mate->SetMaterial(0, m);
	}

	mSelectWireMaterial.mDiffuse.x = 0.95f;
	mSelectWireMaterial.mDiffuse.y = 0.6f;
	mSelectWireMaterial.mDiffuse.z = 0.1f;
	mSelectWireMaterial.Create();
}

void SelectActor::Finish(){
	for (int i = 0; i < 3; i++){

		mVectorBox[i].Finish();
	}
	delete[] mVectorBox;
}

void SelectActor::Initialize(){
	for (int i = 0; i < 3; i++){

		//mVectorBox[i].AddComponent<PhysXComponent>();
		//auto phy = mVectorBox[i].GetComponent<PhysXComponent>();
		//phy->mIsEngineMode = true;
		//mVectorBox[i].AddComponent<PhysXColliderComponent>();
		//auto col = mVectorBox[i].GetComponent<PhysXColliderComponent>();
		//col->SetMesh("EngineResource/Arrow.pmx.tesmesh");
		mVectorBox[i].Initialize();
		mVectorBox[i].Start();

		//phy->SetKinematic(true);
	}
}

Actor* SelectActor::GetSelect(){
	return mSelect;
}

void SelectActor::UpdateInspector(){
	if (!mSelect)return;

	static unsigned long time_start = timeGetTime();
	unsigned long current_time = timeGetTime();
	unsigned long b = (current_time - time_start);
	if (b >= 16){
		time_start = timeGetTime();
		//ˆ—‚ª’Ç‚¢‚Â‚©‚È‚¢ê‡‚ª‚ ‚é
		Window::UpdateInspector();
	}
}
void SelectActor::Update(float deltaTime){
	(void)deltaTime;
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

	if (!mSelect)return;

	mVectorBox[0].mTransform->Position(mSelect->mTransform->WorldPosition());
	mVectorBox[1].mTransform->Position(mSelect->mTransform->WorldPosition());
	mVectorBox[2].mTransform->Position(mSelect->mTransform->WorldPosition());

	mVectorBox[0].mTransform->Rotate(XMVectorSet(0, 0, -XM_PI / 2, 1));
	mVectorBox[2].mTransform->Rotate(XMVectorSet(XM_PI / 2, 0, 0, 1));

	//mVectorBox[0].UpdateComponent(deltaTime);
	//mVectorBox[1].UpdateComponent(deltaTime);
	//mVectorBox[2].UpdateComponent(deltaTime);

	static XMVECTOR vect = XMVectorZero();
	if (Input::Down(MouseCoord::Left)){
		int x, y;
		Input::MouseLeftDragVector(&x, &y);
		float p = -y*0.05f;
		vect = mDragPos;

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
			mSelect->mTransform->Position(vect);
			mSelect->mTransform->SetUndo(vect);
		}
		mDragBox = -1;
	}
	if (mDragBox != -1){
		mSelect->mTransform->Position(vect);
	}
	SelectActorDraw();
}
void SelectActor::SelectActorDraw(){
	Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
		auto mModel = mSelect->GetComponent<ModelComponent>();
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
	//Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
	//	auto mModel = mVectorBox[0].GetComponent<ModelComponent>();
	//	if (!mModel)return;
	//	Model& model1 = *mModel->mModel;
	//	mModel = mVectorBox[1].GetComponent<ModelComponent>();
	//	if (!mModel)return;
	//	Model& model2 = *mModel->mModel;
	//	mModel = mVectorBox[2].GetComponent<ModelComponent>();
	//	if (!mModel)return;
	//	Model& model3 = *mModel->mModel;
	//
	//	ID3D11DepthStencilState* pBackDS;
	//	UINT ref;
	//	Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);
	//
	//	D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	//	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//	//descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//	descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
	//	ID3D11DepthStencilState* pDS = NULL;
	//	Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
	//	Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);
	//
	//	D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	//	descRS.CullMode = D3D11_CULL_BACK;
	//	descRS.FillMode = D3D11_FILL_SOLID;
	//
	//	ID3D11RasterizerState* pRS = NULL;
	//	Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);
	//
	//	Device::mpImmediateContext->RSSetState(pRS);
	//
	//
	//	model1.Draw(mVectorBox[0].GetComponent<MaterialComponent>());
	//	model2.Draw(mVectorBox[1].GetComponent<MaterialComponent>());
	//	model3.Draw(mVectorBox[2].GetComponent<MaterialComponent>());
	//
	//
	//	Device::mpImmediateContext->RSSetState(NULL);
	//	if (pRS)pRS->Release();
	//
	//	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
	//	pDS->Release();
	//
	//	Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
	//	if (pBackDS)pBackDS->Release();
	//}));
}

void SelectActor::SetSelect(Actor* select){
	mDragBox = -1;

	if (Game::FindActor(select)){
		mSelect = select;
	}
	else{
		mSelect = NULL;
	}
	Window::ClearInspector();
	if (mSelect)mSelect->CreateInspector();
}


bool SelectActor::ChackHitRay(PhysX3Main* physx, EditorCamera* camera){

	if (!mSelect)return false;

	int x, y;
	Input::MousePosition(&x, &y);
	XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
	XMVECTOR vect = camera->PointRayVector(point);
	XMVECTOR pos = camera->GetPosition();

	auto act = physx->EngineSceneRaycast(pos, vect);
	if (act){
		if (&mVectorBox[0] == act){
			mDragPos = mSelect->mTransform->Position();
			mDragBox = 0;
			return true;
		}
		if (&mVectorBox[1] == act){
			mDragPos = mSelect->mTransform->Position();
			mDragBox = 1;
			return true;
		}
		if (&mVectorBox[2] == act){
			mDragPos = mSelect->mTransform->Position();
			mDragBox = 2;
			return true;
		}
	}
	return false;
}