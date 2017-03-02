#include "EnginLoading.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Game/Component/TextureModelComponent.h"
#include "Graphic/Material/Material.h"
#include "Engine/AssetDataBase.h"
#include "Window/Window.h"
#include "Game/RenderingSystem.h"
#include "Graphic/RenderTarget/RenderTarget.h"
#include "Graphic/Model/Model.h"

struct Texs {
	TextureAsset mLogo;
	TextureAsset mLoadIco;
	TextureAsset mLoadText;
};

EnginLoading::EnginLoading()
{
	mModel = new Model();
	MeshAsset mesh;
	AssetDataBase::DirectLoad("EngineResource/TextureModel.tesmesh", mesh.m_Ptr);
	mModel->Create(mesh);

	mMaterial = new Material();
	ShaderAsset sha;
	AssetDataBase::DirectLoad("EngineResource/Texture.fx", sha.m_Ptr);
	mMaterial->Create(sha);
	imp = new Texs();
	AssetDataBase::DirectLoad("EngineResource/Logo/Logo.png", imp->mLogo.m_Ptr);
	AssetDataBase::DirectLoad("EngineResource/Logo/loading.png", imp->mLoadIco.m_Ptr);
	AssetDataBase::DirectLoad("EngineResource/Logo/NowLoading.png", imp->mLoadText.m_Ptr);
	
}

EnginLoading::~EnginLoading()
{
	delete mModel;
	delete mMaterial;
	delete imp;
}

void EnginLoading::Update()
{

	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)WindowState::mWidth;
	vp.Height = (FLOAT)WindowState::mHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	render->m_Context->RSSetViewports(1, &vp);
	render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Device::mRenderTargetBack->ClearView(render->m_Context);
	Device::mRenderTargetBack->ClearDepth(render->m_Context);

	Device::mRenderTargetBack->SetRendererTarget(render->m_Context);

	render->PushSet(Rasterizer::Preset::RS_Back_Solid);


	ID3D11ShaderResourceView *const pNULL[4] = { NULL, NULL, NULL, NULL };
	render->m_Context->PSSetShaderResources(0, 4, pNULL);
	ID3D11SamplerState *const pSNULL[4] = { NULL, NULL, NULL, NULL };
	render->m_Context->PSSetSamplers(0, 4, pSNULL);


	{
		render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
		render->PushSet(BlendState::Preset::BS_Alpha, 0xFFFFFFFF);

		Draw();

		render->PopBS();
		render->PopDS();
	}


	render->PopRS();


	{
		ID3D11ShaderResourceView *const pNULL[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		ID3D11SamplerState *const pSNULL[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		render->m_Context->PSSetShaderResources(0, 8, pNULL);
		render->m_Context->PSSetSamplers(0, 8, pSNULL);
		render->m_Context->VSSetShaderResources(0, 8, pNULL);
		render->m_Context->VSSetSamplers(0, 8, pSNULL);
		render->m_Context->GSSetShaderResources(0, 8, pNULL);
		render->m_Context->GSSetSamplers(0, 8, pSNULL);
		render->m_Context->PSSetShader(NULL, NULL, 0);
		render->m_Context->VSSetShader(NULL, NULL, 0);
		render->m_Context->GSSetShader(NULL, NULL, 0);
		render->m_Context->CSSetShader(NULL, NULL, 0);
	}
}

void EnginLoading::Draw() {


	XMMATRIX logoMat;
	XMMATRIX loadTextMat;
	XMMATRIX loadMat;
	{
		auto p = XMVectorSet(WindowState::mWidth / 2.0f, WindowState::mHeight / 2.0f, 0, 1);
		auto s = XMVectorSet(800,129, 0, 1);
		auto r = XMVectorZero();

		auto scaleMatrix = XMMatrixScaling(s.x, s.y, 0.0f);
		s.x = 1.0f / WindowState::mWidth;
		s.y = 1.0f / WindowState::mHeight;
		auto scaleProjMatrix = XMMatrixScaling(s.x, s.y, 0.0f);
		auto rotateMatrix = XMMatrixRotationRollPitchYawFromVector(r);
		p.x = p.x * 2 / WindowState::mWidth - 1.0f;
		p.y = p.y * 2 / WindowState::mHeight - 1.0f;
		auto transMatrix = XMMatrixTranslation(p.x, p.y, 0.0f);

		auto mat = XMMatrixMultiply(scaleMatrix, rotateMatrix);
		mat = XMMatrixMultiply(mat, scaleProjMatrix);
		logoMat = XMMatrixMultiply(mat, transMatrix);
	}
	{

		auto p = XMVectorSet(WindowState::mWidth - 100 -32 - 184/2, 100, 0, 1);
		auto s = XMVectorSet(184, 64, 0, 1);
		auto r = XMVectorZero();

		auto scaleMatrix = XMMatrixScaling(s.x, s.y, 0.0f);
		s.x = 1.0f / WindowState::mWidth;
		s.y = 1.0f / WindowState::mHeight;
		auto scaleProjMatrix = XMMatrixScaling(s.x, s.y, 0.0f);
		auto rotateMatrix = XMMatrixRotationRollPitchYawFromVector(r);
		p.x = p.x * 2 / WindowState::mWidth - 1.0f;
		p.y = p.y * 2 / WindowState::mHeight - 1.0f;
		auto transMatrix = XMMatrixTranslation(p.x, p.y, 0.0f);

		auto mat = XMMatrixMultiply(scaleMatrix, rotateMatrix);
		mat = XMMatrixMultiply(mat, scaleProjMatrix);
		loadTextMat = XMMatrixMultiply(mat, transMatrix);
	}
	{
		static float _p = 0;
		_p -= XM_PI/180.0f * 6;
		_p = fmod(_p, XM_2PI);
		auto p = XMVectorSet(WindowState::mWidth - 100, 100 , 0, 1);
		auto s = XMVectorSet(64,64, 0, 1);
		auto r = XMVectorSet(0,0, _p,1);

		auto scaleMatrix = XMMatrixScaling(s.x, s.y, 0.0f);
		s.x = 1.0f / WindowState::mWidth;
		s.y = 1.0f / WindowState::mHeight;
		auto scaleProjMatrix = XMMatrixScaling(s.x, s.y, 0.0f);
		auto rotateMatrix = XMMatrixRotationRollPitchYawFromVector(r);
		p.x = p.x * 2 / WindowState::mWidth - 1.0f;
		p.y = p.y * 2 / WindowState::mHeight - 1.0f;
		auto transMatrix = XMMatrixTranslation(p.x, p.y, 0.0f);


		auto mat = XMMatrixMultiply(scaleMatrix, rotateMatrix);
		mat = XMMatrixMultiply(mat, scaleProjMatrix);
		loadMat = XMMatrixMultiply(mat, transMatrix);
	}

	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

	mModel->mWorld = logoMat;
	mModel->Update();
	mMaterial->SetTexture(imp->mLogo);
	mModel->Draw(render->m_Context, *mMaterial);

	mModel->mWorld = loadTextMat;
	mModel->Update();
	mMaterial->SetTexture(imp->mLoadText);
	mModel->Draw(render->m_Context, *mMaterial);

	mModel->mWorld = loadMat;
	mModel->Update();
	mMaterial->SetTexture(imp->mLoadIco);
	mModel->Draw(render->m_Context, *mMaterial);

}
