
#include "MeshComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"
#include "ModelComponent.h"

#include "Engine/Inspector.h"

MeshComponent::MeshComponent()
{
	m_ModelMesh = NULL;
	m_ID = 0;
}
MeshComponent::~MeshComponent(){
	if (m_ModelMesh){
		delete m_ModelMesh;
		m_ModelMesh = NULL;
	}
}
void MeshComponent::Initialize(){
	if (!m_ModelMesh){
		m_ModelMesh = new ModelMesh();
	}
	mMesh.Load(mMesh.m_Hash);
	SetMesh(mMesh, m_ID);

}
void MeshComponent::Start(){
	auto p = gameObject->mTransform->GetParent();
	if (p){
		auto com = p->GetComponent<ModelComponent>();
		if (com){
			com->AddMeshComponent(this->shared_from_this());
		}
	}

	auto mate = gameObject->GetComponent<MaterialComponent>();
	m_ModelMesh->SetMaterialComponent(mate);
}
void MeshComponent::Finish(){

}
void MeshComponent::Update(){
	auto mate = gameObject->GetComponent<MaterialComponent>();
	m_ModelMesh->SetMaterialComponent(mate);
}

#ifdef _ENGINE_MODE
void MeshComponent::CreateInspector(){

	std::function<void(int)> collbackid = [&](int id){
		m_ID = id;
		SetMesh(mMesh, m_ID);
	};
	Inspector ins("Mesh",this);
	ins.AddEnableButton(this);
	ins.Add("Mesh", &mMesh, [&](){
		SetMesh(mMesh, m_ID);
	});
	ins.Add("ID", &m_ID, collbackid);
	ins.Complete();
}
#endif

void MeshComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mMesh);
	_KEY(m_ID);
#undef _KEY
}

void MeshComponent::SetMesh(MeshAsset& asset, int id){
	m_ID = id;
	mMesh = asset;
	m_ModelMesh->Create(mMesh, m_ID);
}