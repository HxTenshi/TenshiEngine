#include "Component.h"

#include "Game/Game.h"

void PostEffectComponent::Initialize(){
	mRenderTarget.Create(WindowState::mWidth, WindowState::mHeight);
	mModelTexture.Create("EngineResource/TextureModel.tesmesh");

	mMaterial.Create("EngineResource/PostEffect.fx");
	mMaterial.SetTexture(Game::GetMainViewRenderTarget().GetTexture(), 0);

	mMaterialEnd.Create("EngineResource/PostEffect.fx");
	mMaterialEnd.SetTexture(mRenderTarget.GetTexture(), 0);

}
void PostEffectComponent::Finish(){
	mRenderTarget.Release();
	mModelTexture.Release();
}
void PostEffectComponent::PostDraw(){


	mModelTexture.Update();

	Game::AddDrawList(DrawStage::PostEffect, [&](){

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

		Game::GetMainViewRenderTarget().ClearView();
		Game::GetMainViewRenderTarget().SetRendererTarget();
		//Device::mRenderTargetBack->SetRendererTarget();

		mModelTexture.Draw(mMaterialEnd);


		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS_tex->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();

		//const RenderTarget* r[1] = { NULL };
		//RenderTarget::SetRendererTarget((UINT)1, r[0], NULL);
		//Device::mRenderTargetBack->SetRendererTarget();
		//Device::mpImmediateContext->CopyResource(Device::mRenderTargetBack->GetTexture2D(), mRenderTarget.GetTexture2D());



	});
}