

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
	bool ChangeText = false;

	Material mTexMaterial;

};

TextComponent::TextComponent()
{
	impl = new TextComponentMember();

	mFontSize = 48.0f;
	mCenter = false;
	m_Center = XMFLOAT2(0.5f, 0.5f);
	m_FontName = "ƒƒCƒŠƒI";
	m_FonstSelect = 0;
}
TextComponent::~TextComponent()
{
	impl->mModel.Release();
	delete impl;
}

void TextComponent::Initialize(){

	impl->mTexMaterial.Create("EngineResource/texture.fx");
	AddFont(m_FontFile);
	ChangeFont(m_FontName);
	
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

	Game::AddDrawListZ(DrawStage::UI,gameObject, [&](){

		auto mate = gameObject->GetComponent<MaterialComponent>();
		if (mate){
			auto material = mate->GetMaterial(0);
			impl->mTexMaterial.SetAlbedo(material.GetAlbedo());
		}

		Model& model = impl->mModel;

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);


		if (impl->ChangeText) {
			impl->mFont.SetText(impl->mText, mCenter);
		}
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

	ins.Add("FontFile", &m_FontFile, [&](std::string s) {
		AddFont(s);
	});
	ins.Add("FontName", &m_FontName, [&](std::string s) {
		ChangeFont(s);
	});
	auto fonts = FontFileData::GetFonts();
	ins.AddSelect("Fonts", &m_FonstSelect,fonts, [&](int s) {
		auto fonts = FontFileData::GetFonts();
		ChangeFont(fonts[s]);
	});
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


	_KEY(m_FontFile);
	_KEY(m_FontName);
	_KEY(impl->mText);
	_KEY(m_Center.x);
	_KEY(m_Center.y);
	_KEY(mCenter);
	_KEY(mFontSize);
#undef _KEY
}

void TextComponent::AddFont(const std::string & filePath)
{
	bool f = false;
	if (m_FontFile != "") {
		f = true;
	}
	m_FontFile = filePath;
	impl->mFont.AddFont(m_FontFile.c_str());
	if (m_FontFile != "" || f) {
		FontFileData::ClearFonts();
#ifdef _ENGINE_MODE
		Window::ClearInspector();
#endif
	}
}

void TextComponent::ChangeFont(const std::string & fontName)
{
	m_FontName = fontName;
	impl->mFont.CreateFont_(m_FontName.c_str(), mFontSize);
	ChangeText(impl->mText);
}

void TextComponent::ChangeText(const std::string& text){
	impl->mText = text;
	impl->ChangeText = true;
}
void TextComponent::ChangeFontSize(float size){
	mFontSize = size;
	impl->mFont.CreateFont_(m_FontName.c_str(), mFontSize);
	ChangeText(impl->mText);
}
void TextComponent::ChangeCenter(bool center){
	mCenter = center;
	ChangeText(impl->mText);
}

void TextComponent::SetTextureCenter(const XMFLOAT2& center) {
	m_Center = center;
}