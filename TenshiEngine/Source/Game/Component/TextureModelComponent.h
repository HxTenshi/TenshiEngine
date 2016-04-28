#pragma once

#include "IModelComponent.h"

class Model;
class Material;

class TextureModelComponent : public IModelComponent{
public:
	TextureModelComponent();
	~TextureModelComponent();

	void Initialize() override;
	void Start() override;
	void EngineUpdate() override;
	void Update() override;
	void Finish() override;

	void SetMatrix() override;

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;
private:
	Material* mMaterial;
	std::string mTextureName;
};