

#include "MySTL/Ptr.h"
#include "Graphic/Font/Font.h"
#include "Graphic/Material/Material.h"
#include "TextComponent.h"
#include "TextureModelComponent.h"
#include "MaterialComponent.h"
#include "Game/Game.h"

class TextComponentMember{
public:
	Model mModel;

	Font mFont;
	std::string mText;

	Material mTexMaterial;
};

TextComponent::TextComponent()
{
	impl = new TextComponentMember();

	impl->mTexMaterial.Create("EngineResource/texture.fx");
	impl->mTexMaterial.SetTexture(impl->mFont.GetTexture());
}
TextComponent::~TextComponent()
{
	impl->mModel.Release();
	delete impl;
}

void TextComponent::Initialize(){

	impl->mModel.Create("EngineResource/TextureModel.tesmesh");
	ChangeText(impl->mText);
}
void TextComponent::Finish(){
}
void TextComponent::EngineUpdate(){
	DrawTextUI();
}

void TextComponent::Update(){
	DrawTextUI();
}

void TextComponent::DrawTextUI(){


	Game::AddDrawList(DrawStage::Init, [&](){

		float h = (float)WindowState::mHeight;
		float w = (float)WindowState::mWidth;
		float hh = (float)WindowState::mHeight / 2.0f;
		float wh = (float)WindowState::mWidth / 2.0f;

		auto s = gameObject->mTransform->Scale();
		float sx = s.x / w;
		float sy = s.y / h;
		auto p = gameObject->mTransform->Position();
		float px = (p.x - wh) / wh;
		float py = (-p.y + hh) / hh;

		auto pos = XMVectorSet(px, py, 0, 1);
		auto rot = gameObject->mTransform->Rotate();
		auto scl = XMVectorSet(sx, sy, 0, 1);

		auto mat = XMMatrixMultiply(
			XMMatrixMultiply(
			XMMatrixRotationRollPitchYawFromVector(rot),
			XMMatrixScalingFromVector(scl)),
			XMMatrixTranslationFromVector(pos));

		impl->mModel.mWorld = mat;
		impl->mModel.Update();

	});

	Game::AddDrawList(DrawStage::UI, [&](){

		Model& model = impl->mModel;

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		model.Draw(render->m_Context, impl->mTexMaterial);

		//ID3D11DepthStencilState* pBackDS;
		//UINT ref;
		//Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);
		//
		//D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		//descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		////descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		//descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		//ID3D11DepthStencilState* pDS_tex = NULL;
		//Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
		//Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);
		//
		//D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		//descRS.CullMode = D3D11_CULL_BACK;
		//descRS.FillMode = D3D11_FILL_SOLID;
		//
		//ID3D11RasterizerState* pRS = NULL;
		//Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);
		//
		//Device::mpImmediateContext->RSSetState(pRS);
		//
		//model.Draw(impl->mTexMaterial);
		//
		//Device::mpImmediateContext->RSSetState(NULL);
		//if (pRS)pRS->Release();
		//
		//Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		//pDS_tex->Release();
		//
		//Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		//if (pBackDS)pBackDS->Release();
	});
}

void TextComponent::CreateInspector(){

	std::function<void(std::string)> collback = [&](std::string name){
		ChangeText(name);
	};
	auto data = Window::CreateInspector();
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Text", &impl->mText, collback), data);
	Window::ViewInspector("Text", this, data);
}

void TextComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(impl->mText);
#undef _KEY
}

void TextComponent::ChangeText(const std::string& text){
	impl->mText = text;
	impl->mFont.SetText(impl->mText);
}