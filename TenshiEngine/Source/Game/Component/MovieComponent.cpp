#include "MovieComponent.h"

#include "Engine/AssetFile/Movie/MovieFileData.h"
#include "Library/Movie/MovieTexture.h"
#include "Window/Window.h"

MovieComponent::MovieComponent(){
	mLoop = false;
	mAutoPlay = false;
	mMovieAssetFile = NULL;
}
MovieComponent::~MovieComponent(){

}

void MovieComponent::Initialize() {
	LoadFile(mFileName);
}
void MovieComponent::Start() {
}
void MovieComponent::EngineUpdate() {

	//if (mPlayFlag == (long long)this){
	//	if (!IsPlay()){
	//		mPlayFlag = NULL;
	//	}
	//}

	if (IsPlay()){

		if (!mMovieAssetFile)return;
		if (!mMovieAssetFile->GetFileData())return;
		if (!mMovieAssetFile->GetFileData()->GetMovieData())return;

		return mMovieAssetFile->GetFileData()->GetMovieData()->TextureRendering();
	}

}
void MovieComponent::Update() {

	if (mAutoPlay){
		mAutoPlay = false;
		Play();
	}
	if (IsPlay()){

		if (!mMovieAssetFile)return;
		if (!mMovieAssetFile->GetFileData())return;
		if (!mMovieAssetFile->GetFileData()->GetMovieData())return;

		return mMovieAssetFile->GetFileData()->GetMovieData()->TextureRendering();
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
	Window::ViewInspector("Movie", this, data);

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


	if (!mMovieAssetFile)return;
	if (!mMovieAssetFile->GetFileData())return;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return;
	auto end = mMovieAssetFile->GetFileData()->GetMovieData()->GetStopTime();
	mMovieAssetFile->GetFileData()->GetMovieData()->SetTime(end);
}
void MovieComponent::Play() {
	if (!mMovieAssetFile)return;
	if (!mMovieAssetFile->GetFileData())return;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return;

	//if (MoviePlayFlag::IsMoviePlay())return;

	//mMovieAssetFile->GetFileData()->GetMovieData()->StopMovie();
	//mMovieAssetFile->GetFileData()->GetMovieData()->PlayMovie(mLoop);

	//mPlayFlag = (long long)this;


	mMovieAssetFile->GetFileData()->GetMovieData()->SetTime(0.0);
	mMovieAssetFile->GetFileData()->GetMovieData()->Play();

}
void MovieComponent::Stop() {

	if (!mMovieAssetFile)return;
	if (!mMovieAssetFile->GetFileData())return;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return;

	if (!IsPlay()){
		return;
	}
	//mPlayFlag = NULL;
	//mMovieAssetFile->GetFileData()->GetMovieData()->StopMovie();

	auto end = mMovieAssetFile->GetFileData()->GetMovieData()->GetStopTime();
	mMovieAssetFile->GetFileData()->GetMovieData()->SetTime(end);

	mMovieAssetFile->GetFileData()->GetMovieData()->Stop();
}
void MovieComponent::SetLoop(bool flag) {
	mLoop = flag;
}

bool MovieComponent::IsPlay() const{

	if (!mMovieAssetFile)return false;
	if (!mMovieAssetFile->GetFileData())return false;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return false;
	//return (bool)mMovieAssetFile->GetFileData()->GetMovieData()->IsPlayMovie();

	auto end = mMovieAssetFile->GetFileData()->GetMovieData()->GetStopTime();
	auto cur = mMovieAssetFile->GetFileData()->GetMovieData()->GetCurrentPosition();
	return (bool)(end > cur);
}


Texture* MovieComponent::GetTexture() const{
	if (!mMovieAssetFile)return NULL;
	if (!mMovieAssetFile->GetFileData())return NULL;
	if (!mMovieAssetFile->GetFileData()->GetMovieData())return NULL;

	return mMovieAssetFile->GetFileData()->GetMovieData()->GetTexture();
}