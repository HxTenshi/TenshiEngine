#include "VideoRenderer.h"
#include <cassert>


// ライブラリの設定
#ifdef	NDEBUG
# ifdef _MT
#  ifdef _DLL
#   pragma comment( lib, "lib/DirectShow/lib/strmbase_md.lib" )
#  else
#   pragma comment( lib, "lib/DirectShow/lib/strmbase_mt.lib" )
#  endif
# else
#  pragma comment( lib, "lib/DirectShow/lib/strmbase_ml.lib" )
# endif
#else
# ifdef _MT
#  ifdef _DLL
#   pragma comment( lib, "lib/DirectShow/lib/strmbase_mdd.lib" )
#  else
#   pragma comment( lib, "lib/DirectShow/lib/strmbase_mtd.lib" )
#  endif
# else
#  pragma comment( lib, "lib/DirectShow/lib/strmbase_mld.lib" )
# endif
#endif
///#pragma comment( lib, "lib/DirectShow/lib/strmiids.lib" )
#pragma comment(lib,"strmiids.lib")

// アンロックフラグ
const static int UNLOCK_IMAGE = -1;

// コンストラクタ
VideoRenderer::VideoRenderer( LPUNKNOWN pUnk, HRESULT *phr ) :
	CBaseVideoRenderer( __uuidof(CLSID_MyVideoRenderer), NAME("MyVideo Renderer"), pUnk, phr ),
	mFrontBuffer(0),
	mBackBuffer(1),
	mLockBuffer(UNLOCK_IMAGE),
    mVideoWidth(0),
	mVideoHeight(0),
	mVideoPitch(0)
{
    ASSERT( phr!=0 );
	if (phr) {
        *phr = S_OK;
	}
}

// デストラクタ
VideoRenderer::~VideoRenderer()
{}

// 幅の取得
int VideoRenderer::getWidth() const
{
	return mVideoWidth;
}

// 高さの取得
int VideoRenderer::getHeight() const
{
	return mVideoHeight;
}

// イメージのロック
void* VideoRenderer::lock()
{
	// レンダリング済みのバッファをロックする
	mLockBuffer = mFrontBuffer;
	return (void*)&mBuffer[mLockBuffer][0];
}

// イメージのアンロック
void VideoRenderer::unlock()
{
	mLockBuffer = UNLOCK_IMAGE;
}

// メディアタイプを調べる
HRESULT VideoRenderer::CheckMediaType( const CMediaType* pmt )
{
    CheckPointer( pmt, E_POINTER );

    // ビデオタイプのみ対応する
    if( *pmt->FormatType() != FORMAT_VideoInfo ) {
		return E_INVALIDARG;
    }
	
    // RGB24のフォーマットのみ対応する
    if( IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video )  &&
        IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24 ) ) {
        return S_OK;
    }
    
    return E_FAIL;
}

// メディアタイプを設定する
HRESULT VideoRenderer::SetMediaType( const CMediaType* pmt )
{
    CheckPointer( pmt, E_POINTER );

	// ビデオイメージのフォーマットを取得
    VIDEOINFO* pviBmp = (VIDEOINFO*)pmt->Format();
    mVideoWidth  = pviBmp->bmiHeader.biWidth;
    mVideoHeight = abs(pviBmp->bmiHeader.biHeight);
    mVideoPitch  = (mVideoWidth * 3 + 3) & ~(3);

	// ビデオイメージのバイト数を計算
	int videoSize = mVideoWidth * mVideoHeight * 3;

	// ビデオイメージの初期化
	for ( int i = 0; i < 2; ++i ) {
		mBuffer[i].resize( videoSize );
		std::fill( mBuffer[i].begin(), mBuffer[i].end(), 0 );
	}

    return S_OK;
}

// レンダリングを行う
HRESULT VideoRenderer::DoRenderSample( IMediaSample* pMediaSample )
{
	// 転送元のバッファのポインタを取得
	BYTE* bmpImage = 0;
    pMediaSample->GetPointer( &bmpImage );

	// ロックをしていないバッファを選択する
	if ( mLockBuffer == mBackBuffer ) {
		std::swap( mFrontBuffer, mBackBuffer );
	}

	// 転送先のバッファのポインタを取得
	unsigned char* buffer = &mBuffer[mBackBuffer][0];

	// ビットマップイメージをコピーする
	for ( int y = mVideoHeight-1; y >= 0; --y ) {
		BYTE* bmp = bmpImage + mVideoPitch * y;
		for ( int x = 0; x < mVideoWidth; ++x ) {
			buffer[0] = bmp[2];
			buffer[1] = bmp[1];
			buffer[2] = bmp[0];
			buffer += 3;
			bmp += 3;
		}
	}

	// バッファをスワップする
	std::swap( mFrontBuffer, mBackBuffer );

	return S_OK;
}

// end of file
