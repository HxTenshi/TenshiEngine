

#include "Device/DirectX11Device.h"
#include "PostEffectRendering.h"
#include "Game.h"

#include "Component/PostEffectComponent.h"

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

void PostEffectRendering::Rendering(IRenderingEngine* render, const std::function<void(void)>& func){

	//デファードで解除済み
	//RenderTarget::NullSetRendererTarget(render->m_Context);

	auto rt = Game::GetMainViewRenderTarget();
	PostEffectHelper::SetCurrentTarget(rt);

	func();
	
	auto target = PostEffectHelper::CurrentTarget();

	RenderTarget::SetRendererTarget(render->m_Context,1,&target,&Game::GetMainViewRenderTarget());
}

void PostEffectRendering::Flip(IRenderingEngine* render){

	Device::mRenderTargetBack->SetRendererTarget(render->m_Context);

	auto target = PostEffectHelper::CurrentTarget();
	mMaterial.SetTexture(target.GetTexture(), 0);


	mModelTexture.Update();


	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);

	mModelTexture.Draw(render->m_Context, mMaterial);

	render->PopRS();
	render->PopDS();
}