#pragma once


#include <vector>

#include "Engine/AssetLoader.h"
#include "../AssetFileData.h"

class CMFSession;
class ReleaseMovies;

class MovieFileData : public AssetFileData{
public:
	MovieFileData();
	~MovieFileData();

	void Create(const char* filename) override;
	void FileUpdate() override;

	CMFSession* GetMovieData() const;

private:
	//ÉRÉsÅ[ã÷é~
	MovieFileData(const MovieFileData&) = delete;
	MovieFileData& operator=(const MovieFileData&) = delete;

	CMFSession* m_MovieData;

	shared_ptr<ReleaseMovies> mMovies;
};
