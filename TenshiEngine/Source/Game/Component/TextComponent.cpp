

#include "MySTL/Ptr.h"
#include "Graphic/Font/Font.h"
#include "Graphic/Material/Material.h"
#include "TextComponent.h"
#include "TextureModelComponent.h"
#include "MaterialComponent.h"
#include "Game/Game.h"
#include "Game/SettingObject/Canvas.h"

#include "Engine/Inspector.h"

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
	m_Center = XMFLOAT2(0.5f, 0.5f);
}
TextComponent::~TextComponent()
{
	impl->mModel.Release();
	delete impl;
}

void TextComponent::Initialize(){

	impl->mFont.CreateFont_("", mFontSize);
	
	auto mBackScale = gameObject->mTransform->Scale();
	impl->mFont.CreateTexture_((UINT)mBackScale.x, (UINT)mBackScale.y);

	impl->mModel.Create("EngineResource/TextureModel.tesmesh");
	ChangeText(impl->mText);
}
void TextComponent::Finish(){
}
#ifdef _ENGINE_MODE
void TextComponent::EngineUpdate(){

	auto s = gameObject->mTransform->Scale();
	if (mBackScale.x != s.x || mBackScale.y != s.y){
		impl->mFont.CreateTexture_(s.x, s.y);
		ChangeText(impl->mText);
		mBackScale = s;
	}

	DrawTextUI();
}
#endif
void TextComponent::Update(){
	DrawTextUI();
}

void TextComponent::DrawTextUI(){

	Game::AddDrawList(DrawStage::Init, [&](){

		auto s = gameObject->mTransform->Scale();
		auto p = gameObject->mTransform->Position();
		auto r = gameObject->mTransform->Rotate();

		XMFLOAT2 center = m_Center;
		center.x = (center.x * -2.0f + 1.0f);
		center.y = (center.y * 2.0f - 1.0f);
		auto centerMatrix = XMMatrixTranslation(center.x, center.y, 0.0f);


		s.x /= Canvas::GetWidth();
		s.y /= Canvas::GetHeight();
		auto scaleMatrix = XMMatrixScaling(s.x, s.y, 0.0f);


		auto rotateMatrix = XMMatrixRotationRollPitchYawFromVector(r);


		p.x = p.x * 2 / Canvas::GetWidth() - 1.0f;
		p.y = p.y * 2 / Canvas::GetHeight() - 1.0f;
		auto transMatrix = XMMatrixTranslation(p.x, p.y, 0.0f);


		auto mat = XMMatrixMultiply(centerMatrix, scaleMatrix);
		mat = XMMatrixMultiply(mat, rotateMatrix);
		mat = XMMatrixMultiply(mat, transMatrix);

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
	Inspector ins("Text",this);
	ins.AddEnableButton(this);
	ins.Add("Text", &impl->mText, collback);
	ins.Add("FontSize", &mFontSize, [&](float f){
		ChangeFontSize(f);
	});
	ins.Add("Center", &mCenter, [&](bool f){
		ChangeCenter(f);
	});
	Vector2 vec(m_Center);
	ins.Add("TextureCenter", &vec, [&](Vector2 v) {
		SetTextureCenter(XMFLOAT2(v.x, v.y));
	});

	ins.Complete();
}
#endif

void TextComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(impl->mText);
	_KEY(m_Center.x);
	_KEY(m_Center.y);
	_KEY(mCenter);
	_KEY(mFontSize);
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

void TextComponent::SetTextureCenter(const XMFLOAT2& center) {
	m_Center = center;
}