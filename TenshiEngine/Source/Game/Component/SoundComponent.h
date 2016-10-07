#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"

#include "Engine/AssetDataBase.h"
#include "Engine/Assets.h"


class ISoundComponent{
public:
	virtual ~ISoundComponent(){}

	virtual void LoadFile(SoundAsset& asset) = 0;
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual bool IsPlay() = 0;
	virtual bool Is3DSound() = 0;
	virtual void Set3DSound(bool flag) = 0;
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
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void LoadFile(SoundAsset& asset)override;
	void Play()override;
	void Stop()override;
	bool IsPlay()override;

	bool Is3DSound()override;
	void Set3DSound(bool flag)override;

	void SetLoop(bool flag)override;
	void SetVolume(float vol)override;
	void SetAutoPlay(bool flag)override;
	bool GetAutoPlay()override;
	float GetVolume()override;
private:
	bool mIsLoop;
	bool mAutoPlay;
	bool mIsPlay;
	float mVolume;
	bool mIs3DSound;
	SoundAsset mSoundFileSource;
	SoundAssetDataPtr mSoundFile;
	
};