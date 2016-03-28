
#include"PointLightComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"

#include "Game/Game.h"

#include "CameraComponent.h"

PointLightComponent::PointLightComponent()
	: m_Radius(1)
	, m_AttenuationStart(0)
	, m_AttenuationParam(1)
	, m_Color(XMFLOAT3(1, 1, 1)){

	mPointLightBuffer = ConstantBuffer<cbChangesPointLight>::create(8);
	mModel.Create("EngineResource/ball.tesmesh");
	mMaterial.Create("EngineResource/DeferredPointLightRendering.fx");

}
PointLightComponent::~PointLightComponent(){
	mModel.Release();
}

void PointLightComponent::Initialize(){
}

void PointLightComponent::Finish(){

}
void PointLightComponent::EngineUpdate(){
	Update();
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
		mModel.mWorld = XMMatrixMultiply(XMMatrixScalingFromVector(s), m);
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
void PointLightComponent::CreateInspector(){


	auto data = Window::CreateInspector();
	std::function<void(float)> collbackx = [&](float f){
		m_Color.x = f;
	};

	std::function<void(float)> collbacky = [&](float f){
		m_Color.y = f;
	};

	std::function<void(float)> collbackz = [&](float f){
		m_Color.z = f;
	};

	std::function<void(float)> collbackr = [&](float f){
		m_Radius = f;
	};
	std::function<void(float)> collbackas = [&](float f){
		m_AttenuationStart = f;
	};
	std::function<void(float)> collbackap = [&](float f){
		m_AttenuationParam = f;
	};

	Window::AddInspector(new TemplateInspectorDataSet<float>("radius", &m_Radius, collbackr), data);
	Window::AddInspector(new TemplateInspectorDataSet<float>("AttenuationStart", &m_AttenuationStart, collbackas), data);
	Window::AddInspector(new TemplateInspectorDataSet<float>("AttenuationParam", &m_AttenuationParam, collbackap), data);
	Window::AddInspector(new InspectorColorDataSet("Color", &m_Color.x, collbackx, &m_Color.y, collbacky, &m_Color.z, collbackz, NULL, [](float){}), data);

	//Window::AddInspector(new InspectorSlideBarDataSet("r", 0.0f, 1.0f, &m_Color.x, collbackx), data);
	//Window::AddInspector(new InspectorSlideBarDataSet("g", 0.0f, 1.0f, &m_Color.y, collbacky), data);
	//Window::AddInspector(new InspectorSlideBarDataSet("b", 0.0f, 1.0f, &m_Color.z, collbackz), data);
	Window::ViewInspector("PointLight", this, data);
}

void PointLightComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(m_Radius);
	_KEY(m_AttenuationStart);
	_KEY(m_AttenuationParam);
	_KEY(m_Color.x);
	_KEY(m_Color.y);
	_KEY(m_Color.z);

#undef _KEY
}