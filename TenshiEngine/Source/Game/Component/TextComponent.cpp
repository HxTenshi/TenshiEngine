

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

	FontFileData mFont;
	std::string mText;

	Material mTexMaterial;
};

TextComponent::TextComponent()
{
	impl = new TextComponentMember();

	impl->mTexMaterial.Create("EngineResource/texture.fx");

	mFontSize = 48.0f;
	mCenter = false;
}
TextComponent::~TextComponent()
{
	impl->mModel.Release();
	delete impl;
}

void TextComponent::Initialize(){

	impl->mFont.CreateFont_("", mFontSize);
	
	auto mBackScale = gameObject->mTransform->Scale();
	impl->mFont.CreateTexture_(mBackScale.x, mBackScale.y);

	impl->mModel.Create("EngineResource/TextureModel.tesmesh");
	ChangeText(impl->mText);
}
void TextComponent::Finish(){
}
void TextComponent::EngineUpdate(){

	auto s = gameObject->mTransform->Scale();
	if (mBackScale.x != s.x || mBackScale.y != s.y){
		impl->mFont.CreateTexture_(s.x, s.y);
		ChangeText(impl->mText);
		mBackScale = s;
	}

	DrawTextUI();
}

void TextComponent::Update(){
	DrawTextUI();
}

void TextComponent::DrawTextUI(){

	Game::AddDrawList(DrawStage::Init, [&](){

		//auto h = (float)WindowState::mHeight;
		//auto w = (float)WindowState::mWidth;

		auto h = (float)800.0f;
		auto w = (float)1200.0f;

		float hh = h / 2.0f;
		float wh = w / 2.0f;

		auto s = gameObject->mTransform->Scale();
		float sx = s.x / w;
		float sy = s.y / h;
		auto p = gameObject->mTransform->Position();
		float px = (p.x - wh) / wh;
		float py = (-p.y + hh) / hh;

		//‚‚³‚ð‡‚í‚¹‚Ä•‚Ì”ä—¦‚ð‚ ‚í‚¹‚é
		{
			auto asx = 1200.0f / (float)WindowState::mWidth;
			//px -= asx/2.0f;
			sx *= asx;
		}

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

		auto mate = gameObject->GetComponent<MaterialComponent>();
		if (mate){
			auto material = mate->GetMaterial(0);
			impl->mTexMaterial.mCBMaterial.mParam.Diffuse = material.mCBMaterial.mParam.Diffuse;
		}

		Model& model = impl->mModel;

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		impl->mTexMaterial.SetTexture(impl->mFont.GetTexture(),0);

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

#ifdef _ENGINE_MODE
void TextComponent::CreateInspector(){

	std::function<void(std::string)> collback = [&](std::string name){
		ChangeText(name);
	};
	auto data = Window::CreateInspector();
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Text", &impl->mText, collback), data);
	Window::AddInspector(new TemplateInspectorDataSet<float>("FontSize", &mFontSize, [&](float f){
		ChangeFontSize(f);
	}), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("Center", &mCenter, [&](bool f){
		ChangeCenter(f);
	}), data);
	Window::ViewInspector("Text", this, data);
}
#endif

void TextComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(impl->mText);
#undef _KEY
}

void TextComponent::ChangeText(const std::string& text){
	impl->mText = text;
	impl->mFont.SetText(impl->mText, mCenter);
}
void TextComponent::ChangeFontSize(float size){
	mFontSize = size;
	impl->mFont.CreateFont_("", mFontSize);
	ChangeText(impl->mText);
}
void TextComponent::ChangeCenter(bool center){
	mCenter = center;
	ChangeText(impl->mText);
}