#pragma once

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
	HRESULT Create(const char* FileName, const char* FuncName);

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