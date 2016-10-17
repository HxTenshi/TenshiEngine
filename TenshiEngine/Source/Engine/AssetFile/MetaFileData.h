#pragma once
#include "AssetFileData.h"
#include "Library/MD5.h"

class MetaFileData : public AssetFileData{
public:
	MetaFileData();
	~MetaFileData();

	bool Create(const char* filename) override;


	const MD5::MD5HashCode* GetHash() const{
		return &m_Hash;
	}

private:

	//ÉRÉsÅ[ã÷é~
	MetaFileData(const MetaFileData&) = delete;
	MetaFileData& operator=(const MetaFileData&) = delete;

	MD5::MD5HashCode m_Hash;
};
