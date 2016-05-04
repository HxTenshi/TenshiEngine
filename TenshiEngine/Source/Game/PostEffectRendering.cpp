

#include "Device/DirectX11Device.h"
#include "PostEffectRendering.h"
#include "Game.h"


PostEffectRendering::PostEffectRendering(){
}

PostEffectRendering::~PostEffectRendering(){
	mModelTexture.Release();
}
void PostEffectRendering::Initialize(){
	mModelTexture.Create("EngineResource/TextureModel.tesmesh");
	mMaterial.Create("EngineResource/PostEffectRendering.fx");
	mMaterial.SetTexture(Game::GetMainViewRenderTarget().GetTexture(), 0);
}

void PostEffectRendering::Rendering(){


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	

	Device::mRenderTargetBack->SetRendererTarget(render->m_Context);

	mModelTexture.Update();


	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);

	mModelTexture.Draw(render->m_Context, mMaterial);

	render->PopRS();
	render->PopDS();
}