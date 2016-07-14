#include "Movie.h"
#include "MovieImpl.h"

//コンストラクタ
Movie::Movie() :
	mImpl( new MovieImpl() ), mVolume(0.0)
{}

//デストラクタ
Movie::~Movie()
{
	delete mImpl;
}

// 初期化
bool Movie::load( const char* fileName, bool sound )
{
	if (sound) {
		mVolume = 1.0f;	// デフォルトは最大ボリューム
	} else {
		mVolume = 0.0f;	// 音楽の指定がない場合、ミュートとする
	}
	if ( mImpl->load( fileName, sound ) ) {
		setVolume( mVolume );
		return true;
	}
	return false;
}

// ムービーの高さを取得
int Movie::getImageHeight() const
{
	return mImpl->getImageHeight();
}

// ムービーの幅を取得
int Movie::getImageWidth() const
{
	return mImpl->getImageWidth();
}

// ムービーのイメージをロックする
void* Movie::lockImage()
{
	return mImpl->lockImage();
}

// ムービーのアンロックする
void Movie::unlockImage()
{
	mImpl->unlockImage();
}

// ムービーの再生
void Movie::play()
{
	mImpl->play();
}

// ムービーの停止
void Movie::stop()
{
	mImpl->stop();
}

// 再生スピードの変更
void Movie::setSpeed( float time )
{
	mImpl->setSpeed( time );
}
	
// 現在の再生位置を指定位置にセット
void Movie::setTime( float time )
{
	mImpl->setTime( time );
}

// 終了時間の取得
float Movie::getStopTime() const
{
	return mImpl->getStopTime();
}

// ストリームの時間幅の取得
float Movie::getDuration() const
{
	return mImpl->getDuration();
}

// 現在の再生位置の取得
float Movie::getCurrentPosition() const
{
	return mImpl->getCurrentPosition();
}

// ボリュームの設定　( 0.0(ミュート）～ 1.0(最大））
void Movie::setVolume( float volume )
{
	// 0.0 ～ 1.0のボリュームを-10000～0に変換する
	mVolume = max(0.0f, min(1.0f, volume));
	int v = (int)(mVolume * 10000.0f - 10000.0f);
	mImpl->setVolume( v );
}

// ボリュームの取得　( 0.0(ミュート）～ 1.0(最大））
float Movie::getVolume() const
{
	return mVolume;
}

// クリーンアップ
void Movie::cleanup()
{
	mImpl->cleanup();
}

// end of file
