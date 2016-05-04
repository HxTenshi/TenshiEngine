#pragma once

#include "Window/Window.h"
#include "ShaderFile.h"
#include "Device/DirectX11Device.h"


class ComputeShader : public ShaderFile {
public:
	ComputeShader()
		: mpComputeShader(NULL)
	{

	}
	~ComputeShader()
	{

	}
	HRESULT Create(const char* FileName, const char* FuncName){
		HRESULT hr = S_OK;
		// Compile the vertex shader
		ID3DBlob* pVSBlob = NULL;
		hr = CompileShaderFromFile(FileName, FuncName, "cs_5_0", &pVSBlob);
		if (FAILED(hr))
		{
			MessageBox(Window::GetMainHWND(),
				"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. CS", "Error", MB_OK);
			return hr;
		}

		// Create the vertex shader

		hr = Device::mpd3dDevice->CreateComputeShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpComputeShader);
		pVSBlob->Release();

		return S_OK;

	}

	void SetShader(ID3D11DeviceContext* context) const{
		if (mpComputeShader)
			context->CSSetShader(mpComputeShader, NULL, 0);
	}

	void Release(){

		if (mpComputeShader) mpComputeShader->Release();
	}
private:

	ID3D11ComputeShader*		mpComputeShader;
};