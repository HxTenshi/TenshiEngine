#include "Shader.h"


#include "Engine/AssetFile/Shader/ShaderFileData.h"
#include "Engine/AssetLoad.h"

void ErrorShaderSet(ID3D11DeviceContext* context){
	static bool f = true;
	static Shader g_Error;
	if (f) {
		f = false;
		g_Error.Create("EngineResource/ErrorShader.fx");
	}
	g_Error.SetShader(false,context);
}

void Shader::Create(const char* fileName){

	m_ShaderAsset.Free();
	MD5::MD5HashCode hash;
	AssetDataBase::FilePath2Hash(fileName, hash);
	AssetLoad::Instance(hash, m_ShaderAsset);
}
void Shader::Create(const MD5::MD5HashCode & hash)
{
	m_ShaderAsset.Free();
	AssetLoad::Instance(hash, m_ShaderAsset);
}
void Shader::Create(ShaderAsset& asset){
	if (!asset.IsLoad()) {
		AssetLoad::Instance(asset.m_Hash, asset);
	}
	m_ShaderAsset = asset;
}

void Shader::SetShader(bool UseAnime, ID3D11DeviceContext* context) const{
	if (!m_ShaderAsset.IsLoad()) {
		ErrorShaderSet(context);
		return;
	}
	m_ShaderAsset.Get()->GetShader()->SetShader(UseAnime,context);
}

void Shader::SetShader_VS(bool UseAnime, ID3D11DeviceContext* context) const{
	if (!m_ShaderAsset.IsLoad()) {
		ErrorShaderSet(context);
		return;
	}
	m_ShaderAsset.Get()->GetShader()->SetShader_VS(UseAnime, context);
}
void Shader::SetShader_PS(ID3D11DeviceContext* context) const{
	if (!m_ShaderAsset.IsLoad()) {
		ErrorShaderSet(context);
		return;
	}
	m_ShaderAsset.Get()->GetShader()->SetShader_PS(context);
}

ShaderAsset& Shader::GetAsset()
{
	return m_ShaderAsset;
}
