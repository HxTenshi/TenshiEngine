

#include "Texture.h"

#if _TEXTURE_LOADTYPE_DIRECTXTEX_

#include "../DirectXTex/DirectXTex.h"
#ifndef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#else
#pragma comment(lib, "DirectXTex/DirectXTexD.lib")
#endif //_DEBUG

#endif //_TEXTURE_LOADTYPE_DIRECTXTEX_


#include "Device/DirectX11Device.h"

#include <tchar.h>
#include <locale.h>


#include "../DirectXTex/DirectXTex.h"

//Texture Texture::mNullTexture;

Texture::Texture()
{
	mTexturePtr = make_shared<TextureData>();
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
	mTexturePtr->Release();
	mTexturePtr->mpTextureRV = pTexture;

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
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &mTexturePtr->mpSamplerLinear);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Texture::Create(const char* FileName){
	mTexturePtr->Release();
	mFileName = FileName;
	HRESULT hr = S_OK;

	// Load the Texture
	//hr = D3DX11CreateShaderResourceViewFromFile(Device::mpd3dDevice, L"seafloor.dds", NULL, NULL, &mpTextureRV, NULL);
	//if (FAILED(hr))
	//	return hr;

	hr = LoadTexture();
	if (FAILED(hr))
		return hr;

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

	
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &mTexturePtr->mpSamplerLinear);
	if (FAILED(hr))
		return hr;
	return S_OK;
}

void Texture::PSSetShaderResources(UINT Slot) const{
	if (!mTexturePtr->mpTextureRV){
		//mNullTexture.PSSetShaderResources(Slot);
		//ID3D11ShaderResourceView* r = NULL;
		//ID3D11SamplerState* s = NULL;
		//Device::mpImmediateContext->PSSetShaderResources(Slot, 1, &r);
		//Device::mpImmediateContext->PSSetSamplers(Slot, 1, &s);
		return;
	}
	Device::mpImmediateContext->PSSetShaderResources(Slot, 1, &mTexturePtr->mpTextureRV);
	Device::mpImmediateContext->PSSetSamplers(Slot, 1, &mTexturePtr->mpSamplerLinear);
}

#if _TEXTURE_LOADTYPE_DIRECTXTEX_
HRESULT Texture::LoadDirectXTex(){
	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;

	WCHAR f[256];
	size_t wLen = 0;
	//ロケール指定
	setlocale(LC_ALL, "japanese");
	//変換
	mbstowcs_s(&wLen, f, 255, mFileName.c_str(), _TRUNCATE);

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
		return hr;
	}

	// 画像からシェーダリソースView DirectXTex
	hr = CreateShaderResourceView(Device::mpd3dDevice, image.GetImages(), image.GetImageCount(), metadata, &mTexturePtr->mpTextureRV);
	if (FAILED(hr)){
		return hr;
	}
	image.Release();

	return hr;
}

#else
HRESULT Texture::LoadD3DX11(){
	D3DX11_IMAGE_LOAD_INFO info;
	// ファイルからシェーダーリソースビューを作成する
	::ZeroMemory(&info, sizeof(D3DX11_IMAGE_LOAD_INFO));
	info.Width = D3DX11_DEFAULT;
	info.Height = D3DX11_DEFAULT;
	info.Depth = D3DX11_DEFAULT;
	info.FirstMipLevel = D3DX11_DEFAULT;          // テクスチャーの最高解像度のミップマップ レベル。実際の使用方法不明
	info.MipLevels = 0;                           // ミップマップ数。0 または D3DX11_DEFAULT を使用するとすべてのミップマップ チェーンを作成する
	info.Usage = D3D11_USAGE_DEFAULT;
	info.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	info.CpuAccessFlags = 0;
	info.MiscFlags = 0;
	info.Format = DXGI_FORMAT_FROM_FILE;
	info.Filter = D3DX11_FILTER_POINT;            // テクスチャー読み込み時に使用するフィルター
	info.MipFilter = D3DX11_FILTER_POINT;         // ミップマップ作成時に使用するフィルター
	info.pSrcInfo = NULL;

	//WCHAR wstr[256];
	//size_t wLen = 0;
	//mbstowcs_s(&wLen, wstr, strlen(FileName) + 1, FileName, _TRUNCATE);
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(Device::mpd3dDevice, mFileName.c_str(), &info, NULL, &mTexturePtr->mpTextureRV, NULL);

	return hr;
}
#endif

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
