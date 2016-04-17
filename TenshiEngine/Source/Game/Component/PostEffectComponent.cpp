#include "PostEffectComponent.h"

#include "Game/Game.h"

void PostEffectComponent::Initialize(){
	mRenderTarget.Create(WindowState::mWidth, WindowState::mHeight);
	mModelTexture.Create("EngineResource/TextureModel.tesmesh");

	mMaterial.Create(mShaderName.c_str());

}
void PostEffectComponent::Finish(){
	mRenderTarget.Release();
	mModelTexture.Release();
}

void PostEffectComponent::EngineUpdate(){
	PostDraw();
}
void PostEffectComponent::Update(){
	PostDraw();
}

void PostEffectComponent::PostDraw(){

	mModelTexture.Update();

	Game::AddDrawList(DrawStage::PostEffect, [&](){
		if (mShaderName == ""){
			return;
		}

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

		Device::mpImmediateContext->CopyResource(Game::GetMainViewRenderTarget().GetTexture2D(), mRenderTarget.GetTexture2D());

		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS_tex->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();


	});
}

void PostEffectComponent::CreateInspector(){
	auto data = Window::CreateInspector();

	std::function<void(std::string)> collback = [&](std::string name){
		mShaderName = name;

		mMaterial.Create(mShaderName.c_str());
	};

	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Shader", &mShaderName, collback), data);

	Window::ViewInspector("PostEffect", this, data);
}


void PostEffectComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)

	_KEY(mShaderName);

#undef _KEY
}
