#include "MovieFileData.h"

//#include "Library/Movie/hwndMovie.h"
//#include "Window/Window.h"
//
////全てのムービーを解放してからshutdown
//class ReleaseMovies{
//public:
//	~ReleaseMovies(){
//		{
//			//MFShutdown();
//		}
//	}
//};
//shared_ptr<ReleaseMovies> gMovies = make_shared<ReleaseMovies>();
//
//MovieFileData::MovieFileData()
//	:m_MovieData(NULL){
//
//	//m_MovieData = new CMFSession(Window::GetGameScreenHWND());
//
//	mMovies = gMovies;
//
//}
//MovieFileData::~MovieFileData(){
//	if (m_MovieData){
//		delete m_MovieData;
//		m_MovieData = NULL;
//	}
//	mMovies = NULL;
//}
//
//void MovieFileData::Create(const char* filename){
//	m_FileName = filename;
//
//	if (m_MovieData){
//	//	m_MovieData->LoadMovie(m_FileName.c_str());
//	}
//}
//void MovieFileData::FileUpdate(){
//
//	//if (m_MovieData){
//	//	m_MovieData->ReleaseMovie();
//	//	m_MovieData->LoadMovie(m_FileName.c_str());
//	//}
//}
//
//CMFSession* MovieFileData::GetMovieData() const{
//	return m_MovieData;
//}


#include "MovieFileData.h"

#include "Library/Movie/MovieTexture.h"
#include "Window/Window.h"

#include <codecvt> 

MovieFileData::MovieFileData()
	:m_MovieData(NULL){

	m_MovieData = new CMovieToTexture();

}
MovieFileData::~MovieFileData(){
	if (m_MovieData){
		delete m_MovieData;
		m_MovieData = NULL;
	}
}

bool MovieFileData::Create(const char* filename){
	m_FileName = filename;

	if (m_MovieData){
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
		std::wstring wsmessage = cv.from_bytes(m_FileName);
		if (FAILED(m_MovieData->InitDShowTextureRenderer(Device::mpd3dDevice, wsmessage.c_str(), true))){
			return false;
		}
	}
	return true;
}
bool MovieFileData::FileUpdate(){

	return Create(m_FileName.c_str());
}

CMovieToTexture* MovieFileData::GetMovieData() const{
	return m_MovieData;
}