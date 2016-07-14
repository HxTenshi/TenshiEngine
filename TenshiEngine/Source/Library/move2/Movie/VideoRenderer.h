#ifndef _VIDEO_RENDERER_H_
#define _VIDEO_RENDERER_H_

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#include <Streams.h>
#include <vector>
#pragma warning(pop)

struct __declspec(uuid("{AD040BD2-C8BE-467b-A478-1DECB14CD938}")) CLSID_MyVideoRenderer;

// ビデオレンダラクラス
class VideoRenderer : public CBaseVideoRenderer
{
public:

	// コンストラクタ
    VideoRenderer( LPUNKNOWN pUnk, HRESULT *phr );

	// デストラクタ
	~VideoRenderer();

	// 幅の取得
	int getWidth() const;

	// 高さの取得
	int getHeight() const;

	// イメージのロック
	void* lock();

	// イメージのアンロック
	void unlock();

private:

	// メディアタイプを調べる
    HRESULT CheckMediaType( const CMediaType* pmt );

	// メディアタイプを設定する
	HRESULT SetMediaType( const CMediaType* pmt );

	// レンダリングを行う
	HRESULT DoRenderSample( IMediaSample* pMediaSample );

private:

	// イメージデータ型
	typedef std::vector<unsigned char> ImageBuffer;

	// イメージバッファ
	ImageBuffer	mBuffer[2];

	// レンダリング済みのバッファ
	int		mFrontBuffer;

	// レンダリング中のバッファ
	int		mBackBuffer;

	// ロック中のバッファ
	int		mLockBuffer;

	// 幅
    int		mVideoWidth;

	// 高さ
	int		mVideoHeight;

	// ピッチ
	int		mVideoPitch;
};

#endif

// end of file


