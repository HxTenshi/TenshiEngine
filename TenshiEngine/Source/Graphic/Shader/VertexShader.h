#pragma once

#include "ShaderFile.h"
#include "InputLayout.h"
#include "Device/DirectX11Device.h"

class VertexShader : public ShaderFile {
public:
	VertexShader()
		: mpVertexShader(NULL)
	{

	}
	~VertexShader()
	{
		Release();
	}
	HRESULT Create(const char* FileName){
		return Create(FileName, "VS");
	}
	HRESULT Create(const char* FileName, const char* FuncName){
		HRESULT hr = S_OK;
		// Compile the vertex shader
		ID3DBlob* pVSBlob = NULL;
		hr = CompileShaderFromFile(FileName, FuncName, "vs_4_0", &pVSBlob);
		if (FAILED(hr))
		{
			return hr;
		}

		// Create the vertex shader
		hr = Device::mpd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpVertexShader);
		if (FAILED(hr) || hr==S_FALSE)
		{
			pVSBlob->Release();
			return hr;
		}

		hr = mInputLayout.Init(pVSBlob);
		pVSBlob->Release();
		if (FAILED(hr))
			return hr;

		return S_OK;
	}
	HRESULT Create(const char* FileName, const char* FuncName, const D3D11_INPUT_ELEMENT_DESC* layout, UINT num){
		HRESULT hr = S_OK;
		// Compile the vertex shader
		ID3DBlob* pVSBlob = NULL;
		hr = CompileShaderFromFile(FileName, FuncName, "vs_4_0", &pVSBlob);
		if (FAILED(hr))
		{
			return hr;
		}

		// Create the vertex shader
		hr = Device::mpd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpVertexShader);
		if (FAILED(hr) || hr == S_FALSE)
		{
			pVSBlob->Release();
			return hr;
		}

		hr = mInputLayout.Init(pVSBlob, layout,num);
		pVSBlob->Release();
		if (FAILED(hr))
			return hr;

		return S_OK;
	}

	void SetShader() const{
		if (!mpVertexShader)return;
		Device::mpImmediateContext->VSSetShader(mpVertexShader, NULL, 0);
		mInputLayout.SetInputLayout();
	}

private:
	void Release(){

		if (mpVertexShader){
			mpVertexShader->Release();
			mpVertexShader = NULL;
		}
		mInputLayout.Rerease();
	}
private:

	ID3D11VertexShader*		mpVertexShader;
	InputLayout				mInputLayout;
};

