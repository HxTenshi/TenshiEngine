

#include "TextureModelComponent.h"
#include "MaterialComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"
#include "Game/Game.h"

TextureModelComponent::TextureModelComponent()
{
	mModel= NULL;
	mMaterial = NULL;
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
	if (!mMaterial->IsCreate()){
		mMaterial->Create("EngineResource/Texture.fx");
		mMaterial->SetTexture(mTextureName.c_str(), 0);
	}
}

void TextureModelComponent::Start(){
}

void TextureModelComponent::EngineUpdate(){
	Update();

}
void TextureModelComponent::Update(){


	if (!mModel)return;

	auto mate = gameObject->GetComponent<MaterialComponent>();
	if (mTextureName == "" && !mate)return;

	Game::AddDrawList(DrawStage::Init, std::function<void()>([&](){
		SetMatrix();
	}));
	Game::AddDrawList(DrawStage::UI, std::function<void()>([mate,this](){

		if (mate){
			auto material= mate->GetMaterial(0);
			mMaterial->SetTexture(material.mTexture[0]);
			mMaterial->mCBMaterial.mParam.Diffuse = material.mCBMaterial.mParam.Diffuse;
			mTextureName = "";
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

	mModel->mWorld = mat;
	mModel->Update();
}

void TextureModelComponent::CreateInspector(){

	std::function<void(std::string)> collbacktex = [&](std::string name){
		mTextureName = name;
		mMaterial->SetTexture(mTextureName.c_str(),0);
	};

	auto data = Window::CreateInspector();
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Texture", &mTextureName, collbacktex), data);
	Window::ViewInspector("TextureModel", this, data);
}

void TextureModelComponent::IO_Data(I_ioHelper* io){
	(void)io;
#define _KEY(x) io->func( x , #x)
	_KEY(mTextureName);
#undef _KEY
}