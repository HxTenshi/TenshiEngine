#pragma once

#include "Engine/AssetDatabase.h"
#include "IModelComponent.h"

class Model;
class _IModelComponent{
public:
	virtual ~_IModelComponent(){}
	virtual void Load(MeshAsset& asset) = 0;
	virtual void Load(BoneAsset& asset) = 0;
};

class ModelComponent :public IModelComponent, public _IModelComponent{
public:
	ModelComponent();
	~ModelComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;

	void SetMatrix() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;


	void Load(MeshAsset& asset)override;
	void Load(BoneAsset& asset)override;

	MeshAsset mMesh;
	BoneAsset mBone;

	void AddMeshComponent(weak_ptr<MeshComponent> meshCom);
private:
	void ExpanderMesh();

};
