#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"

#include "Texture.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"
#include "Graphic/Shader/Shader.h"

struct ForcedMaterialFilter{
	enum Enum{
		None = 0,
		Texture_0 = 1 << 0,
		Texture_1 = 1 << 1,
		Texture_2 = 1 << 2,
		Texture_3 = 1 << 3,
		Texture_4 = 1 << 4,
		Texture_5 = 1 << 5,
		Texture_6 = 1 << 6,
		Texture_7 = 1 << 7,
		Texture_ALL = Texture_0 + Texture_1 + Texture_2 + Texture_3 + Texture_4 + Texture_5 + Texture_6 + Texture_7,
		Color = 1 << 8,
		Shader_VS = 1 << 9,
		Shader_PS = 1 << 10,
		ALL = 0xFFFFFFFF
	};
};

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
	void SetShader_VS(bool UseAnime, ID3D11DeviceContext* context) const;
	void SetShader_PS(ID3D11DeviceContext* context) const;
	void VSSetShaderResources(ID3D11DeviceContext* context , ForcedMaterialFilter::Enum filter = ForcedMaterialFilter::ALL) const;
	void PSSetShaderResources(ID3D11DeviceContext* context , ForcedMaterialFilter::Enum filter = ForcedMaterialFilter::ALL) const;
	void GSSetShaderResources(ID3D11DeviceContext* context , ForcedMaterialFilter::Enum filter = ForcedMaterialFilter::ALL) const;
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