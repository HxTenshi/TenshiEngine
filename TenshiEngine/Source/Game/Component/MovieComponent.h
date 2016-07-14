#pragma once

#include "IComponent.h"

#include "MySTL/ptr.h"
#include "Engine/AssetDataBase.h"

//class MoviePlayFlag{
//public:
//	static bool IsMoviePlay();
//protected:
//	static long long mPlayFlag;
//};
class Texture;
class IMovieComponent{
public:
	virtual void LoadFile(const std::string& fileName) = 0;
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void SetLoop(bool flag) = 0;

	virtual bool IsPlay() const = 0;

	virtual Texture *GetTexture() const = 0;
};

class MovieComponent : public IMovieComponent, public Component
//, public MoviePlayFlag
{
public:
	MovieComponent();
	~MovieComponent();

	void Initialize() override;
	void Start() override;
	void EngineUpdate() override;
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif
	void IO_Data(I_ioHelper* io)override;

	void LoadFile(const std::string& fileName) override;
	void Play() override;
	void Stop() override;
	void SetLoop(bool flag) override;

	bool IsPlay() const override;

	Texture *GetTexture() const override;
private:

	std::string mFileName;
	bool mAutoPlay;
	bool mLoop;
	MovieAssetDataPtr mMovieAssetFile;
};