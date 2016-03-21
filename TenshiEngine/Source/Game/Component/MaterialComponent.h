#pragma once

#include "IComponent.h"

class Material;

class MaterialComponent :public Component{
public:

	void LoadAssetResource(const std::string& path);
	void SaveAssetResource(const std::string& path);

	MaterialComponent();
	~MaterialComponent();

	void Initialize() override;

	void SetMaterial(UINT SetNo, Material& material);
	void CreateInspector() override;
	Material GetMaterial(UINT GetNo) const;

	void IO_Data(I_ioHelper* io) override{
#define _KEY(x) io->func( x , #x)
		_KEY(mMaterialPath);
#undef _KEY
	}

	std::string mTextureName;
	std::vector<Material> mMaterials;
	std::string mMaterialPath;
	std::string mShaderName;
};