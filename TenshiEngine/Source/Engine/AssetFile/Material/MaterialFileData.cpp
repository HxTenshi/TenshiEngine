#include "MaterialFileData.h"
#include "Game/Game.h"
#include "Graphic/Material/Material.h"
#include "Engine/AssetLoad.h"

MaterialFileData::MaterialFileData() {
	m_Material = NULL;
}
MaterialFileData::~MaterialFileData() {
	if (m_Material) {
		delete m_Material;
		m_Material = NULL;
	}
	m_FileName = "";
}

bool MaterialFileData::Create(const char* filename) {

	FileInputHelper io(filename, NULL);

	if (io.error) {
		return false;
	}
	if (m_Material) {
		delete m_Material;
		m_Material = NULL;
	}
	if (!m_Material) {
		m_Material = new Material();
	}

	auto dir = forward_than_find_last_of(filename, "/") + "/";

	m_Material->IO_Data(&io, dir);

	ShaderAsset shader;
	if (!m_Material->mShader.GetAsset().m_Hash.IsNull()) {
		AssetLoad::Instance(m_Material->mShader.GetAsset().m_Hash, shader);
	}
	if (shader.IsLoad()) {
		m_Material->Create(shader);
	}
	else {
		m_Material->Create();
	}
	m_FileName = filename;

	return true;
}
void MaterialFileData::SaveFile() {

	if (!m_Material) {
		return;
	}
	FileOutputHelper io(m_FileName, NULL);

	if (io.error) {
		return;
	}

	m_Material->IO_Data(&io, "");
}