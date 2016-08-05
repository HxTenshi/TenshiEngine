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

bool TextureFileData::Create(const char* filename){

	m_FileName = filename;

	m_TextureData.Release();

	m_TextureData.mpTextureRV = LoadDirectXTex();
	if (!m_TextureData.mpTextureRV){
		m_TextureData.mpTextureRV = LoadRGBE();
	}

	if (!m_TextureData.mpTextureRV){
		return false;
	}

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	sampDesc.MipLODBias = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_TextureData.mpSamplerLinear);
	if (FAILED(hr)){
		return false;
	}
	return true;

}

bool TextureFileData::FileUpdate(){
	return Create(m_FileName.c_str());
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
		image.Release();
		return NULL;
	}
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), metadata, DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)){
		mipChain.Release();
		image.Release();
		return NULL;
	}	
	ID3D11ShaderResourceView* temp = NULL;

	// 画像からシェーダリソースView DirectXTex
	hr = DirectX::CreateShaderResourceView(Device::mpd3dDevice, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), &temp);
	mipChain.Release();
	image.Release();
	if (FAILED(hr)){
		return NULL;
	}
	
	return temp;
}

int GetNumMipLevels(int width, int height)
{
	int numLevels = 1;
	while ((width > 1) || (height > 1))
	{
		width = max(width / 2, 1);
		height = max(height / 2, 1);
		++numLevels;
	}

	return numLevels;
}

#include "Graphic/Loader/HDR.h"
#include <vector>
#include "Game/RenderingSystem.h"
ID3D11ShaderResourceView* TextureFileData::LoadRGBE(){

	HRESULT hr;
	HDRLoaderResult result;
	bool temp = true;
	temp = temp&&HDRLoader::load(m_FileName.c_str(), result);
	if (!temp)return NULL;
	int mipnum = GetNumMipLevels(result.width, result.height);

	D3D11_TEXTURE2D_DESC tex_desc;
	ZeroMemory(&tex_desc, sizeof(tex_desc));
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	//レンダーターゲットとして使用＆シェーダリソースとして利用
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	tex_desc.Width = result.width;
	tex_desc.Height = result.height;

	tex_desc.MipLevels = mipnum;

	std::vector<D3D11_SUBRESOURCE_DATA> data(mipnum);
	for (auto& d : data){
		d.pSysMem = result.cols;
		d.SysMemPitch = result.width * sizeof(float) * 4;
		d.SysMemSlicePitch = 0;
	}

	ID3D11Texture2D *tex2d;
	hr = Device::mpd3dDevice->CreateTexture2D(&tex_desc, &data.data()[0], &tex2d);
	if (FAILED(hr)){
		delete[] result.cols;
		return NULL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	memset(&srv_desc, 0, sizeof(srv_desc));
	srv_desc.Format = tex_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = tex_desc.MipLevels;

	ID3D11ShaderResourceView* pShaderResourceView;
	hr = Device::mpd3dDevice->CreateShaderResourceView(tex2d, &srv_desc, &pShaderResourceView);
	if (FAILED(hr)){
		return NULL;
	}

	tex2d->Release();
	delete[] result.cols;

#if _DRAW_MULTI_THREAD
	ここスレッドセーフじゃない
#endif
	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	render->m_Context->GenerateMips(pShaderResourceView);


	return pShaderResourceView;
}