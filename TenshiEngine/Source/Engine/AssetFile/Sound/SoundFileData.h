#pragma once
#include "../AssetFileData.h"

struct IDirectSoundBuffer8;
struct IDirectSound3DBuffer;
#define _SOUND3D_MODE 1

class SoundFile{
public:

	SoundFile()
		:mBuffer(NULL)
		,m3DBuffer(NULL)
	{}
	~SoundFile(){
		Release();
	}

	void Release();

	void Volume(float vol)const;
	void Play(bool loop,bool resetPlayPoint = true)const;
	void Stop()const;
	bool IsPlay()const;

	void Set3DPosition(float x, float y ,float z) const;
	void Set3DMode(bool flag) const;


	void Duplicate(const SoundFile* out)const;

	bool Create(const char* filename);

private:
	// Waveファイルオープン関数
	bool openWave(TCHAR *filepath, WAVEFORMATEX &waveFormatEx, char** ppData, DWORD &dataSize);

#if _SOUND3D_MODE
	IDirectSound3DBuffer *m3DBuffer;
	IDirectSoundBuffer8 *mBuffer;
#else
	IDirectSoundBuffer8 *mBuffer;
#endif
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