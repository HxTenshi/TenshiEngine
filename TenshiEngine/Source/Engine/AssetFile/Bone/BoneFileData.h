#pragma once


#include <vector>

#include "Graphic/model/ModelStructs.h"
#include "Engine/AssetLoader.h"
#include "../AssetFileData.h"

class BoneFileData : public AssetFileData{
public:
	BoneFileData();
	~BoneFileData();

	bool Create(const char* filename) override;
	bool FileUpdate() override;

	const BoneData& GetBoneData() const;

private:
	//コピー禁止
	BoneFileData(const BoneFileData&) = delete;
	BoneFileData& operator=(const BoneFileData&) = delete;

	BoneData m_BoneData;	// ボーンデータ
};
