
#include "CameraComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"

#include "Game/Game.h"

CameraComponent::CameraComponent(){}
CameraComponent::~CameraComponent(){}

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


	UINT width = WindowState::mWidth;
	UINT height = WindowState::mHeight;
	// Initialize the projection matrix
	mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

	mCBChangeOnResize.mParam.mProjection = XMMatrixTranspose(mProjection);
	XMVECTOR null;
	mCBChangeOnResize.mParam.mProjectionInv = XMMatrixTranspose(XMMatrixInverse(&null, mProjection));
	mCBChangeOnResize.UpdateSubresource();

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