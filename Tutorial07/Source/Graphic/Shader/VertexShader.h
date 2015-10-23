#pragma once

#include "ShaderFile.h"
#include "InputLayout.h"

class VertexShader : public ShaderFile {
public:
	VertexShader()
		: mpVertexShader(NULL)
	{

	}
	~VertexShader()
	{

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
			MessageBox(NULL,
				"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. VS", "Error", MB_OK);
			return hr;
		}

		// Create the vertex shader
		hr = Device::mpd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpVertexShader);
		if (FAILED(hr))
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
			MessageBox(NULL,
				"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. VS", "Error", MB_OK);
			return hr;
		}

		// Create the vertex shader
		hr = Device::mpd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpVertexShader);
		if (FAILED(hr))
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
		Device::mpImmediateContext->VSSetShader(mpVertexShader, NULL, 0);
		mInputLayout.SetInputLayout();
	}

	void Release(){

		if (mpVertexShader) mpVertexShader->Release();
		mInputLayout.Rerease();
	}
private:

	ID3D11VertexShader*		mpVertexShader;
	InputLayout				mInputLayout;
};

