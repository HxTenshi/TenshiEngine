#pragma once

#include "ShaderFile.h"
#include "Device/DirectX11Device.h"

class PixelShader : public ShaderFile{
public:
	PixelShader()
		: mpPixelShader(NULL)
	{

	}
	~PixelShader()
	{
		Release();
	}
	HRESULT Create(const char* FileName){
		return Create(FileName, "PS");
	}
	HRESULT Create(const char* FileName, const char* FuncName){
		HRESULT hr = S_OK;
		// Compile the pixel shader
		ID3DBlob* pPSBlob = NULL;
		hr = CompileShaderFromFile(FileName, FuncName, "ps_4_0", &pPSBlob);
		if (FAILED(hr))
		{
			//MessageBox(NULL,"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. PS", "Error", MB_OK);
			return hr;
		}

		// Create the pixel shader
		hr = Device::mpd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &mpPixelShader);
		pPSBlob->Release();
		if (FAILED(hr))
			return hr;

		return S_OK;
	}

	void SetShader(ID3D11DeviceContext* context) const{
		if (mpPixelShader)
			context->PSSetShader(mpPixelShader, NULL, 0);
	}

private:
	void Release(){

		if (mpPixelShader){
			mpPixelShader->Release();
			mpPixelShader = NULL;
		}
	}
private:
	ID3D11PixelShader*	mpPixelShader;
};