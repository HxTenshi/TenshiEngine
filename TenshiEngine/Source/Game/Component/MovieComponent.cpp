#include "MovieComponent.h"

#include "Engine/AssetFile/Movie/MovieFileData.h"
#include "Library/Movie/MovieTexture.h"
#include "Window/Window.h"

#include "Engine/Inspector.h"

MovieComponent::MovieComponent(){
	mLoop = false;
	mAutoPlay = false;
}
MovieComponent::~MovieComponent(){

}

void MovieComponent::Initialize() {
	mMovieAsset.Load(mMovieAsset.m_Hash);
	LoadFile(mMovieAsset);
}
void MovieComponent::Start() {
}

#ifdef _ENGINE_MODE
void MovieComponent::EngineUpdate() {

	//if (mPlayFlag == (long long)this){
	//	if (!IsPlay()){
	//		mPlayFlag = NULL;
	//	}
	//}

	if (IsPlay()){
		auto data = mMovieAsset.Get()->GetMovieData();
		return data->TextureRendering();
	}

}
#endif
void MovieComponent::Update() {

	if (mAutoPlay){
		mAutoPlay = false;
		Play();
	}
	if (IsPlay()){
		auto data = mMovieAsset.Get()->GetMovieData();
		return data->TextureRendering();
	}
	
}
void MovieComponent::Finish() {

	Stop();
	mMovieAsset.Free();
}

#ifdef _ENGINE_MODE
void MovieComponent::CreateInspector(){
	Inspector ins("Movie",this);
	ins.AddEnableButton(this);
	ins.Add("File", &mMovieAsset, [&](){LoadFile(mMovieAsset); });
	ins.Add("AutoPlay", &mAutoPlay, [&](bool f){mAutoPlay = f; });
	ins.Add("Loop", &mLoop, [&](bool f){SetLoop(f); });
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
void MovieComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(mMovieAsset);
	_KEY(mLoop);
	_KEY(mAutoPlay);
#undef _KEY
}

void MovieComponent::LoadFile(MovieAsset& asset) {
	mMovieAsset = asset;
	Stop();

	if (!mMovieAsset.IsLoad())return;
	auto data = mMovieAsset.Get()->GetMovieData();
	auto end = data->GetStopTime();
	data->SetTime(end);
}
void MovieComponent::Play() {
	if (!mMovieAsset.IsLoad())return;
	auto data = mMovieAsset.Get()->GetMovieData();

	data->SetTime(0.0);
	data->Play();

}
void MovieComponent::Stop() {

	if (!IsPlay()){
		return;
	}
	auto data = mMovieAsset.Get()->GetMovieData();

	auto end = data->GetStopTime();
	data->SetTime(end);

	data->Stop();
}
void MovieComponent::SetLoop(bool flag) {
	mLoop = flag;
}

bool MovieComponent::IsPlay() const{

	if (!mMovieAsset.IsLoad())return false;
	auto data = mMovieAsset.Get()->GetMovieData();

	auto end = data->GetStopTime();
	auto cur = data->GetCurrentPosition();
	return (bool)(end > cur);
}


Texture* MovieComponent::GetTexture() const{
	if (!mMovieAsset.IsLoad())return NULL;
	auto data = mMovieAsset.Get()->GetMovieData();

	return data->GetTexture();
}