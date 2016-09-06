#include "GenerateMetaFile.h"

#include "Library/picojson.h"
#include "Library/MD5.h"
#include <fstream>

void MakeMetaFile(std::string FileName){


	MD5::MD5HashCoord m_Hash;
	if (!MD5::GenerateMD5(m_Hash))return;

	FileName += ".meta";

	std::ofstream jsonfile(FileName);

	if (jsonfile.fail()){
		jsonfile.open(FileName);
		if (jsonfile.fail()){
			return;
		}
	}


	picojson::object obj;
	obj.insert(std::make_pair(std::string("key0"), picojson::value((double)m_Hash.key_i[0])));
	obj.insert(std::make_pair(std::string("key1"), picojson::value((double)m_Hash.key_i[1])));
	obj.insert(std::make_pair(std::string("key2"), picojson::value((double)m_Hash.key_i[2])));
	obj.insert(std::make_pair(std::string("key3"), picojson::value((double)m_Hash.key_i[3])));
	picojson::value out(obj);
	jsonfile << out;
}