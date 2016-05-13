#pragma once

#include "IModelComponent.h"

class Model;

class ModelComponent :public IModelComponent{
public:
	ModelComponent();
	~ModelComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;
	void Update() override;

	void SetMatrix() override;

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;

	std::string mFileName;
	std::string mBoneFileName;

	void AddMeshComponent(weak_ptr<MeshComponent> meshCom);
private:
	void ExpanderMesh();

};
