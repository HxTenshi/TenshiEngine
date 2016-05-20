#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"

#include "Texture.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"
#include "Graphic/Shader/Shader.h"

class IMaterial{
public:
	virtual ~IMaterial(){};
};

class Material : public IMaterial{
public:
	Material();
	~Material();

	HRESULT Create(const char* shaderFileName = "EngineResource/DeferredPrePass.fx");
	//HRESULT Create(ConstantBuffer<cbChangesMaterial> const& cbMaterial, ConstantBuffer<cbChangesUseTexture> const& cbUseTexture);
	void SetShader(bool UseAnime, ID3D11DeviceContext* context) const;
	void VSSetShaderResources(ID3D11DeviceContext* context) const;
	void PSSetShaderResources(ID3D11DeviceContext* context) const;
	void GSSetShaderResources(ID3D11DeviceContext* context) const;
	void SetTexture(const char* FileName, UINT Slot = 0);
	void SetTexture(const Texture& Tex, UINT Slot = 0);

	void ExportData(File& f);

	bool IsCreate(){
		return mCBMaterial.mBuffer != NULL;
	}

	void CreateShader(const char* shaderFileName);
public:

	XMFLOAT4 mDiffuse;
	XMFLOAT4 mSpecular;
	XMFLOAT4 mAmbient;
	XMFLOAT2 mTexScale;
	XMFLOAT2 mHeightPower;
	XMFLOAT4 mNormalScale;
	XMFLOAT2 mOffset;
	//unsigned char toon_index;
	//unsigned char edge_flag;//	—ÖŠsƒtƒ‰ƒO

private:
public:
	Shader mShader;
	ConstantBuffer<cbChangesMaterial> mCBMaterial;
	ConstantBuffer<cbChangesUseTexture> mCBUseTexture;
	Texture mTexture[8];
};