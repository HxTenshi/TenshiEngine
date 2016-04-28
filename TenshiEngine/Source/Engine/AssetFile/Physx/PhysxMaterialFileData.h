#pragma once
namespace physx{
	class PxMaterial;
}

#include "../AssetFileData.h"


class PhysxMaterialFileData : public AssetFileData{
public:
	PhysxMaterialFileData();
	~PhysxMaterialFileData();

	void Create(const char* filename) override;
	void FileUpdate() override;
	void SaveFile();

	physx::PxMaterial* GetMaterial() const;

	float m_Param_sf;
	float m_Param_df;
	float m_Param_r;

private:
	//ÉRÉsÅ[ã÷é~
	PhysxMaterialFileData(const PhysxMaterialFileData&) = delete;
	PhysxMaterialFileData& operator=(const PhysxMaterialFileData&) = delete;

	physx::PxMaterial* m_Material;
};
