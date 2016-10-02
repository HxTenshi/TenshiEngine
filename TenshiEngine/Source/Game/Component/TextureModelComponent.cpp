

#include "TextureModelComponent.h"
#include "MaterialComponent.h"
#include "MovieComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"
#include "Game/Game.h"

#include "Engine/Inspector.h"

#include "Game/SettingObject/Canvas.h"

TextureModelComponent::TextureModelComponent()
{
	mModel= NULL;
	mMaterial = NULL;
	m_Center = XMFLOAT2(0.5f,0.5f);
}
TextureModelComponent::~TextureModelComponent(){
	if (mModel){
		mModel->Release();
		delete mModel;
		mModel = NULL;
	}
	if (mMaterial){
		delete mMaterial;
		mMaterial = NULL;
	}
}

void TextureModelComponent::Initialize(){
	if (!mModel){
		mModel = new Model();
	}
	if (!mModel->IsCreate()){
		mModel->Create("EngineResource/TextureModel.tesmesh");

	}
	if (!mMaterial){
		mMaterial = new Material();
	}

	m_TextureAsset.Load(m_TextureAsset.m_Hash);

	if (!mMaterial->IsCreate()){
		mMaterial->Create("EngineResource/Texture.fx");
		mMaterial->SetTexture(m_TextureAsset, 0);
	}
	//if (!mMaterial->IsCreate()){
	//	mMaterial->Create("EngineResource/Texture.fx");
	//	mMaterial->SetTexture(mTextureName.c_str(), 0);
	//
	//	if (mTextureHash != ""){
	//		MD5::MD5HashCoord hash(mTextureHash.c_str());
	//		Texture tex;
	//		if (tex.Create(hash) == S_OK){
	//			mMaterial->SetTexture(tex, 0);
	//		}
	//			
	//	}
	//}
}

void TextureModelComponent::Start(){
}

#ifdef _ENGINE_MODE
void TextureModelComponent::EngineUpdate(){
	Update();

}
#endif
void TextureModelComponent::Update(){


	if (!mModel)return;

	auto mate = gameObject->GetComponent<MaterialComponent>();
	auto movie = gameObject->GetComponent<MovieComponent>();
	if (!m_TextureAsset.IsLoad() && !mate && !movie)return;

	Game::AddDrawList(DrawStage::Init, std::function<void()>([&](){
		SetMatrix();
	}));
	Game::AddDrawList(DrawStage::UI, std::function<void()>([mate, movie, this](){

		if (mate){
			auto material= mate->GetMaterial(0);
			mMaterial->SetTexture(material.mTexture[0]);
			mMaterial->mCBMaterial.mParam.Diffuse = material.mCBMaterial.mParam.Diffuse;
			//mTextureName = "";
		}
		if (movie){
			auto tex = movie->GetTexture();
			if (tex){
				mMaterial->SetTexture(*tex);
				//mTextureName = "";
			}
		}
		SetMatrix();
		
		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		mModel->Draw(render->m_Context, *mMaterial);
	}));
}
void TextureModelComponent::Finish(){
}

void TextureModelComponent::SetMatrix(){
	if (!mModel)return;
	
	//auto h = (float)WindowState::mHeight;
	//auto w = (float)WindowState::mWidth;
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


	//auto h = (float)800.0f;
	//auto w = (float)1200.0f;
	//
	//float hh = h / 2.0f;
	//float wh = w / 2.0f;

	//float sx = s.x / w;
	//float sy = s.y / h;
	//float px = (p.x - wh) / wh;
	//float py = (-p.y + hh) / hh;

	////‚‚³‚ð‡‚í‚¹‚Ä•‚Ì”ä—¦‚ð‚ ‚í‚¹‚é
	//{
	//	auto asx = 1200.0f / (float)WindowState::mWidth;
	//	//px -= asx/2.0f;
	//	sx *= asx;
	//}


	//auto pos = XMVectorSet(px, py, 0, 1);
	//auto rot = gameObject->mTransform->Rotate();
	//auto scl = XMVectorSet(sx, sy, 0, 1);

	//auto mat = XMMatrixMultiply(
	//	XMMatrixMultiply(
	//	XMMatrixRotationRollPitchYawFromVector(rot),
	//	XMMatrixScalingFromVector(scl)),
	//	XMMatrixTranslationFromVector(pos));

	mModel->mWorld = mat;
	mModel->Update();
}

#ifdef _ENGINE_MODE
void TextureModelComponent::CreateInspector(){

	//std::function<void(std::string)> collbacktex = [&](std::string name){
	//	mTextureName = name;
	//	mMaterial->SetTexture(mTextureName.c_str(),0);
	//};

	Inspector ins("TextureModel",this);
	ins.AddEnableButton(this);
	//ins.Add("Texture", &mTextureName, collbacktex);
	//
	//ins.Add("TextureHash", &mTextureHash, [&](std::string name){
	//	MD5::MD5HashCoord hash;
	//	if (AssetDataBase::FilePath2Hash(name.c_str(), hash)){
	//		mTextureHash = hash.GetString();
	//	}
	//	else{
	//		mTextureHash = "";
	//	}
	//	mMaterial->SetTexture(name.c_str(), 0);
	//});

	ins.Add("Texture", &m_TextureAsset,[&](){
		mMaterial->SetTexture(m_TextureAsset, 0);
	});
	Vector2 vec(m_Center);
	ins.Add("Center", &vec, [&](Vector2 v) {
		SetCenter(XMFLOAT2(v.x, v.y));
	});
	ins.Complete();

}
#endif

void TextureModelComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(m_TextureAsset);
	_KEY(m_Center.x);
	_KEY(m_Center.y);
#undef _KEY
}

void TextureModelComponent::SetTexture(TextureAsset& asset){
	//mTextureName = filename;
	m_TextureAsset = asset;
	mMaterial->SetTexture(asset, 0);
}

void TextureModelComponent::SetCenter(const XMFLOAT2& center) {
	m_Center = center;
}