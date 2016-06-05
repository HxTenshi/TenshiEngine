
#include "CameraComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"

#include "Game/Game.h"

#include "Game/RenderingSystem.h"

CameraComponent::CameraComponent()
{
	mScreenClearType = ScreenClearType::Color;
	//mSkyMaterial.mMaterialPath = "EngineResource/Sky/Sky.pmx.txt";
	//mSkyMaterial.Initialize();

	mSkyMaterial.Create("EngineResource/NoLighting.fx");
	mSkyMaterial.SetTexture("EngineResource/Sky/a.png");

	mSkyModel.Create("EngineResource/Sky/Sky.pmx.tesmesh");
}
CameraComponent::~CameraComponent()
{
	mSkyModel.Release();
}

void CameraComponent::Initialize()
{

	mCBNeverChanges = ConstantBuffer<CBNeverChanges>::create(0);
	mCBChangeOnResize = ConstantBuffer<CBChangeOnResize>::create(1);
	mCBBillboard = ConstantBuffer<CBBillboard>::create(8);

	// Initialize the view matrix
	Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	UpdateView();


	SetPerspective();

	//static RenderTarget mRenderTarget;
	//mRenderTarget.CreateCUBE(256, 256, DXGI_FORMAT_R32G32B32A32_FLOAT);

}

void CameraComponent::SetPerspective(){
	UINT width = WindowState::mWidth;
	UINT height = WindowState::mHeight;
	// Initialize the projection matrix
	mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);


	mCBChangeOnResize.mParam.mProjection = XMMatrixTranspose(mProjection);
	XMVECTOR null;
	mCBChangeOnResize.mParam.mProjectionInv = XMMatrixTranspose(XMMatrixInverse(&null, mProjection));


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	mCBChangeOnResize.UpdateSubresource(render->m_Context);
}
void CameraComponent::SetOrthographic(){
	UINT width = WindowState::mWidth;
	UINT height = WindowState::mHeight;
	mProjection = XMMatrixOrthographicLH((FLOAT)width, (FLOAT)height, 0.01f, 4000.0f);


	mCBChangeOnResize.mParam.mProjection = XMMatrixTranspose(mProjection);
	XMVECTOR null;
	mCBChangeOnResize.mParam.mProjectionInv = XMMatrixTranspose(XMMatrixInverse(&null, mProjection));


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	mCBChangeOnResize.UpdateSubresource(render->m_Context);

}

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


	Game::SetMainCamera(this);
}
#ifdef _ENGINE_MODE
void CameraComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::ViewInspector("Camera", this, data);
}
#endif

void CameraComponent::IO_Data(I_ioHelper* io){
	(void)io;
#define _KEY(x) io->func( x , #x)

#undef _KEY
}

void CameraComponent::VSSetConstantBuffers(ID3D11DeviceContext* context) const
{
	mCBNeverChanges.VSSetConstantBuffers(context);
	mCBChangeOnResize.VSSetConstantBuffers(context);
	mCBBillboard.VSSetConstantBuffers(context);
}
void CameraComponent::PSSetConstantBuffers(ID3D11DeviceContext* context) const
{
	mCBNeverChanges.PSSetConstantBuffers(context);
	mCBChangeOnResize.PSSetConstantBuffers(context);
	mCBBillboard.PSSetConstantBuffers(context);
}
void CameraComponent::GSSetConstantBuffers(ID3D11DeviceContext* context) const
{
	mCBNeverChanges.GSSetConstantBuffers(context);
	mCBChangeOnResize.GSSetConstantBuffers(context);
	mCBBillboard.GSSetConstantBuffers(context);
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

	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);



	XMVECTOR pos = gameObject->mTransform->WorldPosition();
	XMMATRIX pm = XMMatrixTranslation(pos.x, pos.y, pos.z);
	
	//UINT width = WindowState::mWidth;
	//UINT height = WindowState::mHeight;
	//auto wh = XMVectorSet(width, height, 0, 1);
	//auto l = XMVector2Length(wh);
	//l.x *= 2;
	//float scale1 = 1.0f / 200.0f;
	//float s = l.x*scale1;
	float s = 0.1f;
	XMMATRIX sm = XMMatrixScaling(s,s,s);
	mSkyModel.mWorld = sm * pm;
	mSkyModel.Update();

	mSkyModel.Draw(render->m_Context, mSkyMaterial);


	render->PopDS();
}