
#include "Material.h"

Material::Material()
{
	mDiffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpecular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mAmbient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mTexScale = XMFLOAT2(1.0f, 1.0f);
	mHeightPower = XMFLOAT2(2.0f, 1.0f);
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


	mCBUseTexture = ConstantBuffer<cbChangesUseTexture>::create(6);
	if (!mCBUseTexture.mBuffer)
		return S_FALSE;
	mCBUseTexture.mParam.UseTexture = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

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

void Material::SetShader(bool UseAnime) const{
	mShader.SetShader(UseAnime);
}
void Material::VSSetShaderResources() const{
	if (!mCBMaterial.mBuffer || !mCBUseTexture.mBuffer){
		return;
	}

	mCBMaterial.UpdateSubresource();
	mCBUseTexture.UpdateSubresource();

	mCBMaterial.VSSetConstantBuffers();
	mCBUseTexture.VSSetConstantBuffers();
}
void Material::PSSetShaderResources() const{
	if (!mCBMaterial.mBuffer || !mCBUseTexture.mBuffer){
		return;
	}

	for (UINT i = 0; i < 8; i++){
		mTexture[i].PSSetShaderResources(i);
	}

	mCBMaterial.UpdateSubresource();
	mCBUseTexture.UpdateSubresource();

	mCBMaterial.PSSetConstantBuffers();
	mCBUseTexture.PSSetConstantBuffers();
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