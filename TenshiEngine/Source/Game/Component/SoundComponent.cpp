#include "SoundComponent.h"
#include "Engine/AssetFile/Sound/SoundFileData.h"
#include "Window/Window.h"

#include "Engine/Inspector.h"

SoundComponent::SoundComponent(){
	mIsLoop = false;
	mAutoPlay = true;
	mVolume = 1.0f;
}
SoundComponent::~SoundComponent(){

}
void SoundComponent::Initialize(){
	Stop();
	mSoundFileSource.Load(mSoundFileSource.m_Hash);
	LoadFile(mSoundFileSource);

}
void SoundComponent::Start(){
}
void SoundComponent::Finish(){
	Stop();
	mSoundFile = NULL;
	mSoundFileSource.Free();
}
void SoundComponent::EngineUpdate(){
}
void SoundComponent::Update(){
	if (mAutoPlay){
		mAutoPlay = false;
		Play();
	}
}
#ifdef _ENGINE_MODE
void SoundComponent::CreateInspector(){
	Inspector ins("Sound", this);
	ins.AddEnableButton(this);
	ins.Add("File", &mSoundFileSource, [&](){LoadFile(mSoundFileSource); });
	ins.Add("AutoPlay", &mAutoPlay, [&](bool f){mAutoPlay = f; });
	ins.Add("Loop", &mIsLoop, [&](bool f){SetLoop(f); });
	ins.AddSlideBar("Volume", 0.0f, 1.0f, &mVolume, [&](float f){SetVolume(f); });
	ins.AddButton("Play", [&](){
		if (IsPlay()){
			Stop();
		}
		else{
			Play();
		}
	});
	ins.Complete();

}
#endif
void SoundComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mSoundFileSource);
	_KEY(mIsLoop);
	_KEY(mVolume);
	_KEY(mAutoPlay);
#undef _KEY
}

void SoundComponent::LoadFile(SoundAsset& asset){
	Stop();
	mSoundFile = NULL;
	mSoundFileSource = asset;
	if (mSoundFileSource.IsLoad()){
		auto sound = mSoundFileSource.Get()->GetSoundFile();
		if (!sound)return;


		mSoundFile = AssetDataTemplate<SoundFileData>::Create(new SoundFileData());
		if (!mSoundFile)return;
		auto file2 = mSoundFile->GetFileData();
		if (!file2)return;
		auto sound2 = file2->GetSoundFile();

		sound->Duplicate(sound2);
	}
}
void SoundComponent::Play(){
	if (IsPlay()){
		Stop();
	}
	if (!mSoundFile)return;
	auto file = mSoundFile->GetFileData();
	if (!file)return;
	auto sound = file->GetSoundFile();
	if (!sound)return;
	sound->Volume(mVolume);
	sound->Play(mIsLoop);
}
void SoundComponent::Stop(){
	if (!mSoundFile)return;
	auto file = mSoundFile->GetFileData();
	if (!file)return;
	auto sound = file->GetSoundFile();
	if (!sound)return;
	sound->Stop();
}
bool SoundComponent::IsPlay(){
	if (!mSoundFile)return false;
	auto file = mSoundFile->GetFileData();
	if (!file)return false;
	auto sound = file->GetSoundFile();
	if (!sound)return false;
	return sound->IsPlay();
}
void SoundComponent::SetLoop(bool flag){
	mIsLoop = flag;
}
void SoundComponent::SetVolume(float vol){
	mVolume = vol;
	if (!mSoundFile)return;
	auto file = mSoundFile->GetFileData();
	if (!file)return;
	auto sound = file->GetSoundFile();
	if (!sound)return;
	sound->Volume(mVolume);
}

void SoundComponent::SetAutoPlay(bool flag){
	mAutoPlay = flag;
}
bool SoundComponent::GetAutoPlay(){
	return mAutoPlay;
}
float SoundComponent::GetVolume(){
	return mVolume;
}