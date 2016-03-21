#pragma once


#include <vector>

#include "Graphic/model/ModelStructs.h"
#include "Engine/AssetLoader.h"

class BoneFileData{
public:
	BoneFileData();
	~BoneFileData();

	void Create(const char* filename);

	const BoneData& GetBoneData() const;

private:
	//コピー禁止
	BoneFileData(const BoneFileData&) = delete;
	BoneFileData& operator=(const BoneFileData&) = delete;

	BoneData m_BoneData;	// ボーンデータ
};
