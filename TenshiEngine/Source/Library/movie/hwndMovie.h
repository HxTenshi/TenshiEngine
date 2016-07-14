//#include <windows.h>
////#include <objbase.h>
////#include <process.h>
//#include <tchar.h>
//
//#include <mfapi.h>
//#include <mfidl.h>
//#include <mferror.h>
//#include <evr.h>
//#include <mbstring.h>
//
//#include "../unknownbase.h"
//
//#pragma comment(lib,"mf.lib")
//#pragma comment(lib,"mfplat.lib")
//#pragma comment(lib,"mfuuid.lib")
//#pragma comment(lib,"strmiids.lib")
//
////ロードされたメディア種別を列挙する
//typedef struct _mediacontenttype{
//	const TCHAR *ext;
//	const WCHAR *type;
//} MEDIACONTENTTYPE;
//
////ステータスコード
//#define MFSSTATUS_NONE				0x00000000
//#define MFSSTATUS_LOAD				0x00000001
//#define MFSSTATUS_PLAY				0x00000002
//#define MFSSTATUS_PAUSE				0x00000004
//#define MFSSTATUS_LOOP				0x00000008
//#define MFSSTATUS_HASAUDIOLINE			0x00000100
//#define MFSSTATUS_HASVIDEOLINE			0x00000200
//
////失敗時のGoto処理
//#define FAILED_BREAK(hr,x)		{ (hr) = (x); if(FAILED(hr)){ break; } }
//
//
//static const MEDIACONTENTTYPE g_avMediaType[] = {
//		{ TEXT("mp4"), L"video/mp4" },
//		{ TEXT("wmv"), L"video/x-ms-wmv" },
//		{ TEXT("avi"), L"video/x-msvideo" },
//		{ NULL, NULL }
//};
//#ifdef _UNICODE
////#define _istlead(c)	0
//#define _isttrail(c)	0
//#else
////#define _istlead(c)		_ismbblead(c)
//#define _isttrail(c)	_ismbbtrail(c)
//#endif //_UNICODE
//
//
//
//#ifndef __mfsession_h__
//#define __mfsession_h__
//
//#define WM_MFSNOTIFY					(WM_USER + 0x1001)
//
//#define MFS_VOLUMEMAX					1000
//#define MFS_VOLUMEMIN					0
//#define MFS_PANLEFT					-(MFS_VOLUMEMAX)
//#define MFS_PANCENTER					MFS_VOLUMEMIN
//#define MFS_PANRIGHT					MFS_VOLUMEMAX
//
//typedef int int32_t;
//typedef __int64 int64_t;
//typedef unsigned int uint32_t;
//typedef unsigned __int64 uint64_t;
//
////メディアセッション管理
//class CMFSession : public CUnknownBase, public IMFAsyncCallback{
//public:
//	CMFSession(HWND hWnd); //コンストラクタ
//	virtual ~CMFSession(); //デストラクタ
//
//	//IUnknownの機能を実装する
//	DECLARE_IUNKNOWN;
//	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv); //インターフェイスの要求
//
//	//IMFAsyncCallbackの実装
//	STDMETHODIMP GetParameters(DWORD *pdwFlags, DWORD *pdwQueue); //内部パラメータを取得する
//	STDMETHODIMP Invoke(IMFAsyncResult *pAsyncResult); //非同期処理が行われたときのコールバック
//
//	virtual HRESULT LoadMovie(const TCHAR *lpFileName); //メディアを読み込む
//	virtual BOOL ReleaseMovie(void); //解放処理
//	virtual BOOL PlayMovie(BOOL bIsLoop = FALSE); //再生
//	virtual BOOL StopMovie(void); //停止
//	virtual BOOL IsPlayMovie(void) const; //再生中かどうか
//	virtual BOOL PauseMovie(void); //一時停止
//	virtual BOOL ForwardMovie(uint32_t nTime); //早送り(ms)
//	virtual BOOL BackwardMovie(uint32_t nTime); //巻き戻し(ms)
//	virtual BOOL SeekMovie(uint32_t nTime); //再生位置の移動(ms)
//	virtual BOOL SeekMovieRel(int64_t llTime); //再生位置の相対移動(100ns)
//	virtual BOOL SeekMovieAbs(uint64_t ullTime); //再生位置の絶対移動(100ns)
//
//	virtual BOOL SetSpeed(double dSpeed); //スピードの変更
//	virtual BOOL SetPan(int nAbsolutePan); //パンの変化を設定する
//	virtual BOOL SetVolume(int nAbsoluteVolume); //音量の設定をする
//
//	virtual uint32_t GetMovieTime(void) const; //メディアの再生時間を取得
//	virtual uint64_t GetMediaTime(void) const; //メディア時間で再生時間を取得
//	virtual BOOL GetMovieSize(int32_t *lpnWidth, int32_t *lpnHeight) const; //メディアの画像のサイズを取得
//	virtual BOOL HasMediaVideo(void) const; //メディアが映像ラインを保持しているかどうか
//	virtual BOOL HasMediaAudio(void) const; //メディアが音声ラインを保持しているかどうか
//
//	virtual BOOL SetPlayWindow(const POINT *lpPlayPos = NULL, const SIZE *lpPlaySize = NULL); //再生ウィンドウの設定
//	virtual BOOL Repaint(void); //再描画を行う
//	virtual BOOL HandleEvent(LPARAM lParam); //イベント処理発行
//
//protected:
//	HRESULT SetByteStreamContentType(const TCHAR *lpFileName, const TCHAR *lpContentType); //ByteStreamにContentTypeを設定する
//	HRESULT CreateSourceNode(IMFPresentationDescriptor *lpPresentDesc, IMFStreamDescriptor *lpStreamDesc, IMFTopologyNode **lplpNode); //ソース部のトポロジーノードを作成する
//	HRESULT CreateOutputNode(IMFStreamDescriptor *lpStreamDesc, IMFTopologyNode **lplpNode); //出力部のトポロジーノードを作成する
//	HRESULT InnerSeekTime(MFTIME nTime); //再生位置を設定する
//	HRESULT InnerSetVolume(void); //ボリュームを設定する
//
//	inline bool IsEnableVideo(void) const; //ビデオ有効テスト
//	inline bool IsEnableAudio(void) const; //サウンド有効テスト
//	inline bool InnerTestStatus(uint32_t nStatus) const; //内部的なステータステスト
//	static inline uint64_t MilliSecondToMediaTime(uint32_t t); //ミリ秒をメディアタイムに変換する
//	static inline uint32_t MediaTimeToMilliSecond(uint64_t t); //メディアタイムをミリ秒に変換する
//
//private:
//	HWND m_hWnd; //ウィンドウハンドル
//	uint32_t m_nStatusCode; //ステータスコード
//	MFTIME m_mtMediaLength; //メディア長(100ns)
//	float m_fAudioVolume; //オーディオボリューム
//	float m_fAudioBalance; //オーディオバランス
//
//	IMFMediaSession *m_lpMediaSession; //メディアセッション
//	IMFByteStream *m_lpByteStream; //バイトストリーム
//	IMFMediaSource *m_lpMediaSource; //メディアソース
//	IMFPresentationClock *m_lpPresentationClock; //プレゼンテーションクロック
//	IMFVideoDisplayControl *m_lpVideoDisplay; //ビデオディスプレイ
//	IMFAudioStreamVolume *m_lpAudioVolume; //オーディオボリューム
//};
//
//#endif //__mfsession_h__
