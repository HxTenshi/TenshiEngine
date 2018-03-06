

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
	mFileName = "";
	mHash.clear();
	//HRESULT hr = S_OK;
	// Create the sample state
	//D3D11_SAMPLER_DESC sampDesc;
	//ZeroMemory(&sampDesc, sizeof(sampDesc));
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	////sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	//sampDesc.MipLODBias = 0;
	//sampDesc.MinLOD = 0;
	//sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//ID3D11SamplerState* smpler;
	//hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &smpler);
	//if (FAILED(hr)){
	//	_SYSTEM_LOG_ERROR("SamplerState�̍쐬");
	//	return hr;
	//}

	if (mTextureAssetData){
		mTextureAssetData->_GetFileData()->SetTexture(pTexture/*, smpler*/);
	}
	else{

		auto file = new TextureFileData();
		file->SetTexture(pTexture/*, smpler*/);

		mTextureAssetData = AssetDataTemplate<TextureFileData>::Create(file);
	}
	return S_OK;
}

HRESULT Texture::Create(ID3D11Texture2D* pTexture){
	mFileName = "";
	mHash.clear();
	ID3D11ShaderResourceView* pShaderResourceView;
	auto hr = Device::mpd3dDevice->CreateShaderResourceView(pTexture, nullptr, &pShaderResourceView);
	if (FAILED(hr)){
		_SYSTEM_LOG_ERROR("ShaderResourceView�̍쐬");
		return hr;
	}

	return Create(pShaderResourceView);
}

HRESULT Texture::Create(const char* FileName){
	mFileName = FileName;
	if (!AssetDataBase::FilePath2Hash(FileName, mHash)){
		mHash.clear();
		return E_FAIL;
	}
	TextureAssetDataPtr tex(NULL);
	AssetDataBase::Instance(mHash, tex);
	mTextureAssetData = tex;
	return ((bool)mTextureAssetData)?S_OK:E_FAIL;
}

HRESULT Texture::Create(const MD5::MD5HashCode& hash){
	mFileName = "";// FileName;
	mHash = hash;
	TextureAssetDataPtr tex(NULL);
	AssetDataBase::Instance(hash, tex);
	mTextureAssetData = tex;
	return ((bool)mTextureAssetData) ? S_OK : E_FAIL;
}
HRESULT Texture::Create(const TextureAsset& tex){
	mFileName = "";// FileName;
	mTextureAssetData = NULL;
	mTextureAssetData = tex.m_Ptr;
	mHash = tex.m_Hash;
	return ((bool)mTextureAssetData) ? S_OK : E_FAIL;
}

void Texture::PSSetShaderResources(ID3D11DeviceContext* context,UINT Slot) const{
	if (!mTextureAssetData ||
		!mTextureAssetData->GetFileData()->GetTexture()->mpTextureRV){
		return;
	}
	auto tex = mTextureAssetData->GetFileData()->GetTexture();
	context->PSSetShaderResources(Slot, 1, &tex->mpTextureRV);
	//context->PSSetSamplers(Slot, 1, &tex->mpSamplerLinear);
}
void Texture::GSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const{
	if (!mTextureAssetData ||
		!mTextureAssetData->GetFileData()->GetTexture()->mpTextureRV){
		return;
	}
	auto tex = mTextureAssetData->GetFileData()->GetTexture();
	context->GSSetShaderResources(Slot, 1, &tex->mpTextureRV);
	//context->GSSetSamplers(Slot, 1, &tex->mpSamplerLinear);
}
void Texture::CSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const{
	if (!mTextureAssetData ||
		!mTextureAssetData->GetFileData()->GetTexture()->mpTextureRV){
		return;
	}
	auto tex = mTextureAssetData->GetFileData()->GetTexture();
	context->CSSetShaderResources(Slot, 1, &tex->mpTextureRV);
	//context->CSSetSamplers(Slot, 1, &tex->mpSamplerLinear);
}


shared_ptr<Release_self<ID3D11Resource>> Texture::GetResource(){
	ID3D11Resource* res;
	mTextureAssetData->GetFileData()->GetTexture()->mpTextureRV->GetResource(&res);
	shared_ptr<Release_self<ID3D11Resource>> temp = make_shared<Release_self<ID3D11Resource>>();
	(*temp) = res;
	return temp;
}

TextureAsset Texture::GetAsset()
{
	TextureAsset tex;
	tex.m_Ptr = mTextureAssetData;
	tex.m_Hash = mHash;
	return tex;
}
