#pragma once

#include <D3D11.h>
#include "MySTL/ptr.h"
#include <string>

#include "Engine/AssetDataBase.h"

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
	HRESULT Create(ID3D11ShaderResourceView* pTexture);
	HRESULT Create(const char* FileName);
	void PSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const;
	void GSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const;
	void CSSetShaderResources(ID3D11DeviceContext* context, UINT Slot) const;

	shared_ptr<Release_self<ID3D11Resource>> GetResource();

	void ExportData(File& f);
	std::string mFileName;

private:
	TextureAssetDataPtr mTextureAssetData;
	//static Texture mNullTexture;
};