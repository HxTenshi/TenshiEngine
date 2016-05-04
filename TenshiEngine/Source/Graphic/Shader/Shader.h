#pragma once

#include "PixelShader.h"
#include "VertexShader.h"
#include "MySTL/Ptr.h"

#include "Engine/AssetDataBase.h"

class Shader{
public:
	Shader(){

	}
	~Shader(){

	}

	void Create(const char* fileName);

	void SetShader(bool UseAnime, ID3D11DeviceContext* context) const;

private:
	ShaderAssetDataPtr m_ShaderAssetData;
};