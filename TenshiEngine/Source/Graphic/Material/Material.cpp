
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
	mCBMaterial.mParam.EmissivePowor = 1.0f;
	mCBMaterial.mParam.MNULL = 0.0f;

	mCBUseTexture = ConstantBuffer<cbChangesUseTexture>::create(6);
	if (!mCBUseTexture.mBuffer)
		return S_FALSE;
	mCBUseTexture.mParam.UseTexture = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	mCBUseTexture.mParam.UseTexture2 = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	mShader.Create(shaderFileName);

	return S_OK;

}

void Material::CreateShader(const char* shaderFileName){
	mShader.Create(shaderFileName);
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

#include "MySTL/File.h"
void  Material::ExportData(File& f){
	auto& use = mCBUseTexture.mParam.UseTexture;
	int num = (int)(use.x + use.y + use.z + use.w);
	f.Out(num);
	if (use.x == 1.0f){
		int slot = 0;
		f.Out(slot);
		mTexture[slot].ExportData(f);
	}
	if (use.y == 1.0f){
		int slot = 1;
		f.Out(slot);
		mTexture[slot].ExportData(f);
	}
	if (use.z == 1.0f){
		int slot = 2;
		f.Out(slot);
		mTexture[slot].ExportData(f);
	}
	if (use.w == 1.0f){
		int slot = 3;
		f.Out(slot);
		mTexture[slot].ExportData(f);
	}
}
