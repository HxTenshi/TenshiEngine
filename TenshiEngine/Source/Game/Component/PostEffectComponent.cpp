#include "PostEffectComponent.h"

#include "Game/Game.h"

#include "Game/RenderingSystem.h"

#include "Engine/Inspector.h"

RenderTarget PostEffectHelper::mCurrentRenderTarget;

void PostEffectComponent::Initialize(){
	mRenderTarget.Create(WindowState::mWidth, WindowState::mHeight, DXGI_FORMAT_R11G11B10_FLOAT);
	mModelTexture.Create("EngineResource/TextureModel.tesmesh");

	mMaterial.Create(mShader);

}
void PostEffectComponent::Finish(){
	mRenderTarget.Release();
	mModelTexture.Release();
}

#ifdef _ENGINE_MODE
void PostEffectComponent::EngineUpdate(){
	PostDraw();
}
#endif
void PostEffectComponent::Update(){
	PostDraw();
}

void PostEffectComponent::PostDraw(){

	if (!mShader.IsLoad()){
		return;
	}
	mModelTexture.Update();

	Game::AddDrawList(DrawStage::PostEffect, [&](){
		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		mRenderTarget.ClearView(render->m_Context);
		mRenderTarget.SetRendererTarget(render->m_Context);

		render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
		render->PushSet(Rasterizer::Preset::RS_None_Solid);

		auto target = PostEffectHelper::CurrentTarget();
		mMaterial.SetTexture(target.GetTexture(), 1);


		mModelTexture.Draw(render->m_Context, mMaterial);


		//auto dst = Game::GetMainViewRenderTarget().GetTexture().GetResource();
		//auto src = mRenderTarget.GetTexture().GetResource();
		//render->m_Context->CopyResource(dst->Get(), src->Get());

		//render->m_Context->CopySubresourceRegion(Game::GetMainViewRenderTarget().GetTexture2D(), NULL, 0, 0, 0, mRenderTarget.GetTexture2D(), NULL, NULL);
		//render->m_Context->CopyResource(Game::GetMainViewRenderTarget().GetTexture2D(), mRenderTarget.GetTexture2D());

		PostEffectHelper::SetCurrentTarget(mRenderTarget);

		render->PopRS();
		render->PopDS();


	});
}

#ifdef _ENGINE_MODE
void PostEffectComponent::CreateInspector(){

	Inspector ins("PostEffect", this);
	ins.AddEnableButton(this);
	ins.Add("Shader", &mShader, [&](){
		mMaterial.Create(mShader);
	});
	ins.Complete();
}
#endif


void PostEffectComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)

	_KEY(mShader);

#undef _KEY
}
