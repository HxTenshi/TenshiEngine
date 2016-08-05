#pragma once


#include <d3d11.h>

class ShaderFile{
protected:
	ShaderFile(){}
	virtual ~ShaderFile(){}
public:
	//--------------------------------------------------------------------------------------
	// Helper for compiling shaders with D3DX11
	//--------------------------------------------------------------------------------------
	HRESULT CompileShaderFromFile(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
};