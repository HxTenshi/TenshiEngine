/*
! @file movietex.h
! @bref	ムービーテクスチャクラス
! @author	Masafumi TAKAHASHI
*/

#ifndef MOVIETEX_H
#define MOVIETEX_H

#define _MOVETEX_ENABLE 0
#if _MOVETEX_ENABLE

#include <d3d11.h>

#pragma push_macro("new")
#undef new
#include <atlbase.h>
#include <stdio.h>
#include <Streams.h>
#include <dshow.h>
#pragma pop_macro("new")
#include <vector>


class Texture;

//-----------------------------------------------------------------------------
// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

//! @class CMovieTexture 
//! @brief ビデオをID3D11Texture2Dに書き込む処理を行うクラス
class CMovieTexture : public CBaseVideoRenderer
{
	CComPtr<ID3D11Device>	m_pd3dDevice;
	CComPtr<ID3D11Texture2D>	m_pTexture;

	typedef std::vector<BYTE> RenderBuffer;
	RenderBuffer mRenderBuffer[2];
	int mLockBuffer;
	int mFrontBuffer;
	int mBackBuffer;

	//D3DFORMAT		m_TextureFormat;

public:
	CMovieTexture(LPUNKNOWN pUnk, HRESULT *phr);
	~CMovieTexture();

	VOID SetDevice(ID3D11Device * pd3dDevice){ m_pd3dDevice = pd3dDevice; };

	HRESULT CheckMediaType(const CMediaType *pmt);     // Format acceptable?
	HRESULT SetMediaType(const CMediaType *pmt);       // Video format notification
	HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	//! @fn ID3D11Resource * GetTexture()
	//! @brief ムービーテクスチャの取得
	//! @return ムービーがレンダリングされたテクスチャ
	ID3D11Texture2D * GetTexture(){ return m_pTexture; };

	void TextureRendering();

	//! @fn VOID GetVideoDesc(LONG* plVidWidth, LONG* plVidHeight, LONG* plVidPitch)
	//! @brief ビデオの幅、高さ、ピッチの取得
	//! @param *plVidWidth (out) ビデオの幅
	//! @param *plVidHeigh (out) ビデオの高
	//! @param *plVidPitch (out) ビデオのピッチ
	VOID GetVideoDesc(LONG* plVidWidth, LONG* plVidHeight, LONG* plVidPitch)
	{
		*plVidWidth = m_lVidWidth;
		*plVidHeight = m_lVidHeight;
		*plVidPitch = m_lVidPitch;
	};

	//! @param ダイナミックテクスチャを使うかどうかどうかのフラグ
	BOOL m_bUseDynamicTextures;
	//! @param ビデオの幅
	LONG m_lVidWidth;
	//! @param ビデオの高さ
	LONG m_lVidHeight;
	//! @param ビデオのピッチ
	LONG m_lVidPitch;
};

//! @class CMovieToTexture
//! @brief ムービーテクスチャクラス
class CMovieToTexture
{
	CComPtr<ID3D11Device>       m_pd3dDevice;   // Our rendering device
	CComPtr<ID3D11Texture2D>      m_pTexture;     // Our texture

	CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder
	CComPtr<IMediaControl>  m_pMC;          // Media Control
	CComPtr<IMediaPosition> m_pMP;          // Media Position
	CComPtr<IMediaEvent>    m_pME;          // Media Event
	CComPtr<IBaseFilter>    m_pRenderer;    // our custom renderer
	CMovieTexture* mCMovieTexture;
	Texture *m_Texture;

	//! @param 幅  
	LONG m_lWidth;
	//! @param 高さ
	LONG m_lHeight;
	//! @param ピッチ
	LONG m_lPitch;

	//! @param 元のムービーのサイズ(幅、高さ) / テクスチャのサイズ(幅、高さ)で算出するUV値
	FLOAT m_fu, m_fv;

	void CheckMovieStatus(void);
	void CleanupDShow(void);

public:
	CMovieToTexture();	//コンストラクタ
	~CMovieToTexture();	//デストラクタ

	//! @fn HRESULT InitDShowTextureRenderer(WCHAR* wFileName, const BOOL bSound)
	//! @brief DirectShowからテクスチャへのレンダリングへの初期化
	//! @param *pd3dDevice (in) Direct3Dデバイス
	//! @param *wFileName (in) ムービーファイルのパス
	//! @param bSound (in) サウンド再生フラグ
	//! @return 関数の成否
	HRESULT InitDShowTextureRenderer(ID3D11Device * pd3dDevice, const WCHAR* wFileName, const BOOL bSound);

	//! @fn IDirect3DTexture9 * GetTexture()
	//! @brief ムービーテクスチャの取得
	//! @return ムービーがレンダリングされたテクスチャ
	Texture* GetTexture(){ return m_Texture; };

	void TextureRendering(){
		if (mCMovieTexture)mCMovieTexture->TextureRendering();
	}

	//! @fn VOID Play()
	//! @brief ムービーの再生
	VOID Play(){ m_pMC->Run(); };

	//! @fn VOID Stop()
	//! @brief ムービーの停止
	VOID Stop(){ m_pMC->Stop(); };

	//! @fn VOID SetSpeed(double time)
	//! @brief 再生スピードの変更
	//! @param time (in) 再生スピードの倍率
	VOID SetSpeed(double time){ m_pMP->put_Rate(time); };

	//! @fn double GetStopTime()
	//! @brief 終了時間の取得
	//! @return 終了時間
	double GetStopTime()
	{
		REFTIME time;
		m_pMP->get_StopTime(&time);
		return time;
	};

	//! @fn double GetDuration()
	//! @brief ストリームの時間幅の取得
	//! @return 全ストリーム長
	double GetDuration()
	{
		REFTIME time;
		m_pMP->get_Duration(&time);
		return time;
	}

	//! @fn double GetCurrentPosition()
	//! @brief 現在の再生位置の取得
	//! @return 現在の再生位置
	double GetCurrentPosition()
	{
		REFTIME time;
		m_pMP->get_CurrentPosition(&time);
		return time;
	}

	//! @fn VOID SetTime(double time)
	//! @brief 現在の再生位置を指定位置にセット
	//! @param time (in) セットしたい再生位置
	VOID SetTime(double time){ m_pMP->put_CurrentPosition(time); };

	//! @fn VOID GetUV(FLOAT* u, FLOAT* v)
	//! @brief 再生するテクスチャのサイズとムービーのサイズが合わない場合の余剰分をカットするためのUV値取得
	//! @param *u (out) テクスチャ座標U
	//! @param *v (out) テクスチャ座標V
	VOID GetUV(FLOAT* u, FLOAT* v)
	{
		*u = m_fu;
		*v = m_fv;
	}

	//! @fn VOID GetEvent(long* lEventCode, long* lParam1, long* lParam2, long msTimeout)
	//! @brief ムービーのイベント取得
	//! @param lEventCode (out) イベント コードを受け取る変数へのポインタ
	//! @param lParam1 (out) 第 1 イベント引数を受け取る変数へのポインタ。
	//! @param lParam2 (out) 第 2 イベント引数を受け取る変数へのポインタ。
	//! @param msTimeout (in) タイムアウト時間 (ミリ秒単位)。イベントが到着するまで動作を停止するには、INFINITE を使う。
	VOID GetEvent(long* lEventCode, long* lParam1, long* lParam2, long msTimeout);
};


#else
class Texture;
class CMediaType;
class IMediaSample;
//-----------------------------------------------------------------------------
// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

//! @class CMovieTexture 
//! @brief ビデオをID3D11Texture2Dに書き込む処理を行うクラス
class CMovieTexture
{

public:
	CMovieTexture(LPUNKNOWN pUnk, HRESULT *phr){}
	~CMovieTexture(){}

	VOID SetDevice(ID3D11Device * pd3dDevice){};

	HRESULT CheckMediaType(const CMediaType *pmt){ return E_FAIL; }     // Format acceptable?
	HRESULT SetMediaType(const CMediaType *pmt){ return E_FAIL; }       // Video format notification
	HRESULT DoRenderSample(IMediaSample *pMediaSample){ return E_FAIL; } // New video sample

	//! @fn ID3D11Resource * GetTexture()
	//! @brief ムービーテクスチャの取得
	//! @return ムービーがレンダリングされたテクスチャ
	ID3D11Texture2D * GetTexture(){};

	void TextureRendering();

	//! @fn VOID GetVideoDesc(LONG* plVidWidth, LONG* plVidHeight, LONG* plVidPitch)
	//! @brief ビデオの幅、高さ、ピッチの取得
	//! @param *plVidWidth (out) ビデオの幅
	//! @param *plVidHeigh (out) ビデオの高
	//! @param *plVidPitch (out) ビデオのピッチ
	VOID GetVideoDesc(LONG* plVidWidth, LONG* plVidHeight, LONG* plVidPitch)
	{
	};
};

//! @class CMovieToTexture
//! @brief ムービーテクスチャクラス
class CMovieToTexture
{
	void CheckMovieStatus(void){}
	void CleanupDShow(void){}

public:
	CMovieToTexture(){}	//コンストラクタ
	~CMovieToTexture(){}	//デストラクタ

	//! @fn HRESULT InitDShowTextureRenderer(WCHAR* wFileName, const BOOL bSound)
	//! @brief DirectShowからテクスチャへのレンダリングへの初期化
	//! @param *pd3dDevice (in) Direct3Dデバイス
	//! @param *wFileName (in) ムービーファイルのパス
	//! @param bSound (in) サウンド再生フラグ
	//! @return 関数の成否
	HRESULT InitDShowTextureRenderer(ID3D11Device * pd3dDevice, const WCHAR* wFileName, const BOOL bSound){ return E_FAIL; }

	//! @fn IDirect3DTexture9 * GetTexture()
	//! @brief ムービーテクスチャの取得
	//! @return ムービーがレンダリングされたテクスチャ
	Texture* GetTexture(){ return NULL; };

	void TextureRendering(){}

	//! @fn VOID Play()
	//! @brief ムービーの再生
	VOID Play(){};

	//! @fn VOID Stop()
	//! @brief ムービーの停止
	VOID Stop(){};

	//! @fn VOID SetSpeed(double time)
	//! @brief 再生スピードの変更
	//! @param time (in) 再生スピードの倍率
	VOID SetSpeed(double time){};

	//! @fn double GetStopTime()
	//! @brief 終了時間の取得
	//! @return 終了時間
	double GetStopTime()
	{
		return 0.0;
	};

	//! @fn double GetDuration()
	//! @brief ストリームの時間幅の取得
	//! @return 全ストリーム長
	double GetDuration()
	{
		return 0.0;
	}

	//! @fn double GetCurrentPosition()
	//! @brief 現在の再生位置の取得
	//! @return 現在の再生位置
	double GetCurrentPosition()
	{
		return 0.0;
	}

	//! @fn VOID SetTime(double time)
	//! @brief 現在の再生位置を指定位置にセット
	//! @param time (in) セットしたい再生位置
	VOID SetTime(double time){ };

	//! @fn VOID GetUV(FLOAT* u, FLOAT* v)
	//! @brief 再生するテクスチャのサイズとムービーのサイズが合わない場合の余剰分をカットするためのUV値取得
	//! @param *u (out) テクスチャ座標U
	//! @param *v (out) テクスチャ座標V
	VOID GetUV(FLOAT* u, FLOAT* v)
	{
	}

	//! @fn VOID GetEvent(long* lEventCode, long* lParam1, long* lParam2, long msTimeout)
	//! @brief ムービーのイベント取得
	//! @param lEventCode (out) イベント コードを受け取る変数へのポインタ
	//! @param lParam1 (out) 第 1 イベント引数を受け取る変数へのポインタ。
	//! @param lParam2 (out) 第 2 イベント引数を受け取る変数へのポインタ。
	//! @param msTimeout (in) タイムアウト時間 (ミリ秒単位)。イベントが到着するまで動作を停止するには、INFINITE を使う。
	VOID GetEvent(long* lEventCode, long* lParam1, long* lParam2, long msTimeout){}
};
#endif




#endif // MOVIETEX_H