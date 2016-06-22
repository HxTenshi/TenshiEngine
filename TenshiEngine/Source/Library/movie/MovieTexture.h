
/*
	! @file movietex.h
	! @bref	ムービーテクスチャクラス
	! @author	Masafumi TAKAHASHI
*/

#ifndef MOVIETEX_H
#define MOVIETEX_H

#include <tchar.h>
typedef TCHAR* PTCHAR;

// DirectX
#include <d3d9.h>
#include <d3dx9.h>

// DirectShow
#include <stdio.h>
#include "DirectShow/baseclasses/streams.h"
#include "DirectShow/common/smartptr.h"
#include <d3d9types.h>
#include <dshow.h>

// DirectShow Library
#ifdef _DEBUG
#pragma comment( lib, "DirectShow/strmbasd.lib" )
#else
#pragma comment( lib, "DirectShow/strmbase.lib" )
#endif

// Define GUID for Texture Renderer
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;


//-----------------------------------------------------------------------------
// テクスチャに動画を書き込む作業を実際に行うためのクラス
// 内部処理に利用するもの
//-----------------------------------------------------------------------------
class TextureRenderer : public CBaseVideoRenderer
{
	SmartPtr<IDirect3DDevice9>	m_pd3dDevice;
	SmartPtr<IDirect3DTexture9>	m_pTexture;

	D3DFORMAT		m_TextureFormat;
 
	BOOL	m_bUseDynamicTextures;	//! @param ダイナミックテクスチャを使うかどうかどうかのフラグ

    LONG	m_lVidWidth;	//! @param ビデオの幅
    LONG	m_lVidHeight;	//! @param ビデオの高さ
    LONG	m_lVidPitch;	//! @param ビデオのピッチ



public:
    TextureRenderer(LPUNKNOWN pUnk,HRESULT *phr);
    ~TextureRenderer();
 
	void SetDevice(IDirect3DDevice9 * pd3dDevice){ m_pd3dDevice = pd3dDevice; };

	HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
    HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
    HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	IDirect3DTexture9 * GetTexture(){ return m_pTexture; };

	void GetVideoDesc(LONG* plVidWidth, LONG* plVidHeight, LONG* plVidPitch)
	{
		*plVidWidth		= m_lVidWidth;
		*plVidHeight	= m_lVidHeight;
		*plVidPitch		= m_lVidPitch;
	};
};



//-----------------------------------------------------------------------------
// 動画テクスチャを取りだす制御を行うクラス
// プログラマは、このクラスを使う
//-----------------------------------------------------------------------------
class MovieTexture
{
private:
	SmartPtr<IDirect3DDevice9>	m_pd3dDevice;   // Our rendering device
	SmartPtr<IDirect3DTexture9>	m_pTexture;     // Our texture

	SmartPtr<IGraphBuilder>		m_pGB;          // GraphBuilder
	SmartPtr<IMediaControl>		m_pMC;          // Media Control
	SmartPtr<IMediaPosition>	m_pMP;          // Media Position
	SmartPtr<IMediaEvent>		m_pME;          // Media Event

	SmartPtr<IBaseFilter>		m_pRenderer;    // our custom renderer
	
	LONG	m_lWidth;			//! @param 幅  
	LONG	m_lHeight;			//! @param 高さ
	LONG	m_lPitch;			//! @param ピッチ

	FLOAT	m_fu, m_fv;			//! @param 元のムービーのサイズ(幅、高さ) / テクスチャのサイズ(幅、高さ)で算出するUV値

	bool	m_loop;



public:
	MovieTexture();			// コンストラクタ
	MovieTexture( IDirect3DDevice9 * pd3dDevice, WCHAR* wFileName, BOOL en_sound=TRUE, BOOL begin_play=TRUE );		// コンストラクタ
	~MovieTexture();		// デストラクタ

private:
	void	CheckMovieStatus( void );			// （内部関数）動画の状態を調べる
	void	CleanupDShow( void );				// （内部関数）破棄処理

public:
	HRESULT	Create( IDirect3DDevice9 * pd3dDevice, WCHAR* wFileName, bool en_sound=true, bool loop=true, bool begin_play=true );	// 初期化関数
	HRESULT	Init( IDirect3DDevice9 * pd3dDevice, WCHAR* wFileName, const BOOL bSound );	// 初期化関数

	IDirect3DTexture9*	GetTexture();			// ムービーテクスチャの取得

	void	Play() { m_pMC->Run(); };			// ムービーの再生
	void	Stop() { m_pMC->Stop(); };			// ムービーの停止

	double	GetStopTime() { REFTIME time; m_pMP->get_StopTime(&time); return time; };	// 終了時間の取得
	double	GetDuration() { REFTIME time; m_pMP->get_Duration(&time); return time; };	// ストリームの時間幅の取得
	double	GetCurrentPosition() { REFTIME time; m_pMP->get_CurrentPosition(&time); return time; }	// 現在の再生位置の取得
	bool	IsLoop( void )	{ return m_loop; };

	void	SetTime( double time ) { m_pMP->put_CurrentPosition(time); };				// 現在の再生位置を指定位置にセット
	void	SetSpeed( double time ) { m_pMP->put_Rate(time); };							// 再生スピードの変更
	void	SetLoop( bool loop )	{ m_loop = loop; };

	void	GetUV(FLOAT* u, FLOAT* v) { *u = m_fu; *v = m_fv; };						// 再生するテクスチャのサイズとムービーのサイズが合わない場合の余剰分をカットするためのUV値取得
	void	GetEvent( long* lEventCode, long* lParam1, long* lParam2, long msTimeout );	// ムービーのイベント取得

	bool	IsEndPos( void )	{ return GetCurrentPosition() >= GetStopTime(); };
};

#endif // MOVIETEX_H