
#include "DeferredRendering.h"

DeferredRendering::~DeferredRendering(){
	m_AlbedoRT.Release();
	m_NormalRT.Release();
	m_DepthRT.Release();
	m_LightRT.Release();
	mModelTexture.Release();

	pBlendState->Release();
}
void DeferredRendering::Initialize(){
	auto w = WindowState::mWidth;
	auto h = WindowState::mHeight;
	m_AlbedoRT.Create(w, h);
	m_NormalRT.Create(w, h);
	m_DepthRT.Create(w, h);
	m_LightRT.Create(w, h);

	mModelTexture.Create("EngineResource/TextureModel.tesmesh");


	mMaterialLight.Create("EngineResource/DeferredLightRendering.fx");
	mMaterialLight.SetTexture(m_NormalRT.GetTexture(), 0);
	mMaterialLight.SetTexture(m_DepthRT.GetTexture(), 1);

	mMaterialDeferred.Create("EngineResource/DeferredRendering.fx");
	mMaterialDeferred.SetTexture(m_AlbedoRT.GetTexture(), 0);
	mMaterialDeferred.SetTexture(m_LightRT.GetTexture(), 1);

	//ブレンドモード設定

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;

	// TRUEの場合、マルチレンダーターゲットで各レンダーターゲットのブレンドステートの設定を個別に設定できる
	// FALSEの場合、0番目のみが使用される
	BlendDesc.IndependentBlendEnable = FALSE;

	//加算合成設定
	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;
	RenderTarget.BlendEnable = TRUE;
	RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	RenderTarget.DestBlend = D3D11_BLEND_ONE;
	RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BlendDesc.RenderTarget[0] = RenderTarget;

	Device::mpd3dDevice->CreateBlendState(&BlendDesc, &pBlendState);


}
void DeferredRendering::Start_G_Buffer_Rendering(){

	m_AlbedoRT.ClearView();
	m_NormalRT.ClearView();
	m_DepthRT.ClearView();

	const RenderTarget* r[3] = { &m_AlbedoRT, &m_NormalRT, &m_DepthRT };
	RenderTarget::SetRendererTarget((UINT)3, r[0], Device::mRenderTargetBack);
}
void DeferredRendering::Start_Light_Rendering(){

	m_LightRT.ClearView();

	const RenderTarget* r[1] = { &m_LightRT };
	RenderTarget::SetRendererTarget((UINT)1, r[0], Device::mRenderTargetBack);


	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Device::mpImmediateContext->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);

	//ディレクショナルライト
	{

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

		mModelTexture.Draw(mMaterialLight);

		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS_tex->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	}
}

void DeferredRendering::End_Light_Rendering(){

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Device::mpImmediateContext->OMSetBlendState(NULL, blendFactor, 0xffffffff);
}
void DeferredRendering::Start_Deferred_Rendering(RenderTarget* rt){

	rt->SetRendererTarget();

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

	mModelTexture.Draw(mMaterialDeferred);

	Device::mpImmediateContext->RSSetState(NULL);
	if (pRS)pRS->Release();

	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
	pDS_tex->Release();

	Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
	if (pBackDS)pBackDS->Release();
}