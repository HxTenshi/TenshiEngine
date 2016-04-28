#include "ShaderFileData.h"


ShaderFileData::ShaderFileData(){

	m_Shader = NULL;
}
ShaderFileData::~ShaderFileData(){
	if (m_Shader){
		delete m_Shader;
		m_Shader = NULL;
	}
	m_FileName = "";
}

void ShaderFileData::Create(const char* filename){


	if (m_Shader){
		delete m_Shader;
		m_Shader = NULL;
	}
	m_Shader = new ShaderData();
	m_FileName = filename;

	m_Shader->Create(m_FileName.c_str());

}

void ShaderFileData::FileUpdate(){
	m_Shader->Create(m_FileName.c_str());
}
