
#include "MeshComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "TransformComponent.h"

#include "Graphic/Model/Model.h"
#include "ModelComponent.h"


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
	SetMesh(m_FileName, m_ID);

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

void MeshComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	std::function<void(std::string)> collbackpath = [&](std::string name){
		m_FileName = name;
		SetMesh(m_FileName, m_ID);

	};
	std::function<void(float)> collbackid = [&](float id){
		m_ID = id;
		SetMesh(m_FileName, m_ID);
	};
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Mesh", &m_FileName, collbackpath), data);
	Window::AddInspector(new TemplateInspectorDataSet<int>("ID", &m_ID, collbackid), data);
	Window::ViewInspector("MeshComponent", this, data);
}

void MeshComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(m_FileName);
	_KEY(m_ID);
#undef _KEY
}


void MeshComponent::SetMesh(std::string fileName, int id){
	m_FileName = fileName;
	m_ID = id;
	m_ModelMesh->Create(m_FileName, m_ID);
}