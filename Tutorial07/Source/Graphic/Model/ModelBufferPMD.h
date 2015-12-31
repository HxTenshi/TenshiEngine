#pragma once

#include "ModelBuffer.h"
#include "Graphic/Loader/Model/PMD.h"

class Model;
class MaterialComponent;

class ModelBufferPMD : public ModelBuffer{
public:

	HRESULT Create(const char* FileName, Model* mpModel) override;
private:
	HRESULT createMaterial(unsigned long count, pmd::t_material* material, const std::string& sFileName, Model* mpModel);

	HRESULT createBone(unsigned long count, pmd::t_bone* bone);

	HRESULT createIk(unsigned long count, pmd::t_ik_data* ik);
};


#include "Graphic/Loader/Model/PMX.h"
class ModelBufferPMX : public ModelBuffer{
public:

	HRESULT Create(const char* FileName, Model* mpModel) override;
private:
	HRESULT createMaterial(unsigned long count, pmx::t_material* material, std::string* textures, const std::string& sFileName, Model* mpModel);

	HRESULT createBone(unsigned long count, pmx::t_bone* bone);

	HRESULT createIk(DWORD ikCount,UINT bidx, pmx::t_ik_data& ik);
};
