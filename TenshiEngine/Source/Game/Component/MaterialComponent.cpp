
#include <functional>
#include "MaterialComponent.h"

#include "Window/Window.h"

#include"Graphic/Material/Material.h"
#include "Engine/Inspector.h"
#include "Engine/AssetLoad.h"
#include "Engine/AssetFile/Material/MaterialFileData.h"

void MaterialComponent::LoadAssetResource(){

	if (!mMaterialAsset.IsLoad()) {
		AssetLoad::Instance(mMaterialAsset.m_Hash, mMaterialAsset);
	}
	if (mMaterialAsset.IsLoad()) {
		return;
	}
	AssetLoad::Instance(mShaderAsset.m_Hash, mShaderAsset);
	if (mShaderAsset.IsLoad()) {
		mMaterials[0].CreateShader(mShaderAsset);
	}
}
//void MaterialComponent::SaveAssetResource(){
//
//	//File f;
//	//if (!f.Open(path))
//	//	f.FileCreate();
//	//f.Clear();
//	//if (!f)return;
//	//std::string name = "Material";
//	//f.Out(name);
//	//int num = mMaterials.size();
//	//f.Out(num);
//	//for (int i = 0; i < num; i++){
//	//	Material &mate = mMaterials[i];
//	//	mate.ExportData(f);
//	//}
//}

void MaterialComponent::MaterialAssetCopy()
{
	if (!mMaterialAsset.IsLoad()) {
		AssetLoad::Instance(mMaterialAsset.m_Hash, mMaterialAsset);
	}
	if (!mMaterialAsset.IsLoad()) {
		return;
	}

	auto mate = mMaterialAsset.Get()->GetMaterial();
	if (!mate)return;

	mShaderAsset.Free();
	mShaderAsset.m_Hash = mate->GetShader().m_Hash;
	AssetLoad::Instance(mShaderAsset.m_Hash, mShaderAsset);
	if (mShaderAsset.IsLoad()) {
		mMaterials[0].CreateShader(mShaderAsset);
	}

	mAlbedo = mate->GetAlbedo();
	mSpecular = mate->GetSpecular();
	mTexScale = mate->GetTextureScale();
	mHeightPower = mate->GetHeightPower();
	mThickness = mate->GetAmbient().w;
	mNormalScale = mate->GetNormalScale();
	mOffset = mate->GetOffset();
	mEmissivePowor = mate->GetEmissivePowor();

	mAlbedoTexture.m_Hash =		mate->GetTexture(0).m_Hash;
	mNormalTexture.m_Hash =		mate->GetTexture(1).m_Hash;
	mHeightTexture.m_Hash =		mate->GetTexture(2).m_Hash;
	mSpecularTexture.m_Hash =	mate->GetTexture(3).m_Hash;
	mRoughnessTexture.m_Hash =	mate->GetTexture(4).m_Hash;
	mEmissiveTexture.m_Hash =	mate->GetTexture(5).m_Hash;


	mMaterials[0].SetAlbedo(mAlbedo);
	mMaterials[0].SetSpecular(mSpecular);
	mMaterials[0].SetTextureScale(mTexScale);
	mMaterials[0].SetHeightPower(mHeightPower);
	auto amb = mMaterials[0].GetAmbient();
	amb.w = mThickness;
	mMaterials[0].SetAmbient(amb);
	mMaterials[0].SetNormalScale(mNormalScale);
	mMaterials[0].SetOffset(mOffset);
	mMaterials[0].SetEmissivePowor(mEmissivePowor);

	//mMaterials[0].mCBMaterial.mParam.TexScale = mTexScale;
	//mMaterials[0].mCBMaterial.mParam.HeightPower = mHeightPower;
	//mMaterials[0].mCBMaterial.mParam.Ambient.w = mThickness;
	//mMaterials[0].mCBMaterial.mParam.MNormaleScale = mNormalScale;
	//mMaterials[0].mCBMaterial.mParam.MOffset = mOffset;
	//mMaterials[0].mCBMaterial.mParam.EmissivePowor = mEmissivePowor;

	AssetLoad::Instance(mAlbedoTexture.m_Hash, mAlbedoTexture);
	AssetLoad::Instance(mNormalTexture.m_Hash, mNormalTexture);
	AssetLoad::Instance(mHeightTexture.m_Hash, mHeightTexture);
	AssetLoad::Instance(mSpecularTexture.m_Hash, mSpecularTexture);
	AssetLoad::Instance(mRoughnessTexture.m_Hash, mRoughnessTexture);
	AssetLoad::Instance(mEmissiveTexture.m_Hash, mEmissiveTexture);
	mMaterials[0].SetTexture(mAlbedoTexture, 0);
	mMaterials[0].SetTexture(mNormalTexture, 1);
	mMaterials[0].SetTexture(mHeightTexture, 2);
	mMaterials[0].SetTexture(mSpecularTexture, 3);
	mMaterials[0].SetTexture(mRoughnessTexture, 4);
	mMaterials[0].SetTexture(mEmissiveTexture, 5);
}

MaterialComponent::MaterialComponent(){
	mMaterials.resize(1);
	mAlbedo = XMFLOAT4(1, 1, 1, 1);
	mSpecular = XMFLOAT4(0, 0, 0, 1);
	mTexScale = XMFLOAT2(1, 1);
	mHeightPower = XMFLOAT2(2.0f, 1);
	mNormalScale = XMFLOAT4(1, 1, 1, 1);
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
		//if (mMaterialPath != ""){
			LoadAssetResource();
			MaterialAssetCopy();
			for (auto& m : mMaterials){
				if (!m.IsCreate()){
					if (mShaderAsset.IsLoad()) {
						m.Create(mShaderAsset);
					}
					else {
						m.Create();
					}
				}
			}
		//}
		//else{
		//	auto& m = mMaterials[0];
		//	m.Create();
		//}
	}

	if (mForwardRendering){
		mMaterials[0].CreateShader("EngineResource/ForwardRendering.fx");
	}
	//else{
	//	mMaterials[0].CreateShader("EngineResource/DeferredPrePass.fx");
	//}

	//AssetLoad::Instance(mShaderAsset.m_Hash, mShaderAsset);
	//if (mShaderAsset.IsLoad()){
	//	mMaterials[0].CreateShader(mShaderAsset);
	//}

	mMaterials[0].SetAlbedo(mAlbedo);
	mMaterials[0].SetSpecular ( mSpecular);
	mMaterials[0].SetTextureScale ( mTexScale);
	mMaterials[0].SetHeightPower ( mHeightPower);
	auto amb = mMaterials[0].GetAmbient();
	amb.w = mThickness;
	mMaterials[0].SetAmbient(amb);
	mMaterials[0].SetNormalScale (mNormalScale);
	mMaterials[0].SetOffset ( mOffset);
	mMaterials[0].SetEmissivePowor ( mEmissivePowor);

	AssetLoad::Instance(mAlbedoTexture.m_Hash, mAlbedoTexture);
	AssetLoad::Instance(mNormalTexture.m_Hash, mNormalTexture);
	AssetLoad::Instance(mHeightTexture.m_Hash, mHeightTexture);
	AssetLoad::Instance(mSpecularTexture.m_Hash, mSpecularTexture);
	AssetLoad::Instance(mRoughnessTexture.m_Hash, mRoughnessTexture);
	AssetLoad::Instance(mEmissiveTexture.m_Hash, mEmissiveTexture);
	mMaterials[0].SetTexture(mAlbedoTexture, 0);
	mMaterials[0].SetTexture(mNormalTexture, 1);
	mMaterials[0].SetTexture(mHeightTexture, 2);
	mMaterials[0].SetTexture(mSpecularTexture, 3);
	mMaterials[0].SetTexture(mRoughnessTexture, 4);
	mMaterials[0].SetTexture(mEmissiveTexture, 5);
	
}

void MaterialComponent::SetMaterial(UINT SetNo, Material& material){
	if (mMaterials.size() <= SetNo)mMaterials.resize(SetNo + 1);
	mMaterials[SetNo] = material;

}
Material* MaterialComponent::GetMaterialPtr(UINT GetNo){
	if (mMaterialAsset.IsLoad()) {
		return mMaterialAsset.Get()->GetMaterial();
	}

	if (mMaterials.size() <= GetNo){
		return NULL;
	}
	//return const_cast<Material*>();
	return &(mMaterials.data()[GetNo]);
}
const Material& MaterialComponent::GetMaterial(UINT GetNo) const{
	if (mMaterialAsset.IsLoad()) {
		return *mMaterialAsset.Get()->GetMaterial();
	}
	if (mMaterials.size() <= GetNo){
		static Material temp;
		return temp;
	}
	return mMaterials[GetNo];
}
#ifdef _ENGINE_MODE
void MaterialComponent::CreateInspector(){


	static XMFLOAT4 col(0, 0,0,1);

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
		//mSpecular.w = f.a;
		SetSpecularColor(mSpecular);
	};

	std::function<void(float)> collbackas = [&](float f){
		mSpecular.w = f;
		SetSpecularColor(mSpecular);
	};

	std::function<void(Vector2)> collbackTex = [&](Vector2 f){
		MaterialAssetCopy(); mMaterialAsset.Free();
		mTexScale.x = f.x;
		mTexScale.y = f.y;
		mMaterials[0].SetTextureScale(mTexScale);
	};

	std::function<void(Vector2)> collbackOffset = [&](Vector2 f){
		MaterialAssetCopy(); mMaterialAsset.Free();
		mOffset.x = f.x;
		mOffset.y = f.y;
		mMaterials[0].SetOffset(mOffset);
	};

	std::function<void(float)> collbackH = [&](float f){
		MaterialAssetCopy(); mMaterialAsset.Free();
		mHeightPower.x = f;
		mMaterials[0].SetHeightPower(mHeightPower);
	};
	std::function<void(float)> collbackHDR = [&](float f){
		MaterialAssetCopy(); mMaterialAsset.Free();
		mHeightPower.y = f;
		mMaterials[0].SetHeightPower(mHeightPower);
	};
	std::function<void(float)> collbackThick = [&](float f){
		MaterialAssetCopy(); mMaterialAsset.Free();
		mThickness = f;

		auto amb = mMaterials[0].GetAmbient();
		amb.w = mThickness;
		mMaterials[0].SetAmbient(amb);
	};

	std::function<void(Vector3)> collbackNormalScale = [&](Vector3 f){
		MaterialAssetCopy(); mMaterialAsset.Free();

		mNormalScale.x = f.x;
		mNormalScale.y = f.y;
		mNormalScale.z = f.z;
		mMaterials[0].SetNormalScale(mNormalScale);
	};

	Inspector ins("Material",this);
	
	ins.Add("Material", &mMaterialAsset, [&]() {LoadAssetResource();
	MaterialAssetCopy(); });
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

	static Color alb(col);
	alb = Color(mAlbedo);
	ins.Add("Albedo", &alb, collbackAlb);

	static Color spec(col);
	spec = Color(mSpecular);
	ins.Add("Specular", &spec, collbackSpec);
	ins.AddSlideBar("Roughness", 0, 1, &mSpecular.w, collbackas);
	ins.Add("AlbedoTextre", &mAlbedoTexture, [&]() {
		MaterialAssetCopy(); mMaterialAsset.Free(); 
		mMaterials[0].SetTexture(mAlbedoTexture, 0);
 });
	ins.Add("NormalTextre", &mNormalTexture, [&]() {
		MaterialAssetCopy(); mMaterialAsset.Free(); 
		mMaterials[0].SetTexture(mNormalTexture, 1);
});
	ins.Add("HeightTextre", &mHeightTexture, [&]() {
		MaterialAssetCopy(); mMaterialAsset.Free(); 
		mMaterials[0].SetTexture(mHeightTexture, 2);
 });
	ins.Add("SpecularTextre", &mSpecularTexture, [&]() {
		MaterialAssetCopy(); mMaterialAsset.Free();
		mMaterials[0].SetTexture(mSpecularTexture, 3);
	});
	ins.Add("RoughnessTextre", &mRoughnessTexture, [&]() {
		MaterialAssetCopy(); mMaterialAsset.Free();
		mMaterials[0].SetTexture(mRoughnessTexture, 4);
 });
	ins.Add("EmissiveTextre", &mEmissiveTexture, [&]() {
		MaterialAssetCopy(); mMaterialAsset.Free();
		mMaterials[0].SetTexture(mEmissiveTexture, 5);
});
	ins.Add("EmissivePowor", &mEmissivePowor, [&](float f){
		MaterialAssetCopy(); mMaterialAsset.Free();
		mEmissivePowor = f;
		mMaterials[0].SetEmissivePowor(mEmissivePowor);
	});

	static XMFLOAT2 v2(0,0);
	static Vector2 tex(v2);
	tex = Vector2(mTexScale);
	ins.Add("TextureScale", &tex, collbackTex);

	static XMFLOAT4 v3(0, 0,0,1);
	static Vector3 texNormal(v3);
	texNormal = Vector3(mNormalScale);
	ins.Add("NormaleScale", &texNormal, collbackNormalScale);

	static Vector2 off(v2);
	off = Vector2(mOffset);
	ins.Add("Offset", &off, collbackOffset);

	ins.AddSlideBar("HightPower", -10, 10, &mHeightPower.x, collbackH);
	ins.Add("HDR", &mHeightPower.y, collbackHDR);
	ins.Add("Thickness", &mThickness, collbackThick);
	ins.Add("Shader", &mShaderAsset, [&]() {
		MaterialAssetCopy(); mMaterialAsset.Free();
		if (mShaderAsset.IsLoad()) {
			mMaterials[0].CreateShader(mShaderAsset);
		}
		else {
			mMaterials[0].CreateShader();
		}
	});
	ins.Complete();

}
#endif

void MaterialComponent::SetAlbedoColor(const XMFLOAT4& col){
	MaterialAssetCopy(); mMaterialAsset.Free();
	mAlbedo = col;
	mMaterials[0].SetAlbedo(mAlbedo);
}
void MaterialComponent::SetSpecularColor(const XMFLOAT4& col){
	MaterialAssetCopy(); mMaterialAsset.Free();
	mSpecular = col;
	mMaterials[0].SetSpecular(mSpecular);
}

void MaterialComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mMaterialAsset);
	_KEY(mShaderAsset);
	_KEY(mForwardRendering);
	_KEY(mAlbedo.x);
	_KEY(mAlbedo.y);
	_KEY(mAlbedo.z);
	_KEY(mAlbedo.w);
	_KEY(mSpecular.x);
	_KEY(mSpecular.y);
	_KEY(mSpecular.z);
	_KEY(mSpecular.w);
	_KEY(mAlbedoTexture);
	_KEY(mNormalTexture);
	_KEY(mHeightTexture);
	_KEY(mSpecularTexture);
	_KEY(mRoughnessTexture);
	_KEY(mEmissiveTexture);
	_KEY(mTexScale.x);
	_KEY(mTexScale.y);
	_KEY(mEmissivePowor);
	_KEY(mOffset.x);
	_KEY(mOffset.y);
	_KEY(mHeightPower.x);
	_KEY(mHeightPower.y);
	_KEY(mNormalScale.x);
	_KEY(mNormalScale.y);
	_KEY(mNormalScale.z);
	_KEY(mNormalScale.w);
	_KEY(mThickness);
#undef _KEY
}