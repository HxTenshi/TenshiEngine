#include "Shader.h"


#include "Engine/AssetFile/Shader/ShaderFileData.h"

void Shader::Create(const char* fileName){
	AssetDataBase::FilePath2Hash(fileName, m_Hash);
	AssetDataBase::Instance(m_Hash, m_ShaderAssetData);
}
void Shader::Create(const MD5::MD5HashCode & hash)
{
	m_Hash = hash;
	AssetDataBase::Instance(m_Hash, m_ShaderAssetData);
}
void Shader::Create(ShaderAsset& asset){
	m_Hash = asset.m_Hash;
	m_ShaderAssetData = asset.m_Ptr;
}

void Shader::SetShader(bool UseAnime, ID3D11DeviceContext* context) const{
	if (!m_ShaderAssetData)return;
	m_ShaderAssetData->GetFileData()->GetShader()->SetShader(UseAnime,context);
}

void Shader::SetShader_VS(bool UseAnime, ID3D11DeviceContext* context) const{
	if (!m_ShaderAssetData)return;
	m_ShaderAssetData->GetFileData()->GetShader()->SetShader_VS(UseAnime, context);
}
void Shader::SetShader_PS(ID3D11DeviceContext* context) const{
	if (!m_ShaderAssetData)return;
	m_ShaderAssetData->GetFileData()->GetShader()->SetShader_PS(context);
}