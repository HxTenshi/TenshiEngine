#include "MovieComponent.h"

#include "Engine/AssetFile/Movie/MovieFileData.h"
#include "Library/Movie/Movie.h"

#include "Window/Window.h"

bool MoviePlayFlag::IsMoviePlay(){
	return (bool)(mPlayFlag != NULL);
}
long long MoviePlayFlag::mPlayFlag = NULL;

MovieComponent::MovieComponent(){
	mLoop = false;
	mAutoPlay = false;
	mMovieAssetFile = NULL;
}
MovieComponent::~MovieComponent(){

}

void MovieComponent::Initialize() {
	if (mFileName != ""){
		AssetDataBase::Instance(mFileName.c_str(), mMovieAssetFile);
	}
}
void MovieComponent::Start() {
}
void MovieComponent::EngineUpdate() {

	if (mPlayFlag == (long long)this){
		if (!IsPlay()){
			mPlayFlag = NULL;
		}
	}

}
void MovieComponent::Update() {

	if (mAutoPlay){
		mAutoPlay = false;
		Play();
	}

	if (mPlayFlag == (long long)this){
		if (!IsPlay()){
			mPlayFlag = NULL;
		}
	}

}
void MovieComponent::Finish() {

	Stop();
	mMovieAssetFile = NULL;
}

#ifdef _ENGINE_MODE
void MovieComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("File", &mFileName, [&](std::string f){LoadFile(f); }), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("AutoPlay", &mAutoPlay, [&](bool f){mAutoPlay = f; }), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("Loop", &mLoop, [&](bool f){SetLoop(f); }), data);
	Window::AddInspector(new InspectorButtonDataSet("Play", [&](){
		if (IsPlay()){
			Stop();
		}
		else{
			Play();
		}
	}), data);
	Window::ViewInspector("Sound", this, data);

}
#endif
void MovieComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mFileName);
	_KEY(mLoop);
	_KEY(mAutoPlay);
#undef _KEY
}

void MovieComponent::LoadFile(const std::string& fileName) {
	mFileName = fileName;
	Stop();
	AssetDataBase::Instance(mFileName.c_str(), mMovieAssetFile);
}
void MovieComponent::Play() {
	if (!mMovieAssetFile)return;
	if (!mMovieAssetFile->GetFileData())return;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return;

	if (MoviePlayFlag::IsMoviePlay())return;

	mMovieAssetFile->GetFileData()->GetMovieData()->StopMovie();
	mMovieAssetFile->GetFileData()->GetMovieData()->PlayMovie(mLoop);

	mPlayFlag = (long long)this;

}
void MovieComponent::Stop() {

	if (!mMovieAssetFile)return;
	if (!mMovieAssetFile->GetFileData())return;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return;

	if (!IsPlay()){
		return;
	}
	mPlayFlag = NULL;
	mMovieAssetFile->GetFileData()->GetMovieData()->StopMovie();
}
void MovieComponent::SetLoop(bool flag) {
	mLoop = flag;
}

bool MovieComponent::IsPlay() const{

	if (!mMovieAssetFile)return false;
	if (!mMovieAssetFile->GetFileData())return false;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return false;
	return (bool)mMovieAssetFile->GetFileData()->GetMovieData()->IsPlayMovie();
}