#include "InputLayout.h"

#include "Device/DirectX11Device.h"

InputLayout::InputLayout()
	: mpVertexLayout(NULL)
{

}
InputLayout::~InputLayout()
{

}

HRESULT InputLayout::Init(ID3DBlob* pVSBlob)
{
	HRESULT hr = S_OK;
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 },


		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = Device::mpd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mpVertexLayout);
	if (FAILED(hr))
		return hr;


	return S_OK;
}

HRESULT InputLayout::Init(ID3DBlob* pVSBlob, const D3D11_INPUT_ELEMENT_DESC* layout,UINT num)
{
	HRESULT hr = S_OK;
	// Create the input layout
	hr = Device::mpd3dDevice->CreateInputLayout(layout, num, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mpVertexLayout);
	if (FAILED(hr))
		return hr;


	return S_OK;
}

void InputLayout::SetInputLayout(ID3D11DeviceContext* context)const{
	// Set the input layout
	if (mpVertexLayout)
		context->IASetInputLayout(mpVertexLayout);
}

void InputLayout::Rerease()
{
	if (mpVertexLayout){
		mpVertexLayout->Release();
		mpVertexLayout = NULL;
	}
}
