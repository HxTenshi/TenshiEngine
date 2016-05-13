
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
		if (!mModel->mBoneModel){
			mModel->CreateBoneModel(mBoneFileName.c_str());
		}
	}
}
void ModelComponent::Start(){

}
void ModelComponent::Finish(){
	m_MeshComVector.clear();
}
void ModelComponent::Update(){

}

void ModelComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	std::function<void(std::string)> collbackpath = [&](std::string name){
		mFileName = name;
		if (mModel){
			mModel->Release();
			delete mModel;
		}
		mModel = new Model();
		mModel->Create(mFileName.c_str());
		mModel->CreateBoneModel(mBoneFileName.c_str());

	};
	std::function<void(std::string)> collbackbonepath = [&](std::string name){
		mBoneFileName = name;
		mModel->CreateBoneModel(mBoneFileName.c_str());
	};
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Model", &mFileName, collbackpath), data);
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Bone", &mBoneFileName, collbackbonepath), data);

	std::function<void(void)> collbackbutton = [&](){
		ExpanderMesh();
	};
	Window::AddInspector(new InspectorButtonDataSet("ExpanderMesh", collbackbutton), data);
	Window::ViewInspector("ModelComponent", this, data);
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

#include "Game/Game.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
void ModelComponent::ExpanderMesh(){
	int num = mModel->GetMeshNum();

	for (int i = 0; i < num; i++){
		auto obj = new Actor();
		obj->mTransform = obj->AddComponent<TransformComponent>();
		Game::AddObject(obj);
		obj->mTransform->SetParent(gameObject);
		obj->Name("Mesh:"+std::to_string(i));
		obj->AddComponent<MaterialComponent>();
		auto com = obj->AddComponent<MeshComponent>();
		com->SetMesh(mFileName, i);
	}
}


void ModelComponent::AddMeshComponent(weak_ptr<MeshComponent> meshCom){
	m_MeshComVector.push_back(meshCom);
}