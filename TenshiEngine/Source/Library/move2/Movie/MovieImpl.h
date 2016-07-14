#ifndef _MOVIE_IMPL_H_
#define _MOVIE_IMPL_H_

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#include "VideoRenderer.h"
#include <dshow.h>
#pragma warning(pop)

// ムービー実装クラス
class MovieImpl
{
public:
	
	//コンストラクタ
	MovieImpl();	

	//デストラクタ
	~MovieImpl();	

	// ファイル読み込み
	bool load( const char* fileName, bool sound );

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

	// ボリュームの設定
	void setVolume( int volume );

	// ボリュームの取得
	int getVolume() const;

	// クリーンアップ
	void cleanup();

private:

	// コピー禁止
	MovieImpl( const MovieImpl& other );
	MovieImpl& operator = ( const MovieImpl& other );

private:

	CComPtr<IGraphBuilder>  mGraphBuilder;
	CComPtr<IMediaControl>  mMediaControl;
	CComPtr<IMediaPosition> mMediaPosition;
	CComPtr<IMediaEvent>    mMediaEvent;
	CComPtr<IBasicAudio>    mAudio;
	VideoRenderer*		    mVideoRenderer;
};


#endif

// end of file
