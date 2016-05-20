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
	HRESULT Create(const char* FileName, const char* FuncName);

	HRESULT Create(const char* FileName, const char* FuncName, const D3D11_SO_DECLARATION_ENTRY* pEntres, UINT NumEntries, const UINT* pBufferStrides, UINT NumStrides);

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