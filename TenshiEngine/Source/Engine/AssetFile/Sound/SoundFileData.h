#pragma once
#include "../AssetFileData.h"

struct IDirectSoundBuffer8;

class SoundFile{
public:

	SoundFile()
		:mBuffer(NULL)
	{}
	~SoundFile(){
		Release();
	}

	void Release();

	void Volume(float vol)const;
	void Play(bool loop,bool resetPlayPoint = true)const;
	void Stop()const;
	bool IsPlay()const;

	void Duplicate(const SoundFile* out)const;

	bool Create(const char* filename);

private:
	// Waveファイルオープン関数
	bool openWave(TCHAR *filepath, WAVEFORMATEX &waveFormatEx, char** ppData, DWORD &dataSize);

	IDirectSoundBuffer8 *mBuffer;
};

class SoundFileData : public AssetFileData{
public:
	SoundFileData();
	~SoundFileData();

	bool Create(const char* filename) override;

	const SoundFile* GetSoundFile() const{
		return &m_SoundData;
	}

private:

	//コピー禁止
	SoundFileData(const SoundFileData&) = delete;
	SoundFileData& operator=(const SoundFileData&) = delete;

	SoundFile m_SoundData;
};