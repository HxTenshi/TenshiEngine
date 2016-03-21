
#include <functional>
#include "MaterialComponent.h"

#include "Window/Window.h"

#include"Graphic/Material/Material.h"

void MaterialComponent::LoadAssetResource(const std::string& path){

	File f(path);
	if (!f)return;

	mMaterials.clear();

	std::string name;
	f.In(&name);
	int materialnum;
	f.In(&materialnum);
	mMaterials.resize(materialnum);
	for (int i = 0; i < materialnum; i++){
		auto& material = mMaterials[i];
		auto hr = material.Create();
		if (FAILED(hr))return;

		int texnum;
		f.In(&texnum);
		for (int i = 0; i < texnum; i++){
			int slot;
			f.In(&slot);
			std::string filename;
			f.In(&filename);
			auto ioc = filename.find("$");
			while (std::string::npos != ioc){
				filename.replace(ioc, 1, " ");
				ioc = filename.find("$");
			}

			material.SetTexture(filename.c_str(), slot);
		}
	}
}
void MaterialComponent::SaveAssetResource(const std::string& path){

	File f;
	if (!f.Open(path))
		f.FileCreate();
	f.Clear();
	if (!f)return;
	std::string name = "Material";
	f.Out(name);
	int num = mMaterials.size();
	f.Out(num);
	for (int i = 0; i < num; i++){
		Material &mate = mMaterials[i];
		mate.ExportData(f);
	}
}

MaterialComponent::MaterialComponent(){
	mMaterials.resize(1);
}
MaterialComponent::~MaterialComponent(){
}

void MaterialComponent::Initialize(){
	auto& mate = mMaterials[0];
	if (!mate.IsCreate()){
		if (mMaterialPath != ""){
			LoadAssetResource(mMaterialPath);
			for (auto& m : mMaterials){
				if (!m.IsCreate()){
					m.Create();
				}
			}
		}
		else{
			auto& m = mMaterials[0];
			m.Create();
		}
	}
}

void MaterialComponent::SetMaterial(UINT SetNo, Material& material){
	if (mMaterials.size() <= SetNo)mMaterials.resize(SetNo + 1);
	mMaterials[SetNo] = material;

}
Material MaterialComponent::GetMaterial(UINT GetNo) const{
	if (mMaterials.size() <= GetNo){
		return Material();
	}
	return mMaterials[GetNo];
}
std::string TextureName = "";
void MaterialComponent::CreateInspector(){

	auto data = Window::CreateInspector();
	std::function<void(float)> collbackx = [&](float f){
		mMaterials[0].mCBMaterial.mParam.Diffuse.x = f;
	};

	std::function<void(float)> collbacky = [&](float f){
		mMaterials[0].mCBMaterial.mParam.Diffuse.y = f;
	};

	std::function<void(float)> collbackz = [&](float f){
		mMaterials[0].mCBMaterial.mParam.Diffuse.z = f;
	};
	std::function<void(std::string)> collbacktex = [&](std::string name){
		mMaterials[0].SetTexture(name.c_str(), 0);
		TextureName = name;
	};
	std::function<void(std::string)> collbackpath = [&](std::string name){
		mMaterialPath = name;
		LoadAssetResource(mMaterialPath);
		//TextureName = (**mppMaterials)[0].mTexture[0].mFileName;
	};
	std::function<void(std::string)> collbacksha = [&](std::string name){
		mShaderName = name;
		mMaterials[0].Create(mShaderName.c_str());
	};

	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Material", &mMaterialPath, collbackpath), data);
	Window::AddInspector(new InspectorSlideBarDataSet("r", 0.0f, 1.0f, &mMaterials[0].mCBMaterial.mParam.Diffuse.x, collbackx), data);
	Window::AddInspector(new InspectorSlideBarDataSet("g", 0.0f, 1.0f, &mMaterials[0].mCBMaterial.mParam.Diffuse.y, collbacky), data);
	Window::AddInspector(new InspectorSlideBarDataSet("b", 0.0f, 1.0f, &mMaterials[0].mCBMaterial.mParam.Diffuse.z, collbackz), data);
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Textre", &TextureName, collbacktex), data);
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Shader", &mShaderName, collbacksha), data);
	Window::ViewInspector("Material", this, data);
}
