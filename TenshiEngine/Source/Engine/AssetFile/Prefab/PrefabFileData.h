#pragma once

#include "MySTL/ptr.h"
#include "Library/picojson.h"
#include "../AssetFileData.h"

class I_InputHelper;
class MemoryInputHelper;
class Actor;

class PrefabFileData : public AssetFileData{
public:
	PrefabFileData();
	~PrefabFileData();

	void Create(const char* filename);
	void FileUpdate(const char* filename);
	picojson::value Apply();

	shared_ptr<I_InputHelper> GetData() const;
	Actor* GetActor() const{
		return m_PrefabActor;
	}

	picojson::value* GetParam() const{
		return mBeforeParam;
	}

	std::string GetFileName()const{
		return mFileName;
	}

private:
	//ÉRÉsÅ[ã÷é~
	PrefabFileData(const PrefabFileData&) = delete;
	PrefabFileData& operator=(const PrefabFileData&) = delete;

	Actor* m_PrefabActor;
	std::string mFileName;
	picojson::value* mBeforeParam;
};
