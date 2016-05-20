#include "ComputeShader.h"
#include "Window/Window.h"

HRESULT ComputeShader::Create(const char* FileName, const char* FuncName){
	HRESULT hr = S_OK;
	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(FileName, FuncName, "cs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		Window::AddLog(std::string(FileName) + ":" + std::string(FuncName) + "ì¬¸”s");
		return hr;
	}

	// Create the vertex shader

	hr = Device::mpd3dDevice->CreateComputeShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpComputeShader);
	pVSBlob->Release();

	return S_OK;

}
