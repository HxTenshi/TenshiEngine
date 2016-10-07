#pragma once

#include "IComponent.h"
#include "IModelComponent.h"

#include "Engine/AssetDataBase.h"
#include "Engine/Assets.h"
#include "Graphic/Material/Material.h"

class Model;
class Material;

class GizumoComponent : public Component {
public:
	GizumoComponent();
	~GizumoComponent();

	void Initialize() override;
	void Start() override;
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

private:
	Model* mModel;

	Material m_Material;
	TextureAsset m_TextureAsset;
};