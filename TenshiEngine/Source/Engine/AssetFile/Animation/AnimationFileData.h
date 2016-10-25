#pragma once


#include <vector>

#include "Engine/AssetLoader.h"
#include "../AssetFileData.h"
#include "Graphic/Model/AnimeData.h"

class AnimationFileData : public AssetFileData {
public:
	AnimationFileData();
	~AnimationFileData();

	bool Create(const char* filename) override;

	const AnimeData& GetAnimeData() const;

private:
	//ÉRÉsÅ[ã÷é~
	AnimationFileData(const AnimationFileData&) = delete;
	AnimationFileData& operator=(const AnimationFileData&) = delete;

	AnimeData m_AnimeData;
};
