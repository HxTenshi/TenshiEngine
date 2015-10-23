#pragma once

#include "Engine/AssetLoader.h"
#include "../AssetFileData.h"

//class CMFSession;
//class ReleaseMovies;
//
//class MovieFileData : public AssetFileData{
//public:
//	MovieFileData();
//	~MovieFileData();
//
//	void Create(const char* filename) override;
//	void FileUpdate() override;
//
//	CMFSession* GetMovieData() const;
//
//private:
//	//コピー禁止
//	MovieFileData(const MovieFileData&) = delete;
//	MovieFileData& operator=(const MovieFileData&) = delete;
//
//	CMFSession* m_MovieData;
//
//	shared_ptr<ReleaseMovies> mMovies;
//};

class CMovieToTexture;

class MovieFileData : public AssetFileData{
public:
	MovieFileData();
	~MovieFileData();

	bool Create(const char* filename) override;
	bool FileUpdate() override;

	CMovieToTexture* GetMovieData() const;

private:
	//コピー禁止
	MovieFileData(const MovieFileData&) = delete;
	MovieFileData& operator=(const MovieFileData&) = delete;

	CMovieToTexture* m_MovieData;
};
