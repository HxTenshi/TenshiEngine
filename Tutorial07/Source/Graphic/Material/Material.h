#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"

#include "Texture.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"
#include "Graphic/Shader/Shader.h"

class Material{
public:
	Material();
	~Material();

	HRESULT Create(const char* shaderFileName = "DeferredPrePass.fx");
	HRESULT Create(ConstantBuffer<cbChangesMaterial> const& cbMaterial, ConstantBuffer<cbChangesUseTexture> const& cbUseTexture);
	void SetShader() const;
	void PSSetShaderResources() const;
	void SetTexture(const char* FileName, UINT Slot = 0);
	void SetTexture(const Texture& Tex, UINT Slot = 0);

	void ExportData(File& f);

	bool IsCreate(){
		return mCBMaterial.mBuffer != NULL;
	}
public:

	XMFLOAT4 mDiffuse;
	XMFLOAT4 mSpecular;
	XMFLOAT4 mAmbient;
	//unsigned char toon_index;
	//unsigned char edge_flag;//	—ÖŠsƒtƒ‰ƒO

private:
public:
	Shader mShader;
	ConstantBuffer<cbChangesMaterial> mCBMaterial;
	ConstantBuffer<cbChangesUseTexture> mCBUseTexture;
	Texture mTexture[4];
};