#pragma once
#include <string>

class AssetFileData{
public:
	AssetFileData(){}
	virtual ~AssetFileData(){}

	std::string GetFileName()const{
		return m_FileName;
	}

	virtual bool Create(const char* filename) =0;
	virtual bool FileUpdate() =0;

private:
	//ÉRÉsÅ[ã÷é~
	AssetFileData(const AssetFileData&) = delete;
	AssetFileData& operator=(const AssetFileData&) = delete;

protected:
	std::string m_FileName;
};
