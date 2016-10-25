#include "AnimationFileData.h"

AnimationFileData::AnimationFileData()
{
}

AnimationFileData::~AnimationFileData()
{
	m_AnimeData.Clear();
}

bool AnimationFileData::Create(const char * filename)
{
	m_FileName = filename;
	return m_AnimeData.Create(filename);
}

const AnimeData & AnimationFileData::GetAnimeData() const
{
	return m_AnimeData;
}
