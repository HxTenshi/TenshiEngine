#include "SoundComponent.h"
#include "Engine/AssetFile/Sound/SoundFileData.h"
#include "Window/Window.h"

#include "Engine/Inspector.h"

SoundComponent::SoundComponent(){
	mIsLoop = false;
	mIsPlay = false;
	mAutoPlay = true;
	mVolume = 1.0f;
}
SoundComponent::~SoundComponent(){

}
void SoundComponent::Initialize(){
	LoadFile(mFileName);

}
void SoundComponent::Start(){
}
void SoundComponent::Finish(){
	Stop();
	mSoundFile = NULL;
	mSoundFileSource = NULL;
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
	ins.Add("File", &mFileName, [&](std::string f){LoadFile(f); });
	ins.Add("AutoPlay", &mAutoPlay, [&](bool f){mAutoPlay = f; });
	ins.Add("Loop", &mIsLoop, [&](bool f){SetLoop(f); });
	ins.AddSlideBar("Volume", 0.0f, 1.0f, &mVolume, [&](float f){SetVolume(f); });
	ins.AddButton("Play", [&](){
		if (mIsPlay){
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
	_KEY(mFileName);
	_KEY(mIsLoop);
	_KEY(mVolume);
	_KEY(mAutoPlay);
#undef _KEY
}

void SoundComponent::LoadFile(const std::string& filename){
	Stop();
	mSoundFile = NULL;
	mSoundFileSource = NULL;
	mFileName = filename;
	if (mFileName != ""){
		AssetDataBase::Instance(mFileName.c_str(), mSoundFileSource);
		if (!mSoundFileSource)return;
		auto file = mSoundFileSource->GetFileData();
		if (!file)return;
		auto sound = file->GetSoundFile();
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
	if (mIsPlay)return;
	mIsPlay = true;
	if (!mSoundFile)return;
	auto file = mSoundFile->GetFileData();
	if (!file)return;
	auto sound = file->GetSoundFile();
	if (!sound)return;
	sound->Volume(mVolume);
	sound->Play(mIsLoop);
}
void SoundComponent::Stop(){
	mIsPlay = false;
	if (!mSoundFile)return;
	auto file = mSoundFile->GetFileData();
	if (!file)return;
	auto sound = file->GetSoundFile();
	if (!sound)return;
	sound->Stop();
}
bool SoundComponent::IsPlay(){
	return mIsPlay;
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