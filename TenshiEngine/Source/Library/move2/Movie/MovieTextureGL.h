#ifndef _MOVIE_TEXTURE_GL_H_
#define _MOVIE_TEXTURE_GL_H_

#include "Movie.h"

// ムービーテクスチャクラス
class MovieTextureGL
{
public:

	// コンストラクタ
	MovieTextureGL();

	// デストラクタ
	~MovieTextureGL();

	// ムービーの高さを取得
	int getHeight() const;

	// ムービーの幅を取得
	int getWidth() const;

	// ムービーファイルの読み込み
	bool load( const char* fileName );

	// ムービーの更新
	void update();

	// ムービーの描画
	void draw();

	// ムービーの再生
	void play();

	// ムービーの停止
	void stop();
	
	// 再生時間の設定
	void setTime( float time );

	// 再生時間の取得
	float getTime() const;

	// 終了時間の取得
	float getEndTime() const;

	// ボリュームの設定
	void setVolume( float volume );

	// ボリュームの取得
	float getVolume() const;

	// クリーンアップ
	void cleanup();

	// テクスチャを返す
	unsigned int getTexture();

private:

	// テクスチャの生成
	void genTexture();

	// テクスチャの更新
	void updateTexture();	

	// テクスチャの描画
	void drawTexture();	

	// テクスチャの削除
	void deleteTexture();	

private:

	// ムービークラス
	Movie			mMovie;

	// ムービー用のテクスチャ
	unsigned int	mTexture;
	
	// ムービー用のテクスチャの幅
	unsigned int	mTextureWidth;

	// ムービー用のテクスチャの高さ
	unsigned int	mTextureHeight;
};

#endif

// end of file
