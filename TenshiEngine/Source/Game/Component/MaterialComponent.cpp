
#include <functional>
#include "MaterialComponent.h"

#include "Window/Window.h"

#include"Graphic/Material/Material.h"
#include "Engine/Inspector.h"

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
	mAlbedo = XMFLOAT4(1, 1, 1, 1);
	mSpecular = XMFLOAT4(0, 0, 0, 1);
	mTexScale = XMFLOAT2(1, 1);
	mHeightPower = XMFLOAT2(2.0f, 1);
	mNormaleScale = XMFLOAT4(1, 1, 1, 1);
	mOffset = XMFLOAT2(0,0);
	mThickness = 0.0f;
	mEmissivePowor = 1.0f;
	mForwardRendering = false;
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

	if (mForwardRendering){
		mMaterials[0].CreateShader("EngineResource/ForwardRendering.fx");
	}
	//else{
	//	mMaterials[0].CreateShader("EngineResource/DeferredPrePass.fx");
	//}

	if (mShaderName != ""){
		mMaterials[0].CreateShader(mShaderName.c_str());
	}

	SetAlbedoColor(mAlbedo);
	SetSpecularColor(mSpecular);
	mMaterials[0].mCBMaterial.mParam.TexScale = mTexScale;
	mMaterials[0].mCBMaterial.mParam.HeightPower = mHeightPower;
	mMaterials[0].mCBMaterial.mParam.Ambient.w = mThickness;
	mMaterials[0].mCBMaterial.mParam.MNormaleScale = mNormaleScale;
	mMaterials[0].mCBMaterial.mParam.MOffset = mOffset;
	mMaterials[0].mCBMaterial.mParam.EmissivePowor = mEmissivePowor;

	if (mAlbedoTextureName != "")mMaterials[0].SetTexture(mAlbedoTextureName.c_str(), 0);
	if (mNormalTextureName != "")mMaterials[0].SetTexture(mNormalTextureName.c_str(), 1);
	if (mHeightTextureName != "")mMaterials[0].SetTexture(mHeightTextureName.c_str(), 2);
	if (mSpecularTextureName != "")mMaterials[0].SetTexture(mSpecularTextureName.c_str(), 3);
	if (mRoughnessTextureName != "")mMaterials[0].SetTexture(mRoughnessTextureName.c_str(), 4);
	if (mEmissiveTextureName != "")mMaterials[0].SetTexture(mEmissiveTextureName.c_str(), 5);
	
}

void MaterialComponent::SetMaterial(UINT SetNo, Material& material){
	if (mMaterials.size() <= SetNo)mMaterials.resize(SetNo + 1);
	mMaterials[SetNo] = material;

}
Material* MaterialComponent::GetMaterialPtr(UINT GetNo) const{
	if (mMaterials.size() <= GetNo){
		return NULL;
	}
	return const_cast<Material*>(&mMaterials[GetNo]);
}
Material MaterialComponent::GetMaterial(UINT GetNo) const{
	if (mMaterials.size() <= GetNo){
		return Material();
	}
	return mMaterials[GetNo];
}
#ifdef _ENGINE_MODE
void MaterialComponent::CreateInspector(){

	std::function<void(Color)> collbackAlb = [&](Color f){
		mAlbedo.x = f.r;
		mAlbedo.y = f.g;
		mAlbedo.z = f.b;
		mAlbedo.w = f.a;
		SetAlbedoColor(mAlbedo);
	};


	std::function<void(Color)> collbackSpec = [&](Color f){
		mSpecular.x = f.r;
		mSpecular.y = f.g;
		mSpecular.z = f.b;
		mSpecular.w = f.a;
		SetSpecularColor(mSpecular);
	};

	std::function<void(float)> collbackas = [&](float f){
		mSpecular.w = f;
		SetSpecularColor(mSpecular);
	};

	std::function<void(std::string)> collbacktex = [&](std::string name){
		mMaterials[0].SetTexture(name.c_str(), 0);
		mAlbedoTextureName = name;
	};
	std::function<void(std::string)> collbackntex = [&](std::string name){
		mMaterials[0].SetTexture(name.c_str(), 1);
		mNormalTextureName = name;
	};
	std::function<void(std::string)> collbackhtex = [&](std::string name){
		mMaterials[0].SetTexture(name.c_str(), 2);
		mHeightTextureName = name;
	};
	std::function<void(std::string)> collbackstex = [&](std::string name){
		mMaterials[0].SetTexture(name.c_str(), 3);
		mSpecularTextureName = name;
	};
	std::function<void(std::string)> collbackrtex = [&](std::string name){
		mMaterials[0].SetTexture(name.c_str(), 4);
		mRoughnessTextureName = name;
	};
	std::function<void(std::string)> collbacketex = [&](std::string name){
		mMaterials[0].SetTexture(name.c_str(), 5);
		mEmissiveTextureName = name;
	};
	std::function<void(std::string)> collbackpath = [&](std::string name){
		mMaterialPath = name;
		LoadAssetResource(mMaterialPath);
		//TextureName = (**mppMaterials)[0].mTexture[0].mFileName;
	};
	std::function<void(std::string)> collbacksha = [&](std::string name){
		mShaderName = name;
		mMaterials[0].CreateShader(mShaderName.c_str());
	};

	std::function<void(Vector2)> collbackTex = [&](Vector2 f){
		mTexScale.x = f.x;
		mTexScale.y = f.y;
		mMaterials[0].mCBMaterial.mParam.TexScale = mTexScale;
	};

	std::function<void(Vector2)> collbackOffset = [&](Vector2 f){
		mOffset.x = f.x;
		mOffset.y = f.y;
		mMaterials[0].mCBMaterial.mParam.MOffset = mOffset;
	};

	std::function<void(float)> collbackH = [&](float f){
		mHeightPower.x = f;
		mMaterials[0].mCBMaterial.mParam.HeightPower = mHeightPower;
	};
	std::function<void(float)> collbackHDR = [&](float f){
		mHeightPower.y = f;
		mMaterials[0].mCBMaterial.mParam.HeightPower = mHeightPower;
	};
	std::function<void(float)> collbackThick = [&](float f){
		mThickness = f;
		mMaterials[0].mCBMaterial.mParam.Ambient.w = mThickness;
	};

	std::function<void(Vector3)> collbackNormalScale = [&](Vector3 f){
		mNormaleScale.x = f.x;
		mNormaleScale.y = f.y;
		mNormaleScale.z = f.z;
		mMaterials[0].mCBMaterial.mParam.MNormaleScale = mNormaleScale;
	};

	Inspector ins("Material",this);
	
	ins.Add("Material", &mMaterialPath, collbackpath);
	ins.Add("UseAlpha", &mForwardRendering, [&](bool f)
	{
		mForwardRendering = f; 
		if (f){
			mMaterials[0].CreateShader("EngineResource/ForwardRendering.fx");
		}
		else{
			mMaterials[0].CreateShader("EngineResource/DeferredPrePass.fx");
		}

	});

	auto alb = Color(mAlbedo);
	ins.Add("Albedo", &alb, collbackAlb);

	auto spec = Color(mSpecular);
	ins.Add("Specular", &spec, collbackSpec);
	ins.AddSlideBar("Roughness", 0, 1, &mSpecular.w, collbackas);
	ins.Add("AlbedoTextre", &mAlbedoTextureName, collbacktex);
	ins.Add("NormalTextre", &mNormalTextureName, collbackntex);
	ins.Add("HeightTextre", &mHeightTextureName, collbackhtex);
	ins.Add("SpecularTextre", &mSpecularTextureName, collbackstex);
	ins.Add("RoughnessTextre", &mRoughnessTextureName, collbackrtex);
	ins.Add("EmissiveTextre", &mEmissiveTextureName, collbacketex);
	ins.Add("EmissivePowor", &mEmissivePowor, [&](float f){ mEmissivePowor = f; mMaterials[0].mCBMaterial.mParam.EmissivePowor = f; });

	auto tex = Vector2(mTexScale);
	ins.Add("TextureScale", &tex, collbackTex);

	auto texNormal = Vector3(mNormaleScale);
	ins.Add("NormaleScale", &texNormal, collbackNormalScale);

	auto off = Vector2(mOffset);
	ins.Add("Offset", &off, collbackOffset);

	ins.AddSlideBar("HightPower", -10, 10, &mHeightPower.x, collbackH);
	ins.Add("HDR", &mHeightPower.y, collbackHDR);
	ins.Add("Thickness", &mThickness, collbackThick);
	ins.Add("Shader", &mShaderName, collbacksha);
	ins.Complete();

}
#endif

void MaterialComponent::SetAlbedoColor(const XMFLOAT4& col){
	mAlbedo = col;
	mMaterials[0].mCBMaterial.mParam.Diffuse = mAlbedo;
}
void MaterialComponent::SetSpecularColor(const XMFLOAT4& col){
	mSpecular = col;
	mMaterials[0].mCBMaterial.mParam.Specular = mSpecular;
}

void MaterialComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mMaterialPath);
	_KEY(mShaderName);
	_KEY(mForwardRendering);
	_KEY(mAlbedo.x);
	_KEY(mAlbedo.y);
	_KEY(mAlbedo.z);
	_KEY(mAlbedo.w);
	_KEY(mSpecular.x);
	_KEY(mSpecular.y);
	_KEY(mSpecular.z);
	_KEY(mSpecular.w);
	_KEY(mAlbedoTextureName);
	_KEY(mNormalTextureName);
	_KEY(mHeightTextureName);
	_KEY(mSpecularTextureName);
	_KEY(mRoughnessTextureName);
	_KEY(mEmissiveTextureName);
	_KEY(mTexScale.x);
	_KEY(mTexScale.y);
	_KEY(mEmissivePowor);
	_KEY(mOffset.x);
	_KEY(mOffset.y);
	_KEY(mHeightPower.x);
	_KEY(mHeightPower.y);
	_KEY(mNormaleScale.x);
	_KEY(mNormaleScale.y);
	_KEY(mNormaleScale.z);
	_KEY(mNormaleScale.w);
	_KEY(mThickness);
#undef _KEY
}