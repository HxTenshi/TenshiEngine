#include "Component.h"


decltype(ComponentFactory::mFactoryComponents) ComponentFactory::mFactoryComponents;
bool ComponentFactory::mIsInit = false;

#include "Game.h"
void MeshDrawComponent::Update(){

	if (!mModel){
		mModel = gameObject->GetComponent<ModelComponent>();
		if (!mModel)
			mModel = gameObject->GetComponent<TextureModelComponent>();
		if (!mModel)return;
	}
	if (!mModel->mModel)return;

	if (!mMaterial){
		mMaterial = gameObject->GetComponent<MaterialComponent>();
	}
	if (!mMaterial)return;

	Game::AddDrawList(DrawStage::Diffuse, std::function<void()>([&](){
		mModel->SetMatrix();
		Model& model = *mModel->mModel;

		if (mWriteDepth){
			model.Draw(mMaterial);
		}
		else{
			NoWriteDepth(model);
		}
	}));

}

void ModelComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorLabelDataSet("Model"), data);
	std::function<void(std::string)> collbackpath = [&](std::string name){
		mFileName = name;
		if (mModel){
			mModel->Release();
			delete mModel;
		}
		mModel = new Model();
		//mModel->Create(mFileName.c_str(), shared_ptr<MaterialComponent>());
		shared_ptr<MaterialComponent> material = shared_ptr<MaterialComponent>(new MaterialComponent());
		mModel->Create(mFileName.c_str());
		//gameObject->RemoveComponent<MaterialComponent>();
		//gameObject->AddComponent<MaterialComponent>(material);

	};
	Window::AddInspector(new InspectorStringDataSet("Model", &mFileName, collbackpath), data);
	Window::ViewInspector("Model",data);
}

void CameraComponent::Update(){
	XMVECTOR null;
	auto lossy = gameObject->mTransform->LossyScale();
	auto scale = gameObject->mTransform->Scale();
	gameObject->mTransform->Scale(XMVectorSet(lossy.x * scale.x, lossy.y * scale.y, lossy.z * scale.z, 1));

	mView = XMMatrixInverse(&null, gameObject->mTransform->GetMatrix());
	mCBNeverChanges.mParam.mView = XMMatrixTranspose(mView);
	mCBNeverChanges.mParam.mViewInv = XMMatrixTranspose(gameObject->mTransform->GetMatrix());
	mCBNeverChanges.UpdateSubresource();
	Game::SetMainCamera(this);
}

void PostEffectComponent::Initialize(){
	mRenderTarget.Create(WindowState::mWidth, WindowState::mHeight);
	mModelTexture.Create("");

	mMaterial.Create("PostEffect.fx");
	mMaterial.SetTexture(Game::GetMainViewRenderTarget().GetTexture(), 0);

	mMaterialEnd.Create("PostEffect.fx");
	mMaterialEnd.SetTexture(mRenderTarget.GetTexture(), 0);

}
void PostEffectComponent::Finish(){
	mRenderTarget.Release();
	mModelTexture.Release();
}
void PostEffectComponent::PostDraw(){


	mModelTexture.Update();

	Game::AddDrawList(DrawStage::PostEffect, [&](){

		mRenderTarget.ClearView();
		mRenderTarget.SetRendererTarget();

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
		//descRS.CullMode = D3D11_CULL_BACK;
		descRS.CullMode = D3D11_CULL_NONE;
		descRS.FillMode = D3D11_FILL_SOLID;

		ID3D11RasterizerState* pRS = NULL;
		Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

		Device::mpImmediateContext->RSSetState(pRS);

		mModelTexture.Draw(mMaterial);

		Game::GetMainViewRenderTarget().ClearView();
		Game::GetMainViewRenderTarget().SetRendererTarget();
		//Device::mRenderTargetBack->SetRendererTarget();

		mModelTexture.Draw(mMaterialEnd);


		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS_tex->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();

		//const RenderTarget* r[1] = { NULL };
		//RenderTarget::SetRendererTarget((UINT)1, r[0], NULL);
		//Device::mRenderTargetBack->SetRendererTarget();
		//Device::mpImmediateContext->CopyResource(Device::mRenderTargetBack->GetTexture2D(), mRenderTarget.GetTexture2D());



	});
}

void PointLightComponent::Update(){

	Game::AddDrawList(DrawStage::Light, [&](){
		auto ccom = Game::GetMainCamera();
		if (!ccom)return;

		auto campos = ccom->gameObject->mTransform->WorldPosition();
		auto lpos = gameObject->mTransform->WorldPosition();
		float lenSq = XMVector3LengthSq(campos - lpos).x;
		//ポイントライトの中にカメラが有る
		bool isEnter = (m_Radius*m_Radius) >= lenSq;

		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		descDS.DepthFunc = D3D11_COMPARISON_LESS;
		if (isEnter){
			descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		}

		ID3D11DepthStencilState* pDS = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);

		Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);


		ID3D11RasterizerState* pRS = NULL;
		ID3D11RasterizerState* pRSBack = NULL;
		if (isEnter){
			Device::mpImmediateContext->RSGetState(&pRSBack);

			D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
			descRS.CullMode = D3D11_CULL_FRONT;
			descRS.FillMode = D3D11_FILL_SOLID;
			Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);
			Device::mpImmediateContext->RSSetState(pRS);
		}



		auto m = gameObject->mTransform->GetMatrix();
		auto lossy = gameObject->mTransform->LossyScale();
		//auto scale = gameObject->mTransform->Scale();
		float r = m_Radius * 2;
		auto s = XMVectorSet(lossy.x*r, lossy.y*r, lossy.z*r, 1);
		mModel.mWorld = XMMatrixMultiply(XMMatrixScalingFromVector(s),m);
		mModel.Update();


		auto world = gameObject->mTransform->WorldPosition();
		auto matv = ccom->GetViewMatrix();
		auto view = XMVector3Transform(world, matv);


		mPointLightBuffer.mParam.ViewPosition = XMFLOAT4(view.x, view.y, view.z, 1);
		mPointLightBuffer.mParam.Color = XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, 1);

		float n = m_AttenuationParam;//任意の値(≠0)　減衰曲線を制御
		float rmin = m_AttenuationStart;// 減衰開始距離　減衰率 = 1
		float rmax = m_Radius;// 影響範囲　減衰率 = 0
		float _rmin = pow(rmin, n);
		float _rmax = pow(rmax, n);
		float A = -1 / (_rmax - _rmin);
		float B = _rmax / (_rmax - _rmin);

		mPointLightBuffer.mParam.Param = XMFLOAT4(A, B, m_Radius, m_AttenuationParam);
		mPointLightBuffer.UpdateSubresource();
		mPointLightBuffer.PSSetConstantBuffers();


		mModel.Draw(mMaterial);


		if (isEnter){
			Device::mpImmediateContext->RSSetState(pRSBack);
			if (pRS)pRS->Release();
			if (pRSBack)pRSBack->Release();
		}

		pDS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	});
}