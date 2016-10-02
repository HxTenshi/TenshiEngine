#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"
#include "Engine/AssetDataBase.h"

class ModelMesh;

class MeshComponent :public Component, public enable_shared_from_this<MeshComponent>{
public:
	MeshComponent();
	~MeshComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;
	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void SetMesh(MeshAsset& asset, int id);
	ModelMesh* GetModelMesh(){
		return m_ModelMesh;
	}
private:
	ModelMesh* m_ModelMesh;
	MeshAsset mMesh;
	int m_ID;
};
