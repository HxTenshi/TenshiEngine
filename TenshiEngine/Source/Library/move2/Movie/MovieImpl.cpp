//#define _WIN32_WINNT 0x0403
//#define _WIN32_DCOM
#include "MovieImpl.h"

//コンストラクタ
MovieImpl::MovieImpl() :
	mGraphBuilder( NULL ),
	mMediaControl( NULL ),
	mMediaPosition( NULL ),
	mMediaEvent( NULL ),
	mAudio( NULL ),
	mVideoRenderer( 0 )
{
	// COMの初期化
	CoInitialize( NULL );
}

//デストラクタ
MovieImpl::~MovieImpl()
{
	// クリーンアップ
	cleanup();
	// COMの終了処理
	CoUninitialize();
}

// クリーンアップ
void MovieImpl::cleanup()
{
	// 停止する
	stop();
	// オブジェクトの解放
	if ( !(!mAudio) ) {
		mAudio.Release();
		mAudio = NULL;
	}
	if ( !(!mMediaControl) ) {
		mMediaControl.Release();
		mMediaControl = NULL;
	}
	if ( !(!mMediaEvent) ) {
		mMediaEvent.Release();
		mMediaEvent = NULL;
	}
	if ( !(!mMediaPosition) ) {
		mMediaPosition.Release();
		mMediaPosition = NULL;
	}
	if ( !(!mGraphBuilder) ) {
		mGraphBuilder.Release();
		mGraphBuilder = NULL;
	}
	mVideoRenderer = 0;
}

// ファイル読み込み
bool MovieImpl::load( const char* fileName, bool sound ) {
	// クリーンアップする
	cleanup();
	
	// フィルタグラフの作成
	if ( FAILED( mGraphBuilder.CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC)) ) {
        return false;
	}

    // Create the Texture Renderer object
	HRESULT hr = S_OK;
	mVideoRenderer = new VideoRenderer( NULL, &hr );
    if ( FAILED(hr) ) {
		delete mVideoRenderer;
		mVideoRenderer = 0;
        return false;
    }
    
    // Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
    if ( FAILED( mGraphBuilder->AddFilter( mVideoRenderer, L"VideoRenderer" ) ) ) {
        return false;
    }
   
	// WCHARに変換する
    WCHAR wFileName[MAX_PATH];
    wFileName[MAX_PATH-1] = 0;    
	MultiByteToWideChar(CP_ACP, 0, fileName, -1, wFileName, MAX_PATH);  

    // Add the source filter to the graph.
    CComPtr<IBaseFilter> srcFilter;          // Source Filter
    hr = mGraphBuilder->AddSourceFilter( wFileName, L"SOURCE", &srcFilter );
    
    // If the media file was not found, inform the user.
    if ( hr == VFW_E_NOT_FOUND || FAILED(hr) ) {
        return false;
    }

    CComPtr<IPin> srcFilterPinOut;    // Source Filter Output Pin   
    if ( SUCCEEDED( srcFilter->FindPin( L"Output", &srcFilterPinOut ) ) ) {
		if( sound == false ) {
			// サウンドが無効
			CComPtr<IPin> filterPinIn;      // Texture Renderer Input Pin
			// Find the source's output pin and the renderer's input pin
			if ( FAILED(hr = mVideoRenderer->FindPin( L"In", &filterPinIn )) ) {
				return false;
			}
			// Connect these two filters
			if ( FAILED(hr = mGraphBuilder->Connect( srcFilterPinOut, filterPinIn ))) {
				return false;
			}
		} else {
			// サウンドが有効
			if ( FAILED(hr = mGraphBuilder->Render( srcFilterPinOut ))) {
				return false;
			}
		}
	} else {
		// WMV,MPEG2の場合
		if ( FAILED(mGraphBuilder->RenderFile( wFileName, NULL )) ) {
			return false;
		}
	}

	// ムービー制御インターフェースを取得
	mGraphBuilder.QueryInterface( &mAudio         );
    mGraphBuilder.QueryInterface( &mMediaControl  );
    mGraphBuilder.QueryInterface( &mMediaPosition );
    mGraphBuilder.QueryInterface( &mMediaEvent    );

	// 音量の設定
	if ( sound == false ) {
		// ミュート
		setVolume( -10000 );
	} else {
		// 最大ボリューム
		setVolume( 0 );
	}
    return true;
}

// ムービーの高さを取得
int MovieImpl::getImageHeight() const
{
	if ( mVideoRenderer == 0 ) {
		return 0;
	}
	return mVideoRenderer->getHeight();
}

// ムービーの幅を取得
int MovieImpl::getImageWidth() const
{
	if ( mVideoRenderer == 0 ) {
		return 0;
	}
	return mVideoRenderer->getWidth();
}

// ムービーのイメージをロックする
void* MovieImpl::lockImage()
{
	if ( mVideoRenderer == 0 ) {
		return 0;
	}
	return mVideoRenderer->lock();
}

// ムービーのアンロックする
void MovieImpl::unlockImage()
{
	if ( mVideoRenderer == 0 ) {
		return;
	}
	mVideoRenderer->unlock();
}

// ムービーの再生
void MovieImpl::play()
{
	if ( mMediaControl == NULL ) {
		return;
	}
	mMediaControl->Run();
}

// ムービーの停止
void MovieImpl::stop()
{
	if ( mMediaControl == NULL ) {
		return;
	}
	mMediaControl->Stop();
}

// 再生スピードの変更
void MovieImpl::setSpeed( float time )
{
	if ( mMediaPosition == NULL ) {
		return;
	}
	mMediaPosition->put_Rate( time );
}
	
// 現在の再生位置を指定位置にセット
void MovieImpl::setTime( float time )
{
	if ( mMediaPosition == NULL ) {
		return;
	}
	mMediaPosition->put_CurrentPosition( time );
}

// 終了時間の取得
float MovieImpl::getStopTime() const
{
	if ( mMediaPosition == NULL ) {
		return 0;
	}
	REFTIME time;
	mMediaPosition->get_StopTime( &time );
	return (float)time;
}

// ストリームの時間幅の取得
float MovieImpl::getDuration() const
{
	if ( mMediaPosition == NULL ) {
		return 0;
	}
	REFTIME time;
	mMediaPosition->get_Duration( &time );
	return (float)time;
}

// 現在の再生位置の取得
float MovieImpl::getCurrentPosition() const
{
	if ( mMediaPosition == NULL ) {
		return 0;
	}
	REFTIME time;
	mMediaPosition->get_CurrentPosition( &time );
	return (float)time;
}

// ボリュームの設定
void MovieImpl::setVolume( int volume )
{
	if ( mAudio == NULL ) {
		return;
	}
	mAudio->put_Volume( (long)volume );
}

// ボリュームの取得
int MovieImpl::getVolume() const {
	if ( mAudio == NULL ) {
		return 0;
	}
	long volume;
	mAudio->get_Volume( &volume );
	return (int)volume;
}

// end of file
