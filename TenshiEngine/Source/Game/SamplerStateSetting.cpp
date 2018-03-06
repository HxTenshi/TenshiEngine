#include "SamplerStateSetting.h"

#include "Device/DirectX11Device.h"
#include <D3D11.h>

SamplerStateSetting::SamplerStateSetting()
	:m_Saplers(8,NULL)
{
}

SamplerStateSetting::~SamplerStateSetting()
{
	Release();
}

void SamplerStateSetting::Initialize()
{


	HRESULT hr;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.MipLODBias = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;

	//LinearClamp、LinearWrap、PointClamp、PointWrap、AnisotropicClamp、および AnisotropicWrap

	sampDesc.MaxAnisotropy = 4;

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[0]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[2]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[4]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[1]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[3]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[5]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}

	sampDesc.MaxAnisotropy = 16;

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[6]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = Device::mpd3dDevice->CreateSamplerState(&sampDesc, &m_Saplers[7]);
	if (FAILED(hr)) {
		_SYSTEM_LOG_ERROR("SamplerStateの作成");
	}

}

void SamplerStateSetting::Release()
{

	for (auto sampler : m_Saplers) {
		if (sampler) {
			sampler->Release();
		}
	}
	m_Saplers.clear();
}

#include "Window/Window.h"

void SamplerStateSetting::Setting(ID3D11DeviceContext* context)
{

	int num = m_Saplers.size();
	num = min(num, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	
	//context->VSSetSamplers(0, num, m_Saplers.data());
	context->PSSetSamplers(0, num, m_Saplers.data());
	//context->CSSetSamplers(0, num, m_Saplers.data());
	//context->GSSetSamplers(0, num, m_Saplers.data());
}

void SamplerStateSetting::Free(ID3D11DeviceContext* context)
{
	ID3D11SamplerState* nullset[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
	context->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullset);
	context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullset);
	context->CSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullset);
	context->GSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullset);
}
