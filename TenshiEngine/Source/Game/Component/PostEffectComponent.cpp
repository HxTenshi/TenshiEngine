#include "PostEffectComponent.h"

#include "Game/Game.h"

#include "Game/RenderingSystem.h"

void PostEffectComponent::Initialize(){
	mRenderTarget.Create(WindowState::mWidth, WindowState::mHeight, DXGI_FORMAT_R11G11B10_FLOAT);
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
		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		mRenderTarget.ClearView(render->m_Context);
		mRenderTarget.SetRendererTarget(render->m_Context);

		render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
		render->PushSet(Rasterizer::Preset::RS_None_Solid);


		mModelTexture.Draw(render->m_Context, mMaterial);

		render->m_Context->CopyResource(Game::GetMainViewRenderTarget().GetTexture2D(), mRenderTarget.GetTexture2D());

		render->PopRS();
		render->PopDS();


	});
}

#ifdef _ENGINE_MODE
void PostEffectComponent::CreateInspector(){
	auto data = Window::CreateInspector();

	std::function<void(std::string)> collback = [&](std::string name){
		mShaderName = name;

		mMaterial.Create(mShaderName.c_str());
	};

	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Shader", &mShaderName, collback), data);

	Window::ViewInspector("PostEffect", this, data);
}
#endif


void PostEffectComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)

	_KEY(mShaderName);

#undef _KEY
}
