
#include"PointLightComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"

#include "Game/Game.h"

#include "CameraComponent.h"

#include "Game/RenderingSystem.h"

#include "Engine/Inspector.h"

PointLightComponent::PointLightComponent()
	: m_Radius(1)
	, m_AttenuationStart(0)
	, m_AttenuationParam(1)
	, m_Color(XMFLOAT4(1, 1, 1,1))
	, m_HDR(1.0f){

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

#ifdef _ENGINE_MODE
void PointLightComponent::EngineUpdate(){
	Update();
}
#endif
void PointLightComponent::Update(){

	Game::AddDrawList(DrawStage::Light, [&](){
		auto ccom = Game::GetMainCamera();
		if (!ccom)return;

		auto campos = ccom->gameObject->mTransform->WorldPosition();
		auto lpos = gameObject->mTransform->WorldPosition();
		float lenSq = XMVector3LengthSq(campos - lpos).x;
		//�|�C���g���C�g�̒��ɃJ�������L��
		bool isEnter = (m_Radius*m_Radius) >= lenSq;


		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		if (isEnter){
			render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
			render->PushSet(Rasterizer::Preset::RS_Front_Solid);
		}
		else{
			render->PushSet(DepthStencil::Preset::DS_Zero_Less);

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
		mPointLightBuffer.mParam.Color = XMFLOAT4(m_Color.x*m_HDR, m_Color.y*m_HDR, m_Color.z*m_HDR, 1);

		float n = m_AttenuationParam;//�C�ӂ̒l(��0)�@�����Ȑ��𐧌�
		float rmin = m_AttenuationStart;// �����J�n�����@������ = 1
		float rmax = m_Radius;// �e���͈́@������ = 0
		float _rmin = pow(rmin, n);
		float _rmax = pow(rmax, n);
		float A = -1 / (_rmax - _rmin);
		float B = _rmax / (_rmax - _rmin);

		mPointLightBuffer.mParam.Param = XMFLOAT4(A, B, m_Radius, m_AttenuationParam);
		mPointLightBuffer.UpdateSubresource(render->m_Context);
		mPointLightBuffer.PSSetConstantBuffers(render->m_Context);


		mModel.Draw(render->m_Context, mMaterial);


		if (isEnter){
			render->PopRS();
		}

		render->PopDS();

	});
}

#ifdef _ENGINE_MODE
void PointLightComponent::CreateInspector(){


	std::function<void(Color)> collback = [&](Color f){
		m_Color.x = f.r;
		m_Color.y = f.g;
		m_Color.z = f.b;
		m_Color.w = f.a;
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
	Inspector ins("PointLight", this);
	ins.AddEnableButton(this);

	ins.Add("radius", &m_Radius, collbackr);
	ins.Add("AttenuationStart", &m_AttenuationStart, collbackas);
	ins.Add("AttenuationParam", &m_AttenuationParam, collbackap);
	auto c = Color(m_Color);
	ins.Add("Color", &c, collback);
	ins.Add("HDR", &m_HDR, [&](float f){m_HDR = f; });

	ins.Complete();
}
#endif

void PointLightComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(m_Radius);
	_KEY(m_AttenuationStart);
	_KEY(m_AttenuationParam);
	_KEY(m_Color.x);
	_KEY(m_Color.y);
	_KEY(m_Color.z);
	_KEY(m_Color.w);
	_KEY(m_HDR);

#undef _KEY
}