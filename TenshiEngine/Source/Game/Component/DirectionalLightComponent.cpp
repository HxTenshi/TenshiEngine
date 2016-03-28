
#include"DirectionalLightComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"

#include "Game/Game.h"

#include "CameraComponent.h"
#include "Game/DeferredRendering.h"

DirectionalLightComponent::DirectionalLightComponent()
	: m_Color(XMFLOAT3(1, 1, 1)){

	mCBChangeLgiht = ConstantBuffer<cbChangesLight>::create(3);

	mCBChangeLgiht.mParam.LightVect = XMFLOAT4(-1.0f, -1.0f, 1.0f, 0.0f);
	mCBChangeLgiht.mParam.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mCBChangeLgiht.UpdateSubresource();


	mModelTexture.Create("EngineResource/TextureModel.tesmesh");

	mMaterial.Create("EngineResource/DeferredLightRendering.fx");

	//mCBChangeLgihtMaterial = ConstantBuffer<cbChangesMaterial>::create(5);
	//
	//mCBChangeLgihtMaterial.mParam.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//mCBChangeLgihtMaterial.mParam.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	//mCBChangeLgihtMaterial.mParam.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//
	//mCBChangeLgihtMaterial.UpdateSubresource();
}
DirectionalLightComponent::~DirectionalLightComponent(){
	mModelTexture.Release();
}

void DirectionalLightComponent::Initialize(){
	SetColor(m_Color);
	mCBChangeLgiht.UpdateSubresource();
}

void DirectionalLightComponent::Finish(){

}
void DirectionalLightComponent::EngineUpdate(){
	Update();
}
void DirectionalLightComponent::Update(){

	auto f = gameObject->mTransform->Forward();
	mCBChangeLgiht.mParam.LightVect = XMFLOAT4(f.x, f.y, f.z, f.w);
	mCBChangeLgiht.UpdateSubresource();

	mCBChangeLgiht.VSSetConstantBuffers();
	mCBChangeLgiht.PSSetConstantBuffers();


	CascadeShadow::SetLightVect(f);

	//{
	//	UINT width = 20;//WindowState::mWidth/20;
	//	UINT height = 20;//WindowState::mHeight/20;
	//	auto Projection = XMMatrixOrthographicLH((FLOAT)width, (FLOAT)height, 0.01f, 100.0f);
	//
	//	mCBChangesLightCamera.mParam.mProjection = XMMatrixTranspose(Projection);
	//
	//	auto cam = Game::GetMainCamera();
	//	auto atf = cam->gameObject->mTransform->Forward();
	//	auto pos = cam->gameObject->mTransform->WorldPosition();
	//
	//	auto At = pos +atf * 10.0f;
	//
	//	auto lf = gameObject->mTransform->Forward();
	//	auto Eye = At - lf * mTest;
	//
	//	auto Up = XMVectorSet(0, 1, 0, 1);
	//	auto View = XMMatrixLookAtLH(Eye, At, Up);
	//
	//	mCBChangesLightCamera.mParam.mView = XMMatrixTranspose(View);
	//
	//}




	//ディレクショナルライト
	Game::AddDrawList(DrawStage::Light, [&](){

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
	});
}
void DirectionalLightComponent::CreateInspector(){


	auto data = Window::CreateInspector();
	std::function<void(float)> collbackx = [&](float f){
		m_Color.x = f;
		SetColor(m_Color);
	};

	std::function<void(float)> collbacky = [&](float f){
		m_Color.y = f;
		SetColor(m_Color);
	};

	std::function<void(float)> collbackz = [&](float f){
		m_Color.z = f;
		SetColor(m_Color);
	};

	Window::AddInspector(new InspectorColorDataSet("Color", &m_Color.x, collbackx, &m_Color.y, collbacky, &m_Color.z, collbackz, NULL, [](float){}), data);

	Window::ViewInspector("DirecionalLight", this, data);
}

void DirectionalLightComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(m_Color.x);
	_KEY(m_Color.y);
	_KEY(m_Color.z);

#undef _KEY
}

void DirectionalLightComponent::SetColor(XMFLOAT3 color){
	m_Color = color;

	mCBChangeLgiht.mParam.LightColor.x = m_Color.x;
	mCBChangeLgiht.mParam.LightColor.y = m_Color.y;
	mCBChangeLgiht.mParam.LightColor.z = m_Color.z;
	mCBChangeLgiht.mParam.LightColor.w = 1;
	mCBChangeLgiht.UpdateSubresource();
}




