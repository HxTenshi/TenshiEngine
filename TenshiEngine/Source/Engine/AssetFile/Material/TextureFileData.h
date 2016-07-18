#pragma once
#include "../AssetFileData.h"

struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

class TextureData{
public:
	TextureData()
		:mpTextureRV(NULL)
		, mpSamplerLinear(NULL)
	{
	}
	~TextureData(){
		Release();
	}
	void Release();

	ID3D11ShaderResourceView*	mpTextureRV;
	ID3D11SamplerState*			mpSamplerLinear;
};

class TextureFileData : public AssetFileData{
public:
	TextureFileData();
	~TextureFileData();

	bool Create(const char* filename) override;
	bool FileUpdate() override;

	void SetTexture(ID3D11ShaderResourceView* TextureRV, ID3D11SamplerState* SamplerLinear){

		if (m_FileName == ""){
			m_TextureData.mpTextureRV = TextureRV;
			m_TextureData.mpSamplerLinear = SamplerLinear;
		}
	}

	const TextureData* GetTexture() const{
		return &m_TextureData;
	}

private:

	ID3D11ShaderResourceView* LoadDirectXTex();


	//�R�s�[�֎~
	TextureFileData(const TextureFileData&) = delete;
	TextureFileData& operator=(const TextureFileData&) = delete;

	TextureData m_TextureData;
};