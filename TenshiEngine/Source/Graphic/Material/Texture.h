#pragma once

#include <D3D11.h>
#include "MySTL/ptr.h"
#include <string>

#include "Engine/AssetDataBase.h"

class File;

class Texture{
public:
	Texture();
	~Texture();
	HRESULT Create(ID3D11ShaderResourceView* pTexture);
	HRESULT Create(const char* FileName);
	void PSSetShaderResources(UINT Slot) const;

	void ExportData(File& f);
	std::string mFileName;

private:
	TextureAssetDataPtr mTextureAssetData;
	//static Texture mNullTexture;
};