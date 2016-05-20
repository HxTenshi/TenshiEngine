#include "GeometryShader.h"
#include "Window/Window.h"

HRESULT GeometryStreamOutputShader::Create(const char* FileName, const char* FuncName){
	HRESULT hr = S_OK;
	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(FileName, FuncName, "gs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		Window::AddLog(std::string(FileName) + ":" + std::string(FuncName) + "ì¬¸”s");
		//MessageBox(NULL,
		//	"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. VS", "Error", MB_OK);
		return hr;
	}

	// Create the vertex shader

	hr = Device::mpd3dDevice->CreateGeometryShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpGeometryShader);
	pVSBlob->Release();

	return S_OK;
}

HRESULT GeometryStreamOutputShader::Create(const char* FileName, const char* FuncName, const D3D11_SO_DECLARATION_ENTRY* pEntres, UINT NumEntries, const UINT* pBufferStrides, UINT NumStrides){
	HRESULT hr = S_OK;
	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(FileName, FuncName, "gs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		Window::AddLog(std::string(FileName) + ":" + std::string(FuncName) + "ì¬¸”s");
		//MessageBox(NULL,
		//	"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. VS", "Error", MB_OK);
		return hr;
	}

	// Create the vertex shader

	//hr = Device::mpd3dDevice->CreateGeometryShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpGeometryShader);
	hr = Device::mpd3dDevice->CreateGeometryShaderWithStreamOutput(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), pEntres, NumEntries, pBufferStrides, NumStrides, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &mpGeometryShader);
	pVSBlob->Release();

	return S_OK;
}