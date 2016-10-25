#pragma once

#include "Engine/AssetDatabase.h"
#include "Engine/Assets.h"
#include "IModelComponent.h"

class BoneModel;
class ModelComponent;
class IBoneMirrorComponent {
public:
	virtual ~IBoneMirrorComponent() {}

	virtual void ChangeTargetBone(GameObject target) = 0;
	virtual std::vector<std::string>& GetBoneNames() = 0;

	virtual int GetTargetBoneID() = 0;
	virtual void SetTargetBoneID(int id) = 0;
};

class BoneMirrorComponent :public IBoneMirrorComponent, public Component{
public:
	BoneMirrorComponent();
	~BoneMirrorComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void ChangeTargetBone(GameObject target) override;
	std::vector<std::string>& GetBoneNames() override;
	int GetTargetBoneID() override;
	void SetTargetBoneID(int id) override;
private:
	void transform(BoneModel* bone);

	GameObject m_TargetBone;
	weak_ptr<ModelComponent> m_ModelComponent;
	int m_TargetBoneID;
	std::vector<std::string> m_BoneNames;
	bool m_LoadNamesInitialize;

};
