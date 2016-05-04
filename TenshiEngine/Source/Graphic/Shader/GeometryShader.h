#pragma once

#include "ShaderFile.h"
#include "Device/DirectX11Device.h"


class GeometryStreamOutputShader : public ShaderFile {
public:
	GeometryStreamOutputShader()
		: mpGeometryShader(NULL)
	{

	}
	~GeometryStreamOutputShader()
	{

	}
	//HRESULT Create(const char* FileName){
	//	return Create(FileName, "GS");
	//}
	HRESULT Create(const char* FileName, const char* FuncName){
		HRESULT hr = S_OK;
		// Compile the vertex shader
		ID3DBlob* pVSBlob = NULL;
		hr = CompileShaderFromFile(FileName, FuncName, "gs_4_0", &pVSBlob);
		if (FAILED(hr))
		{
			MessageBox(NULL,
				"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. VS", "Error", MB_OK);
			return hr;
		}

		// Create the vertex shader

		hr = Device::mpd3dDevice->CreateGeometryShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpGeometryShader);
		pVSBlob->Release();

		return S_OK;
	}

	HRESULT Create(const char* FileName, const char* FuncName, const D3D11_SO_DECLARATION_ENTRY* pEntres,UINT NumEntries, const UINT* pBufferStrides, UINT NumStrides){
		HRESULT hr = S_OK;
		// Compile the vertex shader
		ID3DBlob* pVSBlob = NULL;
		hr = CompileShaderFromFile(FileName, FuncName, "gs_4_0", &pVSBlob);
		if (FAILED(hr))
		{
			MessageBox(NULL,
				"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. VS", "Error", MB_OK);
			return hr;
		}

		// Create the vertex shader
		
		//hr = Device::mpd3dDevice->CreateGeometryShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mpGeometryShader);
		hr = Device::mpd3dDevice->CreateGeometryShaderWithStreamOutput(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), pEntres, NumEntries, pBufferStrides, NumStrides, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &mpGeometryShader);
		pVSBlob->Release();

		return S_OK;
	}

	void SetShader(ID3D11DeviceContext* context) const{
		if (mpGeometryShader)
			context->GSSetShader(mpGeometryShader, NULL, 0);
	}

	void Release(){

		if (mpGeometryShader) mpGeometryShader->Release();
	}
private:

	ID3D11GeometryShader*		mpGeometryShader;
};