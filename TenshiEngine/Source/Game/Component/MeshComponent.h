#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"

class ModelMesh;

class MeshComponent :public Component, public enable_shared_from_this<MeshComponent>{
public:
	MeshComponent();
	~MeshComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;
	void Update() override;

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;

	void SetMesh(std::string fileName,int id);
	ModelMesh* GetModelMesh(){
		return m_ModelMesh;
	}
private:
	ModelMesh* m_ModelMesh;
	std::string m_FileName;
	int m_ID;
};
