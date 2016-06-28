#include "MovieFileData.h"

#include "Library/Movie/Movie.h"
#include "Window/Window.h"

//全てのムービーを解放してからshutdown
class ReleaseMovies{
public:
	~ReleaseMovies(){
		{
			MFShutdown();
		}
	}
};
shared_ptr<ReleaseMovies> gMovies = make_shared<ReleaseMovies>();

MovieFileData::MovieFileData()
	:m_MovieData(NULL){

	m_MovieData = new CMFSession(Window::GetGameScreenHWND());

	mMovies = gMovies;

}
MovieFileData::~MovieFileData(){
	if (m_MovieData){
		delete m_MovieData;
		m_MovieData = NULL;
	}
	mMovies = NULL;
}

void MovieFileData::Create(const char* filename){
	m_FileName = filename;

	if (m_MovieData){
		m_MovieData->LoadMovie(m_FileName.c_str());
	}
}
void MovieFileData::FileUpdate(){

	if (m_MovieData){
		m_MovieData->ReleaseMovie();
		m_MovieData->LoadMovie(m_FileName.c_str());
	}
}

CMFSession* MovieFileData::GetMovieData() const{
	return m_MovieData;
}