
#include "CameraComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"

#include "Game/Game.h"

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

}

void CameraComponent::SetPerspective(){
	UINT width = WindowState::mWidth;
	UINT height = WindowState::mHeight;
	// Initialize the projection matrix
	mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);


	mCBChangeOnResize.mParam.mProjection = XMMatrixTranspose(mProjection);
	XMVECTOR null;
	mCBChangeOnResize.mParam.mProjectionInv = XMMatrixTranspose(XMMatrixInverse(&null, mProjection));
	mCBChangeOnResize.UpdateSubresource();
}
void CameraComponent::SetOrthographic(){
	UINT width = WindowState::mWidth;
	UINT height = WindowState::mHeight;
	mProjection = XMMatrixOrthographicLH((FLOAT)width, (FLOAT)height, 0.01f, 4000.0f);


	mCBChangeOnResize.mParam.mProjection = XMMatrixTranspose(mProjection);
	XMVECTOR null;
	mCBChangeOnResize.mParam.mProjectionInv = XMMatrixTranspose(XMMatrixInverse(&null, mProjection));
	mCBChangeOnResize.UpdateSubresource();
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

void CameraComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::ViewInspector("Camera", this, data);
}

void CameraComponent::IO_Data(I_ioHelper* io){
	(void)io;
#define _KEY(x) io->func( x , #x)

#undef _KEY
}

void CameraComponent::VSSetConstantBuffers() const
{
	mCBNeverChanges.VSSetConstantBuffers();
	mCBChangeOnResize.VSSetConstantBuffers();
	mCBBillboard.VSSetConstantBuffers();
}
void CameraComponent::PSSetConstantBuffers() const
{
	mCBNeverChanges.PSSetConstantBuffers();
	mCBChangeOnResize.PSSetConstantBuffers();
	mCBBillboard.PSSetConstantBuffers();
}
void CameraComponent::GSSetConstantBuffers() const
{
	mCBNeverChanges.GSSetConstantBuffers();
	mCBChangeOnResize.GSSetConstantBuffers();
	mCBBillboard.GSSetConstantBuffers();
}

const XMMATRIX& CameraComponent::GetViewMatrix(){
	return mView;
}


void CameraComponent::UpdateView(){
	mView = XMMatrixLookAtLH(Eye, At, Up);


	auto Billboard = XMMatrixLookAtLH(XMVectorZero(), Eye - At, Up);
	XMVECTOR null;
	Billboard = XMMatrixTranspose(XMMatrixInverse(&null, Billboard));
	mCBBillboard.mParam.mBillboardMatrix = Billboard;
	mCBBillboard.UpdateSubresource();

	mCBNeverChanges.mParam.mView = XMMatrixTranspose(mView);
	mCBNeverChanges.mParam.mViewInv = XMMatrixTranspose(XMMatrixInverse(&null, mView));
	mCBNeverChanges.UpdateSubresource();

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
	D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11DepthStencilState* pDS = NULL;
	Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
	Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);


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

	mSkyModel.Draw(mSkyMaterial);


	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
	if (pDS)pDS->Release();
}