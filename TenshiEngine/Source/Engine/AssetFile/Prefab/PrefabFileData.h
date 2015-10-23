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

	bool Create(const char* filename) override;
	bool FileUpdate() override;
	picojson::value Apply();

	shared_ptr<I_InputHelper> GetData() const;
	Actor* GetActor() const{
		return m_PrefabActor;
	}

	picojson::value* GetParam() const{
		return mBeforeParam;
	}


private:
	//ÉRÉsÅ[ã÷é~
	PrefabFileData(const PrefabFileData&) = delete;
	PrefabFileData& operator=(const PrefabFileData&) = delete;

	Actor* m_PrefabActor;
	picojson::value* mBeforeParam;
};
