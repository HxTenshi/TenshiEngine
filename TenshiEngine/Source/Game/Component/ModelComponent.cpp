
#include "ModelComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"

#include "Engine/Inspector.h"
#include "Engine/AssetLoad.h"

ModelComponent::ModelComponent()
{
	mModel =NULL;
}
ModelComponent::~ModelComponent(){
		if (mModel){
			mModel->Release();
			delete mModel;
		}
		mMesh.Free();
		mBone.Free();
	}
void ModelComponent::Initialize(){
	
	AssetLoad::Instance(mMesh.m_Hash, mMesh);
	AssetLoad::Instance(mBone.m_Hash, mBone);
	Load(mMesh);
	Load(mBone);
}
void ModelComponent::Start(){

}
void ModelComponent::Finish(){
	m_MeshComVector.clear();
}
#ifdef _ENGINE_MODE
void ModelComponent::EngineUpdate(){
}
#endif
void ModelComponent::Update(){
}

#ifdef _ENGINE_MODE
void ModelComponent::CreateInspector(){

	Inspector ins("Model", this);
	ins.Add("Model", &mMesh, [&](){
		Load(mMesh);
	});
	ins.Add("Bone", &mBone, [&](){
		Load(mBone);
	});
	std::function<void(void)> collbackbutton = [&](){
		ExpanderMesh();
	};
	ins.AddButton("ExpanderMesh", collbackbutton);
	ins.Complete();
}
#endif

void ModelComponent::SetMatrix(){
	if (!mModel)return;
	mModel->mWorld = gameObject->mTransform->GetMatrix();
	mModel->Update();
}

void ModelComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mMesh);
	_KEY(mBone);
#undef _KEY
}

#include "Game/Game.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
void ModelComponent::ExpanderMesh(){
	int num = mModel->GetMeshNum();

	for (int i = 0; i < num; i++){
		auto obj = make_shared<Actor>();
		obj->mTransform = obj->AddComponent<TransformComponent>();
		obj->mTransform->SetParentUniqueID(gameObject->GetUniqueID());
		obj->Name("Mesh:"+std::to_string(i));
		obj->AddComponent<MaterialComponent>();
		auto com = obj->AddComponent<MeshComponent>();
		com->SetMesh(mMesh, i);
		Game::AddObject(obj);
	}
}


void ModelComponent::AddMeshComponent(weak_ptr<MeshComponent> meshCom){
	m_MeshComVector.push_back(meshCom);
}



void ModelComponent::Load(MeshAsset& asset){
	mMesh = asset;

	if (mModel){
		mModel->Release();
		delete mModel;
	}
	mModel = new Model();
	mModel->Create(mMesh);
	if (mBone.IsLoad())
		mModel->CreateBoneModel(mBone);
}
void ModelComponent::Load(BoneAsset& asset){
	mBone = asset;
	mModel->CreateBoneModel(mBone);
}