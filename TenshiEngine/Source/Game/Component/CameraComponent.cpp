
#include "CameraComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"

#include "Game/Game.h"

#include "Game/RenderingSystem.h"

#include "Engine/Inspector.h"
#include "Engine/AssetLoad.h"

CameraComponent::CameraComponent()
{
	mScreenClearType = ScreenClearType::Color;
	m_IsPerspective = true;
	m_Fov = XM_PIDIV4;
	m_Near = 0.01f;
	m_Far = 1000.0f;
	//mSkyMaterial.mMaterialPath = "EngineResource/Sky/Sky.pmx.txt";
	//mSkyMaterial.Initialize();

	//mSkyMaterial.Create("EngineResource/NoLighting.fx");
	//mSkyMaterial.SetTexture("EngineResource/Sky/a.png");
	//mSkyModel.Create("EngineResource/Sky/Sky.pmx.tesmesh");

}
CameraComponent::~CameraComponent()
{
	//mSkyModel.Release();
}

void CameraComponent::Initialize()
{

	mCBNeverChanges = ConstantBuffer<CBNeverChanges>::create(0);
	mCBChangeOnResize = ConstantBuffer<CBChangeOnResize>::create(1);
	mCBBillboard = ConstantBuffer<CBBillboard>::create(8);
	mCBNearFar = ConstantBuffer<cbNearFar>::create(12);

	mCBNearFar.mParam.Near = 0.01f;
	mCBNearFar.mParam.Far = 1000.0f;
	mCBNearFar.mParam.NULLnf = XMFLOAT2(0, 0);

	// Initialize the view matrix
	Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	UpdateView();

	SetPerspective();

	AssetLoad::Instance(mSkyTexture.m_Hash, mSkyTexture);
	SetSkyTexture(mSkyTexture);

	//static RenderTarget mRenderTarget;
	//mRenderTarget.CreateCUBE(256, 256, DXGI_FORMAT_R32G32B32A32_FLOAT);

}

void CameraComponent::SetPerspective(){
	m_IsPerspective = true;
}
void CameraComponent::SetOrthographic(){
	m_IsPerspective = false;
}
#ifdef _ENGINE_MODE
void CameraComponent::EngineUpdate(){

	Game::SetMainCameraEngineUpdate(this);
}
#endif
void CameraComponent::Update(){

	//XMVECTOR null;
	//auto lossy = gameObject->mTransform->LossyScale();
	//auto scale = gameObject->mTransform->Scale();
	//gameObject->mTransform->Scale(XMVectorSet(lossy.x * scale.x, lossy.y * scale.y, lossy.z * scale.z, 1));
	//
	//mView = XMMatrixInverse(&null, gameObject->mTransform->GetMatrix());
	//mCBNeverChanges.mParam.mView = XMMatrixTranspose(mView);
	//mCBNeverChanges.mParam.mViewInv = XMMatrixTranspose(gameObject->mTransform->GetMatrix());
	//mCBNeverChanges.UpdateSubresource();
	Eye = gameObject->mTransform->WorldPosition();
	At = Eye + gameObject->mTransform->Forward();
	UpdateView();


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	mCBNearFar.mParam.Near = m_Near;
	mCBNearFar.mParam.Far = m_Far;
	mCBNearFar.UpdateSubresource(render->m_Context);

	FLOAT width = (FLOAT)WindowState::mWidth;
	FLOAT height = (FLOAT)WindowState::mHeight;
	if (m_IsPerspective){
		mProjection = XMMatrixPerspectiveFovLH(m_Fov, width / height, m_Near, m_Far);
	}
	else{
		mProjection = XMMatrixOrthographicLH(width, height, m_Near, m_Far);
	}
	mCBChangeOnResize.mParam.mProjection = XMMatrixTranspose(mProjection);
	XMVECTOR null;
	mCBChangeOnResize.mParam.mProjectionInv = XMMatrixTranspose(XMMatrixInverse(&null, mProjection));

	mCBChangeOnResize.UpdateSubresource(render->m_Context);

	Game::SetMainCamera(this);
}
#ifdef _ENGINE_MODE
void CameraComponent::CreateInspector(){
	Inspector ins("Camera", this);
	ins.AddEnableButton(this);
	auto func = [&](){SetSkyTexture(mSkyTexture); };
	ins.Add("SkyTexture", &mSkyTexture, func);
	ins.Complete();
}
#endif

void CameraComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mSkyTexture);
#undef _KEY
}


void CameraComponent::SetSkyTexture(TextureAsset asset){
	mSkyTexture = asset;
}

void CameraComponent::VSSetConstantBuffers(ID3D11DeviceContext* context) const
{
	mCBNeverChanges.VSSetConstantBuffers(context);
	mCBChangeOnResize.VSSetConstantBuffers(context);
	mCBBillboard.VSSetConstantBuffers(context);
	mCBNearFar.VSSetConstantBuffers(context);
}
void CameraComponent::PSSetConstantBuffers(ID3D11DeviceContext* context) const
{
	mCBNeverChanges.PSSetConstantBuffers(context);
	mCBChangeOnResize.PSSetConstantBuffers(context);
	mCBBillboard.PSSetConstantBuffers(context);
	mCBNearFar.PSSetConstantBuffers(context);
}
void CameraComponent::GSSetConstantBuffers(ID3D11DeviceContext* context) const
{
	mCBNeverChanges.GSSetConstantBuffers(context);
	mCBChangeOnResize.GSSetConstantBuffers(context);
	mCBBillboard.GSSetConstantBuffers(context);
	mCBNearFar.GSSetConstantBuffers(context);
}

const XMMATRIX& CameraComponent::GetViewMatrix(){
	return mView;
}


void CameraComponent::UpdateView(){


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	
	mView = XMMatrixLookAtLH(Eye, At, Up);

	auto Billboard = XMMatrixLookAtLH(XMVectorZero(), Eye - At, Up);
	XMVECTOR null;
	Billboard = XMMatrixTranspose(XMMatrixInverse(&null, Billboard));
	mCBBillboard.mParam.mBillboardMatrix = Billboard;
	mCBBillboard.UpdateSubresource(render->m_Context);

	mCBNeverChanges.mParam.mView = XMMatrixTranspose(mView);
	mCBNeverChanges.mParam.mViewInv = XMMatrixTranspose(XMMatrixInverse(&null, mView));
	mCBNeverChanges.UpdateSubresource(render->m_Context);

	mCBChangeOnResize.UpdateSubresource(render->m_Context);
	mCBNearFar.UpdateSubresource(render->m_Context);

	//gameObject->mTransform->Position(Eye);

	//XMVECTOR zaxis = XMVector3Normalize(At - Eye);
	//XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(Up, zaxis));
	//XMVECTOR yaxis = XMVector3Cross(zaxis, xaxis);
	//
	//float yaw = 0.0f;
	//float roll = 0.0f;
	//float pitch = asin(mView._31);
	//if (cos(pitch) == 0.0f){
	//	yaw = atan2(mView._23, mView._22);
	//}
	//else{
	//	float roll = asin(-mView._21 / cos(pitch));
	//	if (mView._11 < 0) roll = 180 - roll;
	//
	//	yaw = atan2(-mView._32, mView._33);
	//}

	//gameObject->mTransform->Rotate(XMVectorSet(roll, pitch, yaw, 1.0f));
}

void CameraComponent::ScreenClear(){

	//auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	//render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	//
	//
	//
	//XMVECTOR pos = gameObject->mTransform->WorldPosition();
	//XMMATRIX pm = XMMatrixTranslation(pos.x, pos.y, pos.z);
	//
	////UINT width = WindowState::mWidth;
	////UINT height = WindowState::mHeight;
	////auto wh = XMVectorSet(width, height, 0, 1);
	////auto l = XMVector2Length(wh);
	////l.x *= 2;
	////float scale1 = 1.0f / 200.0f;
	////float s = l.x*scale1;
	//float s = 0.1f;
	//XMMATRIX sm = XMMatrixScaling(s,s,s);
	//mSkyModel.mWorld = sm * pm;
	//mSkyModel.Update();
	//
	//mSkyModel.Draw(render->m_Context, mSkyMaterial);
	//
	//
	//render->PopDS();
}


float CameraComponent::GetNear(){
	return mCBNearFar.mParam.Near;
}
float CameraComponent::GetFar(){
	return mCBNearFar.mParam.Far;

}