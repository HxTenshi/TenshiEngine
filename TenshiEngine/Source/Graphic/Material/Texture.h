#pragma once

#include <D3D11.h>
#include "MySTL/ptr.h"
#include <string>

#include "Engine/AssetDataBase.h"
#include "Engine/Assets.h"

#include "MySTL/Release_self.h"

class File;

class ITexture{
public:
	virtual ~ITexture(){};
};

class Texture : public ITexture{
public:
	Texture();
	~Texture();
	HRESULT Create(ID3D11Texture2D* pTexture);
	HRESULT Create(ID3D11ShaderResourceView* pTexture);
	HRESULT Create(const char* FileName);
	HRESULT Create(const MD5::MD5HashCode& hash);
	HRESULT Create(const TextureAsset& tex);
	void PSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const;
	void GSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const;
	void CSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const;

	shared_ptr<Release_self<ID3D11Resource>> GetResource();
	std::string GetFileName()const {
		return mFileName;
	}
	MD5::MD5HashCode GetHash()const{
		return mHash;
	}

	TextureAsset GetAsset();
	std::string mFileName;
private:
	MD5::MD5HashCode mHash;
	TextureAssetDataPtr mTextureAssetData;
	//static Texture mNullTexture;
};