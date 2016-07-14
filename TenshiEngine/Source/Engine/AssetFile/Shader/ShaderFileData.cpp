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

bool ShaderFileData::Create(const char* filename){


	m_FileName = filename;
	if (m_Shader){
		delete m_Shader;
		m_Shader = NULL;
	}
	m_Shader = new ShaderData();

	m_Shader->Create(m_FileName.c_str());


	return true;
}

bool ShaderFileData::FileUpdate(){
	m_Shader->Create(m_FileName.c_str());

	return true;
}
