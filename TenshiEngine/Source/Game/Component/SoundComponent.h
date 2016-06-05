#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"

#include "Engine/AssetDataBase.h"


class ISoundComponent{
public:
	virtual ~ISoundComponent(){}

	virtual void LoadFile(const std::string& filename) = 0;
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual bool IsPlay() = 0;
	virtual void SetLoop(bool flag) = 0;
	virtual void SetVolume(float vol) = 0;
	virtual void SetAutoPlay(bool flag) = 0;
	virtual bool GetAutoPlay() = 0;
	virtual float GetVolume() = 0;
};

class SoundComponent :public ISoundComponent, public Component{
public:
	SoundComponent();
	~SoundComponent();

	void Initialize() override;
	void Start() override;
	void EngineUpdate() override;
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void LoadFile(const std::string& filename)override;
	void Play()override;
	void Stop()override;
	bool IsPlay()override;
	void SetLoop(bool flag)override;
	void SetVolume(float vol)override;
	void SetAutoPlay(bool flag)override;
	bool GetAutoPlay()override;
	float GetVolume()override;
private:
	std::string mFileName;
	bool mIsLoop;
	bool mAutoPlay;
	bool mIsPlay;
	float mVolume;
	SoundAssetDataPtr mSoundFileSource;
	SoundAssetDataPtr mSoundFile;
	
};