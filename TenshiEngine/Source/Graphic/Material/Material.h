#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"

#include "Texture.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"
#include "Graphic/Shader/Shader.h"
class AssetLoader;
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
	virtual void CreateShader(ShaderAsset& asset) = 0;
	virtual void SetTexture(const TextureAsset& Tex, UINT Slot = 0) = 0;
	virtual ShaderAsset GetShader() = 0;
	virtual TextureAsset GetTexture(UINT Slot = 0) = 0;

	virtual XMFLOAT4 GetAlbedo() = 0;
	virtual XMFLOAT4 GetSpecular() = 0;
	virtual XMFLOAT4 GetAmbient() = 0;
	virtual XMFLOAT2 GetTextureScale() = 0;
	virtual XMFLOAT2 GetHeightPower() = 0;
	virtual XMFLOAT4 GetNormalScale() = 0;
	virtual XMFLOAT2 GetOffset() = 0;
	virtual float GetEmissivePowor() = 0;

	virtual void SetAlbedo(const XMFLOAT4& value) = 0;
	virtual void SetSpecular(const XMFLOAT4& value) = 0;
	virtual void SetAmbient(const XMFLOAT4& value) = 0;
	virtual void SetTextureScale(const XMFLOAT2& value) = 0;
	virtual void SetHeightPower(const XMFLOAT2& value) = 0;
	virtual void SetNormalScale(const XMFLOAT4& value) = 0;
	virtual void SetOffset(const XMFLOAT2& value) = 0;
	virtual void SetEmissivePowor(float value) = 0;
};

class I_ioHelper;
class Material : public IMaterial{
public:
	Material();
	~Material();

	HRESULT Create(const char* shaderFileName = "EngineResource/DeferredPrePass.fx");
	HRESULT Create(ShaderAsset& asset);
	//HRESULT Create(ConstantBuffer<cbChangesMaterial> const& cbMaterial, ConstantBuffer<cbChangesUseTexture> const& cbUseTexture);
	void SetShader(bool UseAnime, ID3D11DeviceContext* context) const;
	void SetShader_VS(bool UseAnime, ID3D11DeviceContext* context) const;
	void SetShader_PS(ID3D11DeviceContext* context) const;
	void VSSetShaderResources(ID3D11DeviceContext* context , ForcedMaterialFilter::Enum filter = ForcedMaterialFilter::ALL) const;
	void PSSetShaderResources(ID3D11DeviceContext* context , ForcedMaterialFilter::Enum filter = ForcedMaterialFilter::ALL) const;
	void GSSetShaderResources(ID3D11DeviceContext* context , ForcedMaterialFilter::Enum filter = ForcedMaterialFilter::ALL) const;
	void SetTexture(const char* FileName, UINT Slot = 0);
	void SetTexture(const MD5::MD5HashCode& hash, UINT Slot = 0);
	void SetTexture(const Texture& Tex, UINT Slot = 0);
	void SetTexture(const TextureAsset& Tex, UINT Slot = 0) override;
	

	void IO_Data(I_ioHelper* io, const std::string& materialPath = "");

	bool IsCreate(){
		return mCBMaterial.mBuffer != NULL;
	}

	void CreateShader(const char* shaderFileName = "EngineResource/DeferredPrePass.fx");
	void CreateShader(ShaderAsset& asset) override;

	ShaderAsset GetShader() override;
	TextureAsset GetTexture(UINT Slot = 0) override;

	XMFLOAT4 GetAlbedo() override;
	XMFLOAT4 GetSpecular() override;
	XMFLOAT4 GetAmbient() override;
	XMFLOAT2 GetTextureScale() override;
	XMFLOAT2 GetHeightPower() override;
	XMFLOAT4 GetNormalScale() override;
	XMFLOAT2 GetOffset() override;
	float GetEmissivePowor() override;

	void SetAlbedo(const XMFLOAT4& value) override;
	void SetSpecular(const XMFLOAT4& value) override;
	void SetAmbient(const XMFLOAT4& value) override;
	void SetTextureScale(const XMFLOAT2& value) override;
	void SetHeightPower(const XMFLOAT2& value) override;
	void SetNormalScale(const XMFLOAT4& value) override;
	void SetOffset(const XMFLOAT2& value) override;
	void SetEmissivePowor(float value) override;

private:

	XMFLOAT4 mDiffuse;
	XMFLOAT4 mSpecular;
	XMFLOAT4 mAmbient;
	XMFLOAT2 mTexScale;
	XMFLOAT2 mHeightPower;
	XMFLOAT4 mNormalScale;
	XMFLOAT2 mOffset;
	float mEmissivePowor;
	//unsigned char toon_index;
	//unsigned char edge_flag;//	—ÖŠsƒtƒ‰ƒO

	Shader mShader;
	ConstantBuffer<cbChangesMaterial> mCBMaterial;
	ConstantBuffer<cbChangesUseTexture> mCBUseTexture;
	Texture mTexture[8];

	friend AssetLoader;
	friend AssetDataTemplate<MaterialFileData>;
	friend MaterialFileData;
};