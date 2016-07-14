#ifndef _MOVIE_H_
#define _MOVIE_H_

// ムービー実装クラス
class MovieImpl;

// ムービークラス
class Movie
{
public:
	
	//コンストラクタ
	Movie();	

	//デストラクタ
	~Movie();	

	// ファイルの読み込み
	bool load( const char* fileName, bool sound=true );

	// ムービーの高さを取得
	int getImageHeight() const;

	// ムービーの幅を取得
	int getImageWidth() const;

	// ムービーのイメージをロックする
	void* lockImage();

	// ムービーのアンロックする
	void unlockImage();

	// ムービーの再生
	void play();

	// ムービーの停止
	void stop();

	// 再生スピードの変更
	void setSpeed( float time );
	
	// 現在の再生位置を指定位置にセット
	void setTime( float time );

	// 終了時間の取得
	float getStopTime() const;

	// ストリームの時間幅の取得
	float getDuration() const;

	// 現在の再生位置の取得
	float getCurrentPosition() const;

	// ボリュームの設定　( 0.0(ミュート）～ 1.0(最大））
	void setVolume( float volume );

	// ボリュームの取得　( 0.0(ミュート）～ 1.0(最大））
	float getVolume() const;

	// クリーンアップ
	void cleanup();

private:

	// コピー禁止
	Movie( const Movie& other );
	Movie& operator = ( const Movie& other );

private:

	// ムービー実装クラス
	MovieImpl*	mImpl;

	// ボリューム
	float mVolume;
};

#endif

// end of file
