#include "GenerateMetaFile.h"

#include "Library/picojson.h"
#include "Library/MD5.h"
#include <fstream>
#include <filesystem>

void MakeMetaFile(std::string FileName){

	std::tr2::sys::path path(FileName);

	auto p = path.extension().string().find("~");
	if (!path.has_extension() ||
		path.extension() == ".tmp" || std::string::npos != p ||
		path.extension() == ".h" || path.extension() == ".cpp" ||
		path.extension() == ".meta" ||
		path.parent_path() == "ScriptComponent/Scripts" ||
		(path.stem() == "desktop" && path.extension() == ".ini")) {
		return;
	}

	MD5::MD5HashCode m_Hash;
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