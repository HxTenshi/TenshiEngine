
#include "ModelComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"



ModelComponent::ModelComponent()
{
	mModel =NULL;
}
ModelComponent::~ModelComponent(){
		if (mModel){
			mModel->Release();
			delete mModel;
		}
	}
void ModelComponent::Initialize(){
		if (!mModel){
			mModel = new Model();
		}
		if (!mModel->IsCreate()){
			mModel->Create(mFileName.c_str());
			if (!mBoneFileName.empty() || !mModel->mBoneModel){
				mModel->CreateBoneModel(mBoneFileName.c_str());
			}
		}
	}
void ModelComponent::Update(){

}

void ModelComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorLabelDataSet("Model"), data);
	std::function<void(std::string)> collbackpath = [&](std::string name){
		mFileName = name;
		if (mModel){
			mModel->Release();
			delete mModel;
		}
		mModel = new Model();
		mModel->Create(mFileName.c_str());
		if (!mBoneFileName.empty()){
			mModel->CreateBoneModel(mBoneFileName.c_str());
		}

	};
	std::function<void(std::string)> collbackbonepath = [&](std::string name){
		mBoneFileName = name;

		if (!mBoneFileName.empty()){
			mModel->CreateBoneModel(mBoneFileName.c_str());
		}
	};
	Window::AddInspector(new InspectorStringDataSet("Model", &mFileName, collbackpath), data);
	Window::AddInspector(new InspectorStringDataSet("Bone", &mBoneFileName, collbackbonepath), data);
	Window::ViewInspector("Model", this, data);
}

void ModelComponent::SetMatrix(){
	if (!mModel)return;
	mModel->mWorld = gameObject->mTransform->GetMatrix();
	mModel->Update();
}

void ModelComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
		_KEY(mFileName);
		_KEY(mBoneFileName);
#undef _KEY
}