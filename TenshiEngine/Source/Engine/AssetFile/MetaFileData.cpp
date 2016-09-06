#include "MetaFileData.h"

#include <fstream>
#include "Library/picojson.h"

MetaFileData::MetaFileData(){

}
MetaFileData::~MetaFileData(){

}

bool MetaFileData::Create(const char* filename){

	m_FileName = filename;
	picojson::value json;

	std::ifstream jsonfile(m_FileName);
	if (jsonfile.fail()){
		return false;
	}
	jsonfile >> json;

	auto obj = json.get<picojson::object>();
	m_Hash.key_i[0] = (int)obj["key0"].get<double>();
	m_Hash.key_i[1] = (int)obj["key1"].get<double>();
	m_Hash.key_i[2] = (int)obj["key2"].get<double>();
	m_Hash.key_i[3] = (int)obj["key3"].get<double>();

	return true;

}

bool MetaFileData::FileUpdate(){
	return Create(m_FileName.c_str());
}
