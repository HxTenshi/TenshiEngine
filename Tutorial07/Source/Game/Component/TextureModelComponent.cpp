

#include "TextureModelComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"

TextureModelComponent::TextureModelComponent()
{
	mModel= NULL;
}
TextureModelComponent::~TextureModelComponent(){
	if (mModel){
		mModel->Release();
		delete mModel;
	}
}

void TextureModelComponent::Initialize(){
	if (!mModel){
		mModel = new Model();
	}
	if (!mModel->IsCreate()){
		mModel->Create("EngineResource/TextureModel.tesmesh");
	}
}

void TextureModelComponent::SetMatrix(){
	if (!mModel)return;
	mModel->mWorld = gameObject->mTransform->GetMatrix();
	mModel->Update();
}

void TextureModelComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::ViewInspector("TextureModel", this, data);
}

void TextureModelComponent::IO_Data(I_ioHelper* io){
	(void)io;
#define _KEY(x) io->func( x , #x)
#undef _KEY
}