#pragma once

#include <map>
#include "Device/DirectX11Device.h"
#include "Graphic/RenderTarget/RenderTarget.h"

#include "Input/Input.h"
#include "Game/Component/Component.h"
#include "Actor.h"

#include "Sound/Sound.h"

#include "../PhysX/PhysX3.h"
#include "Engine/ICommand.h"

#include "Scene.h"





#include "Engine/SelectActor.h"

#include "Game/EngineObject/FPSChecker.h"
#include "Game/EngineObject/EditorCamera.h"
#include "Engine/WorldGrid.h"


class CameraComponent;

enum class DrawStage{
	Diffuse,
	Depth,
	Normal,
	Light,
	PostEffect,
	Engine,
	UI,
	Count
};
typedef void(*DrawFunc)();


class PostEffectRendering{
public:
	~PostEffectRendering();
	void Initialize();

	void Rendering(){
		Device::mRenderTargetBack->SetRendererTarget();

		mModelTexture.Update();

		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		ID3D11DepthStencilState* pDS_tex = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
		Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);


		D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		descRS.CullMode = D3D11_CULL_NONE;
		descRS.FillMode = D3D11_FILL_SOLID;

		ID3D11RasterizerState* pRS = NULL;
		Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

		Device::mpImmediateContext->RSSetState(pRS);

		mModelTexture.Draw(mMaterial);

		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS_tex->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	}

private:
	Model mModelTexture;
	Material mMaterial;
};

class DeferredRendering{
public:
	~DeferredRendering(){
		m_AlbedoRT.Release();
		m_NormalRT.Release();
		m_DepthRT.Release();
		m_LightRT.Release();
		mModelTexture.Release();

		pBlendState->Release();
	}
	void Initialize(){
		auto w = WindowState::mWidth;
		auto h = WindowState::mHeight;
		m_AlbedoRT.Create(w, h);
		m_NormalRT.Create(w, h);
		m_DepthRT.Create(w, h);
		m_LightRT.Create(w, h);

		mModelTexture.Create("EngineResource/TextureModel.tesmesh");


		mMaterialLight.Create("EngineResource/DeferredLightRendering.fx");
		mMaterialLight.SetTexture(m_NormalRT.GetTexture(), 0);
		mMaterialLight.SetTexture(m_DepthRT.GetTexture(), 1);

		mMaterialDeferred.Create("EngineResource/DeferredRendering.fx");
		mMaterialDeferred.SetTexture(m_AlbedoRT.GetTexture(), 0);
		mMaterialDeferred.SetTexture(m_LightRT.GetTexture(), 1);

		//ブレンドモード設定

		D3D11_BLEND_DESC BlendDesc;
		ZeroMemory(&BlendDesc, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable = FALSE;

		// TRUEの場合、マルチレンダーターゲットで各レンダーターゲットのブレンドステートの設定を個別に設定できる
		// FALSEの場合、0番目のみが使用される
		BlendDesc.IndependentBlendEnable = FALSE;

		//加算合成設定
		D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;
		RenderTarget.BlendEnable = TRUE;
		RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		RenderTarget.DestBlend = D3D11_BLEND_ONE;
		RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		BlendDesc.RenderTarget[0] = RenderTarget;

		Device::mpd3dDevice->CreateBlendState(&BlendDesc, &pBlendState);


	}
	void Start_G_Buffer_Rendering(){

		m_AlbedoRT.ClearView();
		m_NormalRT.ClearView();
		m_DepthRT.ClearView();

		const RenderTarget* r[3] = { &m_AlbedoRT, &m_NormalRT, &m_DepthRT };
		RenderTarget::SetRendererTarget((UINT)3, r[0], Device::mRenderTargetBack);
	}
	void Start_Light_Rendering(){

		m_LightRT.ClearView();

		const RenderTarget* r[1] = { &m_LightRT };
		RenderTarget::SetRendererTarget((UINT)1, r[0], Device::mRenderTargetBack);


		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		Device::mpImmediateContext->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);

		//ディレクショナルライト
		{

			mModelTexture.Update();

			ID3D11DepthStencilState* pBackDS;
			UINT ref;
			Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

			D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
			descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
			ID3D11DepthStencilState* pDS_tex = NULL;
			Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
			Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);


			D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
			descRS.CullMode = D3D11_CULL_NONE;
			descRS.FillMode = D3D11_FILL_SOLID;

			ID3D11RasterizerState* pRS = NULL;
			Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

			Device::mpImmediateContext->RSSetState(pRS);

			mModelTexture.Draw(mMaterialLight);

			Device::mpImmediateContext->RSSetState(NULL);
			if (pRS)pRS->Release();

			Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
			pDS_tex->Release();

			Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
			if (pBackDS)pBackDS->Release();
		}
	}

	void End_Light_Rendering(){

		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		Device::mpImmediateContext->OMSetBlendState(NULL, blendFactor, 0xffffffff);
	}
	void Start_Deferred_Rendering(RenderTarget* rt){

		rt->SetRendererTarget();

		mModelTexture.Update();

		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		ID3D11DepthStencilState* pDS_tex = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
		Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);


		D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		descRS.CullMode = D3D11_CULL_NONE;
		descRS.FillMode = D3D11_FILL_SOLID;

		ID3D11RasterizerState* pRS = NULL;
		Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

		Device::mpImmediateContext->RSSetState(pRS);

		mModelTexture.Draw(mMaterialDeferred);

		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS_tex->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	}

private:
	RenderTarget m_AlbedoRT;
	RenderTarget m_NormalRT;
	RenderTarget m_DepthRT;
	RenderTarget m_LightRT;
	Model mModelTexture;
	Material mMaterialLight;
	Material mMaterialDeferred;

	ID3D11BlendState* pBlendState = NULL;
};

class Game{
public:
	Game();
	~Game();

	static void AddObject(Actor* actor);
	static void DestroyObject(Actor* actor);
	static void ActorMoveStage();
	static PxRigidActor* CreateRigitBody();
	static PxRigidActor* CreateRigitBodyEngine();
	static PhysX3Main* GetPhysX();
	static void RemovePhysXActor(PxActor* act);
	static void RemovePhysXActorEngine(PxActor* act);

	static Actor* FindActor(Actor* actor);
	static Actor* FindNameActor(const char* name);
	static Actor* FindUID(UINT uid);
	static void AddDrawList(DrawStage stage, std::function<void()> func);
	static void SetUndo(ICommand* command);
	static void SetMainCamera(CameraComponent* Camera);
	static CameraComponent* GetMainCamera();
	static RenderTarget GetMainViewRenderTarget();

	void ChangePlayGame(bool isPlay);
	void SaveScene();


	float GetDeltaTime(){
		float deltaTime;
		if (Device::mDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			deltaTime = (float)XM_PI * 0.0125f;
		}
		else
		{
			static float t = 0.0f;
			static float bt = 0.0f;
			static DWORD dwTimeStart = 0;
			DWORD dwTimeCur = GetTickCount();
			if (dwTimeStart == 0)
				dwTimeStart = dwTimeCur;
			bt = t;
			t = (dwTimeCur - dwTimeStart) / 1000.0f;
			deltaTime = t - bt;
		}
		return deltaTime;
	}

	void Update(){
		if (mIsPlay){
			GamePlay();
		}
		else{
			GameStop();
		}
		mFPS.Update(1);
		mSelectActor.UpdateInspector();
	}
	void GameStop(){
		float deltaTime = GetDeltaTime();

		mCamera.Update(deltaTime);

		//if (Input::Trigger(KeyCoord::Key_C)){
		//	int x, y;
		//	Input::MousePosition(&x, &y);
		//	XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
		//	point = mCamera.ScreenToWorldPoint(point);
		//
		//	AddObject(new Box(point));
		//}
		//if (Input::Trigger(KeyCoord::Key_X)){
		//	int x, y;
		//	Input::MousePosition(&x, &y);
		//	XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
		//	point = mCamera.ScreenToWorldPoint(point);
		//	auto act = (new Box(point));
		//	AddObject(act);
		//	auto par = mSelectActor.GetSelect();
		//	if (par)act->mTransform->SetParent(par);
		//}

		if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_Z)){
			mCommandManager.Undo();
		}

		if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_Y)){
			mCommandManager.Redo();
		}


		mSelectActor.Update(deltaTime);
		if (Input::Trigger(MouseCoord::Left)){

			if (!mSelectActor.ChackHitRay(mPhysX3Main, &mCamera)){

				int x, y;
				Input::MousePosition(&x, &y);
				XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
				XMVECTOR vect = mCamera.PointRayVector(point);
				XMVECTOR pos = mCamera.GetPosition();

				auto act = mPhysX3Main->Raycast(pos, vect);
				if (act){
					mSelectActor.SetSelect(act);
				}

				//mSelectActor.ChackHitRay(pos, vect);
				//
				//for (auto& p : mList){
				//	if (p.second->ChackHitRay(pos, vect)){
				//		mSelectActor.SetSelect(p.second);
				//	}
				//}
			}
		}

		mRootObject->DrawOnlyUpdateComponent(deltaTime);

		Game::AddDrawList(DrawStage::Engine, [&](){
			mWorldGrid.Draw();
		});


	}
	void GamePlay(){
		float deltaTime = GetDeltaTime();

		mRootObject->UpdateComponent(deltaTime);
		mPhysX3Main->Display();
	}

	void Draw();

	void ClearDrawList(){
		for (auto &list : mDrawList){
			list.second.clear();
		}
	}
	void PlayDrawList(DrawStage Stage){
		auto &list = mDrawList[Stage];
		for (auto &p : list){
			p();
		}
	}

private:
	Game(const Game&);
	Game operator = (Game&);

	enum class ActorMove{
		Create,
		Delete,
		Count,
	};
	//ツリービューのアイテム削除に失敗したアクター
	std::list<Actor*> mTreeViewItem_ErrerClearList;
	//追加と削除
	std::queue<std::pair<ActorMove, Actor*>> mActorMoveList;
	//ゲームプレイ中のリスト
	std::map<UINT, Actor*> mGamePlayList;
	//ゲームプレイ前のリスト
	std::map<UINT, Actor*> mList;
	//パラメータ保存用
	std::map<UINT, Actor*> mListBack;
	std::map<DrawStage, std::list<std::function<void()>>> mDrawList;
	static Actor* mRootObject;

	SelectActor mSelectActor;

	EditorCamera mCamera;

	WorldGrid mWorldGrid;

	SoundPlayer mSoundPlayer;


	PhysX3Main* mPhysX3Main;

	CommandManager mCommandManager;

	RenderTarget mMainViewRenderTarget;
	CameraComponent* mMainCamera;
	bool mIsPlay;

	DeferredRendering m_DeferredRendering;
	PostEffectRendering mPostEffectRendering;

	FPSChecker mFPS;

	Scene m_Scene;

};
#include "IGame.h"
class SGame : public IGame{
public:
	Actor* CreateActor(const char* prefab)override{
		auto a = new Actor();
		if (!a->ImportDataAndNewID(prefab)){
			delete a;
			return NULL;
		}
		return a;
	}

	Actor* FindActor(const char* name)override{
		return Game::FindNameActor(name);
	}
	void AddObject(Actor* actor) override{
		Game::AddObject(actor);
	}
	void DestroyObject(Actor* actor) override{
		Game::DestroyObject(actor);

	}
};
