
#include"DirectionalLightComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"

#include "Game/Game.h"

#include "CameraComponent.h"
#include "Game/DeferredRendering.h"

#include "Game/RenderingSystem.h"

#include "Engine/Inspector.h"

DirectionalLightComponent::DirectionalLightComponent()
	: m_Color(XMFLOAT4(1, 1, 1, 1))
	, m_HDR(1.0f){

	mCBChangeLgiht = ConstantBuffer<cbChangesLight>::create(3);

	mCBChangeLgiht.mParam.LightVect = XMFLOAT4(-1.0f, -1.0f, 1.0f, 0.0f);
	mCBChangeLgiht.mParam.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);



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
}

void DirectionalLightComponent::Finish(){

}
#ifdef _ENGINE_MODE
void DirectionalLightComponent::EngineUpdate(){
	Update();
}
#endif
void DirectionalLightComponent::Update(){

	auto f = gameObject->mTransform->Forward();
	mCBChangeLgiht.mParam.LightVect = XMFLOAT4(f.x, f.y, f.z, f.w);

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


	Game::AddDrawList(DrawStage::Init, [&](){
		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		mCBChangeLgiht.UpdateSubresource(render->m_Context);
		mCBChangeLgiht.VSSetConstantBuffers(render->m_Context);
		mCBChangeLgiht.PSSetConstantBuffers(render->m_Context);
	});

	//ディレクショナルライト
	Game::AddDrawList(DrawStage::Light, [&](){

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		mModelTexture.Update();

		render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
		render->PushSet(Rasterizer::Preset::RS_None_Solid);

		mModelTexture.Draw(render->m_Context, mMaterial);

		render->PopRS();
		render->PopDS();
	});
}
#ifdef _ENGINE_MODE
void DirectionalLightComponent::CreateInspector(){

	Inspector ins("DirectionalLight",this);
	ins.AddEnableButton(this);
	std::function<void(Color)> collback = [&](Color f){
		m_Color.x = f.r;
		m_Color.y = f.g;
		m_Color.z = f.b;
		m_Color.w = f.a;
		SetColor(m_Color);
	};

	auto col = Color(m_Color);

	ins.Add("Color", &col, collback);
	ins.Add("HDR", &m_HDR, [&](float f){
		m_HDR = f;
		SetColor(m_Color); });
	ins.Complete();
}
#endif

void DirectionalLightComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(m_Color.x);
	_KEY(m_Color.y);
	_KEY(m_Color.z);
	_KEY(m_HDR);

#undef _KEY
}

void DirectionalLightComponent::SetColor(XMFLOAT4 color){
	m_Color = color;

	mCBChangeLgiht.mParam.LightColor.x = m_Color.x*m_HDR;
	mCBChangeLgiht.mParam.LightColor.y = m_Color.y*m_HDR;
	mCBChangeLgiht.mParam.LightColor.z = m_Color.z*m_HDR;
	mCBChangeLgiht.mParam.LightColor.w = m_Color.w;
}




