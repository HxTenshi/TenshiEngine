#pragma once
#include "../AssetFileData.h"

class Material;

class MaterialFileData : public AssetFileData {
public:
	MaterialFileData();
	~MaterialFileData();

	bool Create(const char* filename) override;

	Material* GetMaterial() const {
		return m_Material;
	}

	void SaveFile();

private:


	//ÉRÉsÅ[ã÷é~
	MaterialFileData(const MaterialFileData&) = delete;
	MaterialFileData& operator=(const MaterialFileData&) = delete;

	Material* m_Material;
};
