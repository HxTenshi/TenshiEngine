

#include "Texture.h"
#include "Device/DirectX11Device.h"

#include <tchar.h>
#include <locale.h>

#include "Engine/AssetFile/Material/TextureFileData.h"

//Texture Texture::mNullTexture;

Texture::Texture()
{
}
Texture::~Texture()
{
}

//static Texture Create(){
//	Texture* t = new Texture();
//	auto r = AutoReleaseComponent<Texture>(t);
//	Texture a(*(Texture*)&r);
//	return a;
//}


HRESULT Texture::Create(ID3D11ShaderResourceView* pTexture){

	HRESULT hr = S_OK;
	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MipLODBias = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* smpler;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &smpler);
	if (FAILED(hr))
		return hr;



	auto file = new TextureFileData();
	file->SetTexture(pTexture, smpler);

	mTextureAssetData = AssetDataTemplate<TextureFileData>::Create(file);

	return S_OK;
}

HRESULT Texture::Create(const char* FileName){
	mFileName = FileName;
	TextureAssetDataPtr tex(NULL);
	AssetDataBase::Instance(FileName, tex);
	mTextureAssetData = tex;
	return ((bool)mTextureAssetData)?S_OK:E_FAIL;
}

void Texture::PSSetShaderResources(ID3D11DeviceContext* context,UINT Slot) const{
	if (!mTextureAssetData ||
		!mTextureAssetData->GetFileData()->GetTexture()->mpTextureRV){
		return;
	}
	auto tex = mTextureAssetData->GetFileData()->GetTexture();
	context->PSSetShaderResources(Slot, 1, &tex->mpTextureRV);
	context->PSSetSamplers(Slot, 1, &tex->mpSamplerLinear);
}
void Texture::GSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const{
	if (!mTextureAssetData ||
		!mTextureAssetData->GetFileData()->GetTexture()->mpTextureRV){
		return;
	}
	auto tex = mTextureAssetData->GetFileData()->GetTexture();
	context->GSSetShaderResources(Slot, 1, &tex->mpTextureRV);
	context->GSSetSamplers(Slot, 1, &tex->mpSamplerLinear);
}
void Texture::CSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const{
	if (!mTextureAssetData ||
		!mTextureAssetData->GetFileData()->GetTexture()->mpTextureRV){
		return;
	}
	auto tex = mTextureAssetData->GetFileData()->GetTexture();
	context->CSSetShaderResources(Slot, 1, &tex->mpTextureRV);
	context->CSSetSamplers(Slot, 1, &tex->mpSamplerLinear);
}


#include "MySTL/File.h"
void Texture::ExportData(File& f){
	if (mFileName != ""){
		auto name = mFileName;
		auto ioc = name.find(" ");
		while (std::string::npos != ioc){
			name.replace(ioc, 1, "$");
			ioc = name.find(" ");
		}
		f.Out(name);
	}
}
