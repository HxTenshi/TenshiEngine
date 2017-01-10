
#include "Material.h"

Material::Material()
{
	mDiffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpecular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mAmbient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	mTexScale = XMFLOAT2(1.0f, 1.0f);
	mHeightPower = XMFLOAT2(2.0f, 1.0f);
	mNormalScale = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mOffset = XMFLOAT2(0.0f,0.0f);
	mEmissivePowor = 1.0f;

	mCBUseTexture.mParam.UseTexture = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	mCBUseTexture.mParam.UseTexture2 = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}
Material::~Material()
{

}

HRESULT Material::Create(const char* shaderFileName){

	mCBMaterial = ConstantBuffer<cbChangesMaterial>::create(4);
	if (!mCBMaterial.mBuffer)
		return S_FALSE;


	mCBMaterial.mParam.Diffuse = mDiffuse;
	mCBMaterial.mParam.Specular = mSpecular;
	mCBMaterial.mParam.Ambient = mAmbient;
	mCBMaterial.mParam.TexScale = mTexScale;
	mCBMaterial.mParam.HeightPower = mHeightPower;
	mCBMaterial.mParam.MNormaleScale = mNormalScale;
	mCBMaterial.mParam.MOffset = mOffset;
	mCBMaterial.mParam.EmissivePowor = mEmissivePowor;
	mCBMaterial.mParam.MNULL = 0.0f;

	mShader.Create(shaderFileName);

	auto tex1 = mCBUseTexture.mParam.UseTexture;
	auto tex2 = mCBUseTexture.mParam.UseTexture2;
	mCBUseTexture = ConstantBuffer<cbChangesUseTexture>::create(6);
	mCBUseTexture.mParam.UseTexture  = tex1;
	mCBUseTexture.mParam.UseTexture2 = tex2;

	return S_OK;

}
HRESULT Material::Create(ShaderAsset& asset){

	mCBMaterial = ConstantBuffer<cbChangesMaterial>::create(4);
	if (!mCBMaterial.mBuffer)
		return S_FALSE;


	mCBMaterial.mParam.Diffuse = mDiffuse;
	mCBMaterial.mParam.Specular = mSpecular;
	mCBMaterial.mParam.Ambient = mAmbient;
	mCBMaterial.mParam.TexScale = mTexScale;
	mCBMaterial.mParam.HeightPower = mHeightPower;
	mCBMaterial.mParam.MNormaleScale = mNormalScale;
	mCBMaterial.mParam.MOffset = mOffset;
	mCBMaterial.mParam.EmissivePowor = mEmissivePowor;
	mCBMaterial.mParam.MNULL = 0.0f;

	mShader.Create(asset);

	auto tex1 = mCBUseTexture.mParam.UseTexture;
	auto tex2 = mCBUseTexture.mParam.UseTexture2;
	mCBUseTexture = ConstantBuffer<cbChangesUseTexture>::create(6);
	mCBUseTexture.mParam.UseTexture = tex1;
	mCBUseTexture.mParam.UseTexture2 = tex2;

	return S_OK;

}

void Material::CreateShader(const char* shaderFileName){
	mShader.Create(shaderFileName);
}
void Material::CreateShader(ShaderAsset& asset) {
	mShader.Create(asset);
}

//HRESULT Material::Create(const ConstantBuffer<cbChangesMaterial>& cbMaterial,const ConstantBuffer<cbChangesUseTexture>& cbUseTexture){
//	HRESULT hr = S_OK;
//	mCBMaterial = cbMaterial.Duplicate();
//	if (!mCBMaterial.mBuffer)
//		return S_FALSE;
//
//
//	mCBMaterial.mParam.Diffuse = mDiffuse;
//	mCBMaterial.mParam.Specular = mSpecular;
//	mCBMaterial.mParam.Ambient = mAmbient;
//
//	//mCBMaterial->UpdateSubresource();
//
//
//	mCBUseTexture = cbUseTexture.Duplicate();
//	if (!mCBUseTexture.mBuffer)
//		return S_FALSE;
//	mCBUseTexture.mParam.UseTexture = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
//
//	//mCBUseTexture->UpdateSubresource();
//	mShader.Create("Tutorial07.fx");
//
//	return S_OK;
//}

void Material::SetShader(bool UseAnime, ID3D11DeviceContext* context) const{
	mShader.SetShader(UseAnime,context);
}
void Material::SetShader_PS(ID3D11DeviceContext* context) const{
	mShader.SetShader_PS(context);
}
void Material::SetShader_VS(bool UseAnime, ID3D11DeviceContext* context) const{
	mShader.SetShader_VS(UseAnime, context);
}
void Material::VSSetShaderResources(ID3D11DeviceContext* context, ForcedMaterialFilter::Enum filter) const{
	if (!mCBMaterial.mBuffer || !mCBUseTexture.mBuffer){
		return;
	}

	if (filter & ForcedMaterialFilter::Color){
		mCBMaterial.UpdateSubresource(context);
		mCBMaterial.VSSetConstantBuffers(context);
	}

	if (filter & ForcedMaterialFilter::Texture_ALL){
		mCBUseTexture.UpdateSubresource(context);
		mCBUseTexture.VSSetConstantBuffers(context);
	}
}
void Material::PSSetShaderResources(ID3D11DeviceContext* context, ForcedMaterialFilter::Enum filter) const{
	if (!mCBMaterial.mBuffer || !mCBUseTexture.mBuffer){
		return;
	}

	for (UINT i = 0; i < 8; i++){

		if (filter & (ForcedMaterialFilter::Texture_0 << i)){
			mTexture[i].PSSetShaderResources(context, i);
		}
	}

	if (filter & ForcedMaterialFilter::Color){
		mCBMaterial.UpdateSubresource(context);
		mCBMaterial.PSSetConstantBuffers(context);
	}

	if (filter & ForcedMaterialFilter::Texture_ALL){
		mCBUseTexture.UpdateSubresource(context);
		mCBUseTexture.PSSetConstantBuffers(context);
	}
}
void Material::GSSetShaderResources(ID3D11DeviceContext* context, ForcedMaterialFilter::Enum filter) const{
	if (!mCBMaterial.mBuffer || !mCBUseTexture.mBuffer){
		return;
	}


	for (UINT i = 0; i < 8; i++){

		if (filter & (ForcedMaterialFilter::Texture_0 << i)){
			mTexture[i].GSSetShaderResources(context, i);
		}
	}

	if (filter & ForcedMaterialFilter::Color){
		mCBMaterial.UpdateSubresource(context);
		mCBMaterial.GSSetConstantBuffers(context);
	}

	if (filter & ForcedMaterialFilter::Texture_ALL){
		mCBUseTexture.UpdateSubresource(context);
		mCBUseTexture.GSSetConstantBuffers(context);
	}
}


void Material::SetTexture(const char* FileName, UINT Slot){
	HRESULT hr = mTexture[Slot].Create(FileName);
	mTexture[Slot].mFileName = "";
	float flag = 1.0f;
	if (FAILED(hr))
		flag = 0.0f;

	if (Slot == 0)
		mCBUseTexture.mParam.UseTexture.x = flag;
	if (Slot == 1)
		mCBUseTexture.mParam.UseTexture.y = flag;
	if (Slot == 2)
		mCBUseTexture.mParam.UseTexture.z = flag;
	if (Slot == 3)
		mCBUseTexture.mParam.UseTexture.w = flag;

	if (Slot == 4)
		mCBUseTexture.mParam.UseTexture2.x = flag;
	if (Slot == 5)
		mCBUseTexture.mParam.UseTexture2.y = flag;
	if (Slot == 6)
		mCBUseTexture.mParam.UseTexture2.z = flag;
	if (Slot == 7)
		mCBUseTexture.mParam.UseTexture2.w = flag;
	//mCBUseTexture->UpdateSubresource();
}
void Material::SetTexture(const MD5::MD5HashCode& hash, UINT Slot) {
	HRESULT hr = mTexture[Slot].Create(hash);
	mTexture[Slot].mFileName = "ref";
	float flag = 1.0f;
	if (FAILED(hr))
		flag = 0.0f;
	if (Slot == 0)
		mCBUseTexture.mParam.UseTexture.x = flag;
	if (Slot == 1)
		mCBUseTexture.mParam.UseTexture.y = flag;
	if (Slot == 2)
		mCBUseTexture.mParam.UseTexture.z = flag;
	if (Slot == 3)
		mCBUseTexture.mParam.UseTexture.w = flag;

	if (Slot == 4)
		mCBUseTexture.mParam.UseTexture2.x = flag;
	if (Slot == 5)
		mCBUseTexture.mParam.UseTexture2.y = flag;
	if (Slot == 6)
		mCBUseTexture.mParam.UseTexture2.z = flag;
	if (Slot == 7)
		mCBUseTexture.mParam.UseTexture2.w = flag;
	//mCBUseTexture->UpdateSubresource();
}
void Material::SetTexture(const Texture& Tex, UINT Slot){
	mTexture[Slot] = Tex;
	mTexture[Slot].mFileName = "ref";
	if (Slot == 0)
		mCBUseTexture.mParam.UseTexture.x = 1.0f;
	if (Slot == 1)
		mCBUseTexture.mParam.UseTexture.y = 1.0f;
	if (Slot == 2)
		mCBUseTexture.mParam.UseTexture.z = 1.0f;
	if (Slot == 3)
		mCBUseTexture.mParam.UseTexture.w = 1.0f;

	if (Slot == 4)
		mCBUseTexture.mParam.UseTexture2.x = 1.0f;
	if (Slot == 5)
		mCBUseTexture.mParam.UseTexture2.y = 1.0f;
	if (Slot == 6)
		mCBUseTexture.mParam.UseTexture2.z = 1.0f;
	if (Slot == 7)
		mCBUseTexture.mParam.UseTexture2.w = 1.0f;
	//mCBUseTexture->UpdateSubresource();
}
void Material::SetTexture(const TextureAsset& Tex, UINT Slot){
	HRESULT hr = mTexture[Slot].Create(Tex);

	float flag = 1.0f;
	if (FAILED(hr)) {
		flag = 0.0f;
		mTexture[Slot].mFileName = "";
	}
	else {
		mTexture[Slot].mFileName = "ref";
	}

	if (Slot == 0)
		mCBUseTexture.mParam.UseTexture.x = flag;
	if (Slot == 1)
		mCBUseTexture.mParam.UseTexture.y = flag;
	if (Slot == 2)
		mCBUseTexture.mParam.UseTexture.z = flag;
	if (Slot == 3)
		mCBUseTexture.mParam.UseTexture.w = flag;

	if (Slot == 4)
		mCBUseTexture.mParam.UseTexture2.x = flag;
	if (Slot == 5)
		mCBUseTexture.mParam.UseTexture2.y = flag;
	if (Slot == 6)
		mCBUseTexture.mParam.UseTexture2.z = flag;
	if (Slot == 7)
		mCBUseTexture.mParam.UseTexture2.w = flag;
	//mCBUseTexture->UpdateSubresource();
}

#include "MySTL/ioHelper.h"
void Material::IO_Data(I_ioHelper* io, const std::string& materialPath) {
#define _KEY(x) io->func( x , #x)

	io->func(mDiffuse, "Albedo");
	io->func(mSpecular, "Specular");
	io->func(mAmbient, "Ambient");
	io->func(mTexScale, "TexScale");
	io->func(mHeightPower, "HeightPower");
	io->func(mNormalScale, "NormalScale");
	io->func(mOffset, "Offset");
	io->func(mSpecular.w, "Roughness");
	io->func(mAmbient.w, "Thickness");
	io->func(mEmissivePowor, "EmissivePowor");
	io->func(mSpecular.w, "Roughness");

	if (io->isInput()) {

		mCBMaterial.mParam.Diffuse = mDiffuse;
		mCBMaterial.mParam.Specular = mSpecular;
		mCBMaterial.mParam.Ambient = mAmbient;
		mCBMaterial.mParam.TexScale = mTexScale;
		mCBMaterial.mParam.HeightPower = mHeightPower;
		mCBMaterial.mParam.MNormaleScale = mNormalScale;
		mCBMaterial.mParam.MOffset = mOffset;
		mCBMaterial.mParam.EmissivePowor = mEmissivePowor;
		mCBMaterial.mParam.MNULL = 0.0f;

		std::string name;
		io->func(name, "AlbedoTexture");
		SetTexture((materialPath + name).c_str(), 0);
		name = "";
		io->func(name, "NormalTexture");
		SetTexture((materialPath + name).c_str(), 1);
		name = "";
		io->func(name, "HeightTexture");
		SetTexture((materialPath + name).c_str(), 2);
		name = "";
		io->func(name, "SpecularTexture");
		SetTexture((materialPath + name).c_str(), 3);
		name = "";
		io->func(name, "RoughnessTexture");
		SetTexture((materialPath + name).c_str(), 4);
		name = "";
		io->func(name, "EmissiveTexture");
		SetTexture((materialPath + name).c_str(), 5);
		name = "";
		io->func(name, "Tex6");
		SetTexture((materialPath + name).c_str(), 6);
		name = "";
		io->func(name, "Tex7");
		SetTexture((materialPath + name).c_str(), 7);


		MD5::MD5HashCode hash;

		io->func(hash, "Shader");
		mShader.Create(hash);
		TextureAsset tex;
		io->func(hash, "AlbedoTextureHash");
		if(!hash.IsNull())SetTexture(hash, 0);
		hash.clear();
		io->func(hash, "NormalTextureHash");
		if (!hash.IsNull())SetTexture(hash, 1);
		hash.clear();
		io->func(hash, "HeightTextureHash");
		if (!hash.IsNull())SetTexture(hash, 2);
		hash.clear();
		io->func(hash, "SpecularTextureHash");
		if (!hash.IsNull())SetTexture(hash, 3);
		hash.clear();
		io->func(hash, "RoughnessTextureHash");
		if (!hash.IsNull())SetTexture(hash, 4);
		hash.clear();
		io->func(hash, "EmissiveTextureHash");
		if (!hash.IsNull())SetTexture(hash, 5);
		hash.clear();
		io->func(hash, "Tex6Hash");
		if (!hash.IsNull())SetTexture(hash, 6);
		hash.clear();
		io->func(hash, "Tex7Hash");
		if (!hash.IsNull())SetTexture(hash, 7);
	}
	else {
		io->func(mShader.GetHash(), "Shader");

		io->func(mTexture[0].GetHash(), "AlbedoTextureHash");
		io->func(mTexture[1].GetHash(), "NormalTextureHash");
		io->func(mTexture[2].GetHash(), "HeightTextureHash");
		io->func(mTexture[3].GetHash(), "SpecularTextureHash");
		io->func(mTexture[4].GetHash(), "RoughnessTextureHash");
		io->func(mTexture[5].GetHash(), "EmissiveTextureHash");
		io->func(mTexture[6].GetHash(), "Tex6Hash");
		io->func(mTexture[7].GetHash(), "Tex7Hash");
	}

	//_KEY(mForwardRendering);
#undef _KEY
}