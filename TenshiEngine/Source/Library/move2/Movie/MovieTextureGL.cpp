#include "MovieTextureGL.h"
#include <windows.h>

static int power2( int num );

// コンストラクタ
MovieTextureGL::MovieTextureGL() :
	mTexture( 0 ),
	mTextureWidth( 0 ),
	mTextureHeight( 0 )
{}

// デストラクタ
MovieTextureGL::~MovieTextureGL()
{
	cleanup();
}

// ムービーの高さを取得
int MovieTextureGL::getHeight() const
{
	return mMovie.getImageHeight();
}

// ムービーの幅を取得
int MovieTextureGL::getWidth() const
{
	return mMovie.getImageWidth();
}

// ムービーファイルの読み込み
bool MovieTextureGL::load( const char* fileName )
{
	// ムービーファイルの読み込み
	if ( mMovie.load( fileName ) == false ) {
		return false;
	}

	// ムービー用のテクスチャを作成する
	genTexture();

	return true;
}

// ムービーの更新
void MovieTextureGL::update()
{
	updateTexture();
}

// ムービーの描画
void MovieTextureGL::draw()
{
	drawTexture();
}

// ムービーの再生
void MovieTextureGL::play()
{
	mMovie.play();
}

// ムービーの停止
void MovieTextureGL::stop()
{
	mMovie.stop();
}

// 再生時間の設定
void MovieTextureGL::setTime( float time )
{
	mMovie.setTime( time );
}

// 再生時間の取得
float MovieTextureGL::getTime() const
{
	return mMovie.getCurrentPosition();
}

// 終了時間の取得
float MovieTextureGL::getEndTime() const
{
	return mMovie.getStopTime();
}

// ボリュームの設定
void MovieTextureGL::setVolume( float volume )
{
	mMovie.setVolume( volume );
}

// ボリュームの取得
float MovieTextureGL::getVolume() const
{
	return mMovie.getVolume();
}

// クリーンアップ
void MovieTextureGL::cleanup()
{
	// ムービーをクリーンアップ
	mMovie.cleanup();

	// ムービー用のテクスチャを削除
	deleteTexture();
}

// ムービー用のテクスチャを取得
unsigned int MovieTextureGL::getTexture()
{
	return mTexture;
}

// ムービー用のテクスチャを作成．
void MovieTextureGL::genTexture()
{
	//// ムービー用のテクスチャを削除
	//deleteTexture();
	//
	//glEnable( GL_TEXTURE_2D );
	//
	//// ムービー用のテクスチャを作成
	//glGenTextures( 1, &mTexture );
	//glBindTexture( GL_TEXTURE_2D, mTexture );
	//
	//// テクスチャの幅と高さを計算する
	//mTextureWidth  = power2( mMovie.getImageWidth()  );
	//mTextureHeight = power2( mMovie.getImageHeight() );
	//
	//// テクスチャイメージを設定
	//glTexImage2D(
	//	GL_TEXTURE_2D,
	//	0, GL_RGB, mTextureWidth, mTextureHeight,
	//	0, GL_RGB, GL_UNSIGNED_BYTE , 0
	//);
	//
	//// テクスチャフィルタの設定
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	//
	//glBindTexture( GL_TEXTURE_2D , 0 );
}

#include "../../../Device/DirectX11Device.h"
// ムービー用のテクスチャを更新．
void MovieTextureGL::updateTexture()
{
	//if ( mTexture == 0  ) {
	//	return;
	//}
	//
	//glBindTexture( GL_TEXTURE_2D , mTexture );
	//
	//// イメージを更新する
	void* movieImage = mMovie.lockImage();
	//glTexSubImage2D(
	//	GL_TEXTURE_2D,
	//	0,
	//	0, 0,
	//	mMovie.getImageWidth(), mMovie.getImageHeight(),
	//	GL_RGB,
	//	GL_UNSIGNED_BYTE,
	//	movieImage
	//);
	//mMovie.unlockImage();
	//
	//glBindTexture( GL_TEXTURE_2D , 0 );
}

// ムービー用のテクスチャを描画．
void MovieTextureGL::drawTexture()
{
	//if ( mTexture == 0  ) {
	//	return;
	//}
	//
	//// テクスチャをバインド
	//glEnable( GL_TEXTURE_2D );
	//glBindTexture( GL_TEXTURE_2D , mTexture );
	//
	//// ムービーを貼り付けた四角ポリゴンを描画
	//float right  = (float)mMovie.getImageWidth()  / mTextureWidth;
	//float bottom = (float)mMovie.getImageHeight() / mTextureHeight;
	//glBegin(GL_QUADS);
	//	glTexCoord2f(0,0);
	//	glVertex2f(-1.0f,1.0f);
	//	glTexCoord2f(0,bottom);
	//	glVertex2f(-1.0f,-1.0f);
	//	glTexCoord2f(right, bottom);
	//	glVertex2f(1.0f,-1.0f);
	//	glTexCoord2f(right,0);
	//	glVertex2f(1.0f,1.0f);
	//glEnd();
	//
	//glBindTexture( GL_TEXTURE_2D , 0 );
}

// ムービー用のテクスチャを削除
void MovieTextureGL::deleteTexture()
{
	//if ( mTexture != 0 ) {
	//	glDeleteTextures( 1, &mTexture );
	//	mTexture = 0;
	//}
}

// 最も２の累乗に近い数字を求める
static int power2( int num )
{
	int result = 1;
	while ( result < num ) {
		result <<= 1;
	}
	return result;
}

// end of file
