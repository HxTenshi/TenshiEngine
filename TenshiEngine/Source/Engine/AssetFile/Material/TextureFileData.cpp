#include "TextureFileData.h"
#include "Device/DirectX11Device.h"

#include <D3D11.h>
void TextureData::Release(){
	if (mpTextureRV){
		mpTextureRV->Release();
		mpTextureRV = NULL;
	}
	if (mpSamplerLinear){
		mpSamplerLinear->Release();
		mpSamplerLinear = NULL;
	}
}

TextureFileData::TextureFileData(){
}
TextureFileData::~TextureFileData(){
	m_FileName = "";
}

void TextureFileData::Create(const char* filename){

	m_FileName = filename;

	m_TextureData.Release();

	m_TextureData.mpTextureRV = LoadDirectXTex();

	if (!m_TextureData.mpTextureRV){
		return;
	}

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


	Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_TextureData.mpSamplerLinear);

}

void TextureFileData::FileUpdate(){
	Create(m_FileName.c_str());
}


#include "../DirectXTex/DirectXTex.h"
#ifndef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#else
#pragma comment(lib, "DirectXTex/DirectXTexD.lib")
#endif //_DEBUG

#include "Device/DirectX11Device.h"

ID3D11ShaderResourceView* TextureFileData::LoadDirectXTex(){
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;

	WCHAR f[256];
	size_t wLen = 0;
	//ロケール指定
	setlocale(LC_ALL, "japanese");
	//変換
	mbstowcs_s(&wLen, f, 255, m_FileName.c_str(), _TRUNCATE);

	HRESULT hr;
	do{
		hr = DirectX::LoadFromWICFile(f, 0, &metadata, image);
		if (SUCCEEDED(hr)){
			break;
		}
		hr = DirectX::LoadFromTGAFile(f, &metadata, image);
		if (SUCCEEDED(hr)){
			break;
		}
		hr = DirectX::LoadFromDDSFile(f, 0, &metadata, image);
		if (SUCCEEDED(hr)){
			break;
		}
	} while (false);
	if (FAILED(hr)){
		return NULL;
	}

	ID3D11ShaderResourceView* temp = NULL;

	// 画像からシェーダリソースView DirectXTex
	hr = CreateShaderResourceView(Device::mpd3dDevice, image.GetImages(), image.GetImageCount(), metadata, &temp);
	if (FAILED(hr)){
		return NULL;
	}
	image.Release();

	return temp;
}