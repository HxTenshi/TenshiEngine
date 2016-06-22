/*
	! @file movietex.cpp
	! @bref	ムービーテクスチャクラス
	! @author	Masafumi TAKAHASHI
*/

#include "MovieTexture.h"

#pragma warning( disable: 4995 )
#pragma warning( disable: 4996 )

//グローバル
TCHAR g_achCopy[]     = TEXT("Bitwise copy of the sample");
TCHAR g_achOffscr[]   = TEXT("Using offscreen surfaces and StretchCopy()");
TCHAR g_achDynTextr[] = TEXT("Using Dynamic Textures");
TCHAR* g_pachRenderMethod = NULL;


//コンストラクタ
TextureRenderer::TextureRenderer( LPUNKNOWN pUnk, HRESULT *phr )
                                  : CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), 
                                    NAME("Texture Renderer"), pUnk, phr),
                                    m_bUseDynamicTextures(FALSE)
{
    // Store and AddRef the texture for our use.
    ASSERT(phr);
    if (phr)
        *phr = S_OK;
}

//デストラクタ
TextureRenderer::~TextureRenderer()
{
}

//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT TextureRenderer::CheckMediaType(const CMediaType *pmt)
{
    HRESULT   hr = E_FAIL;
    VIDEOINFO *pvi=0;
    
    CheckPointer(pmt,E_POINTER);

    // Reject the connection if this is not a video type
    if( *pmt->FormatType() != FORMAT_VideoInfo ) {
        return E_INVALIDARG;
    }
    
    // Only accept RGB24 video
    pvi = (VIDEOINFO *)pmt->Format();

    if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
       IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24))
    {
        hr = S_OK;
    }
    
    return hr;
}

//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made. 
//-----------------------------------------------------------------------------
HRESULT TextureRenderer::SetMediaType(const CMediaType *pmt)
{
    HRESULT hr;

    UINT uintWidth = 2;
    UINT uintHeight = 2;

    // Retrive the size of this media type
    D3DCAPS9 caps;
    VIDEOINFO *pviBmp;                      // Bitmap info header
    pviBmp = (VIDEOINFO *)pmt->Format();

    m_lVidWidth  = pviBmp->bmiHeader.biWidth;
    m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
    m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

    // here let's check if we can use dynamic textures
    ZeroMemory( &caps, sizeof(D3DCAPS9));
    hr = m_pd3dDevice->GetDeviceCaps( &caps );
    if( caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES )
    {
        m_bUseDynamicTextures = TRUE;
    }

    if( caps.TextureCaps & D3DPTEXTURECAPS_POW2 
		&& caps.TextureCaps & !D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
    {
        while( (LONG)uintWidth < m_lVidWidth )
        {
            uintWidth = uintWidth << 1;
        }
        while( (LONG)uintHeight < m_lVidHeight )
        {
            uintHeight = uintHeight << 1;
        }
    }
    else
    {
        uintWidth = m_lVidWidth;
        uintHeight = m_lVidHeight;
		m_bUseDynamicTextures = FALSE;
    }

    // Create the texture that maps to this media type
    hr = E_UNEXPECTED;
    if( m_bUseDynamicTextures )
    {
        hr = m_pd3dDevice->CreateTexture(uintWidth, uintHeight, 1, 0, 
                                         D3DFMT_X8R8G8B8,D3DPOOL_DEFAULT,
                                         &m_pTexture, NULL);
        g_pachRenderMethod = g_achDynTextr;
        if( FAILED(hr))
        {
            m_bUseDynamicTextures = FALSE;
        }
    }
    if( FALSE == m_bUseDynamicTextures )
    {
        hr = m_pd3dDevice->CreateTexture(uintWidth, uintHeight, 1, 0, 
                                         D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,
                                         &m_pTexture, NULL);
        g_pachRenderMethod = g_achCopy;
    }
    if( FAILED(hr))
    {
        Msg(TEXT("Could not create the D3DX texture!  hr=0x%x"), hr);
        return hr;
    }

    // CreateTexture can silently change the parameters on us
    D3DSURFACE_DESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));

    if ( FAILED( hr = m_pTexture->GetLevelDesc( 0, &ddsd ) ) ) {
        Msg(TEXT("Could not get level Description of D3DX texture! hr = 0x%x"), hr);
        return hr;
    }


    SmartPtr<IDirect3DSurface9> pSurf; 

    if (SUCCEEDED(hr = m_pTexture->GetSurfaceLevel(0, &pSurf)))
        pSurf->GetDesc(&ddsd);

    // Save format info
    m_TextureFormat = ddsd.Format;

    if (m_TextureFormat != D3DFMT_X8R8G8B8 &&
        m_TextureFormat != D3DFMT_A1R5G5B5) {
        Msg(TEXT("Texture is format we can't handle! Format = 0x%x"), m_TextureFormat);
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT TextureRenderer::DoRenderSample( IMediaSample * pSample )
{
    BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer
    LONG  lTxtPitch;                // Pitch of bitmap, texture

    BYTE  * pbS = NULL;
    DWORD * pdwS = NULL;
    DWORD * pdwD = NULL;
    UINT row, col, dwordWidth;
    
    CheckPointer(pSample,E_POINTER);
    CheckPointer(m_pTexture,E_UNEXPECTED);

    // Get the video bitmap buffer
    pSample->GetPointer( &pBmpBuffer );

    // Lock the Texture
    D3DLOCKED_RECT d3dlr;
    if( m_bUseDynamicTextures )
    {
        if( FAILED(m_pTexture->LockRect(0, &d3dlr, 0, D3DLOCK_DISCARD)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pTexture->LockRect(0, &d3dlr, 0, 0)))
            return E_FAIL;
    }
    // Get the texture buffer & pitch
    pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
    lTxtPitch = d3dlr.Pitch;
    
    
    // Copy the bits
	pTxtBuffer	+= lTxtPitch*(m_lVidHeight-1);

    if (m_TextureFormat == D3DFMT_X8R8G8B8) 
    {
        // Instead of copying data bytewise, we use DWORD alignment here.
        // We also unroll loop by copying 4 pixels at once.
        //
        // original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
        //
        // aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
        //
        // We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
        // below, bitwise operations do exactly this.

        dwordWidth = m_lVidWidth / 4; // aligned width of the row, in DWORDS
                                      // (pixel by 3 bytes over sizeof(DWORD))

        for( row = 0; row< (UINT)m_lVidHeight; row++)
        {
            pdwS = ( DWORD*)pBmpBuffer;
            pdwD = ( DWORD*)pTxtBuffer;

            for( col = 0; col < dwordWidth; col ++ )
            {
                pdwD[0] =  pdwS[0] | 0xFF000000;
                pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
                pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
                pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
                pdwD +=4;
                pdwS +=3;
            }

            // we might have remaining (misaligned) bytes here
            pbS = (BYTE*) pdwS;
            for( col = 0; col < (UINT)m_lVidWidth % 4; col++)
            {
                *pdwD = 0xFF000000     |
                        (pbS[2] << 16) |
                        (pbS[1] <<  8) |
                        (pbS[0]);
                pdwD++;
                pbS += 3;           
            }

            pBmpBuffer  += m_lVidPitch;
            pTxtBuffer	-= lTxtPitch;
        }// for rows
    }

    if (m_TextureFormat == D3DFMT_A1R5G5B5) 
    {
        for(int y = 0; y < m_lVidHeight; y++ ) 
        {
            BYTE *pBmpBufferOld = pBmpBuffer;
            BYTE *pTxtBufferOld = pTxtBuffer;   

            for (int x = 0; x < m_lVidWidth; x++) 
            {
                *(WORD *)pTxtBuffer = (WORD)
                    (0x8000 +
                    ((pBmpBuffer[2] & 0xF8) << 7) +
                    ((pBmpBuffer[1] & 0xF8) << 2) +
                    (pBmpBuffer[0] >> 3));

                pTxtBuffer += 2;
                pBmpBuffer += 3;
            }

            pBmpBuffer = pBmpBufferOld + m_lVidPitch;
            pTxtBuffer = pTxtBufferOld + lTxtPitch;
        }
    }

    // Unlock the Texture
    if (FAILED(m_pTexture->UnlockRect(0)))
        return E_FAIL;
    
    return S_OK;
}



// コンストラクタ
MovieTexture::MovieTexture()
{
	m_pGB = NULL;
	m_pMC = NULL;
	m_pMP = NULL;
	m_pME = NULL;
	m_pRenderer = NULL;

	m_pTexture = NULL;

	m_fu = m_fv = 1.0f;

	m_loop	= false;
}

MovieTexture::MovieTexture( IDirect3DDevice9 *pd3dDevice, WCHAR *wFileName, BOOL en_sound, BOOL begin_play )
	: m_pGB( NULL )
	, m_pMC( NULL )
	, m_pMP( NULL )
	, m_pME( NULL )
	, m_pRenderer( NULL )
	, m_pTexture( NULL )
	, m_fu( 1.0f )
	, m_fv( 1.0f )
{
	if( pd3dDevice && wFileName )
		Init( pd3dDevice, wFileName, en_sound );

	if( begin_play )
		Play();
}



// デストラクタ
MovieTexture::~MovieTexture()
{
	CleanupDShow();
}

IDirect3DTexture9*	MovieTexture::GetTexture()
{ 
	if( IsLoop() && IsEndPos() )
	{
		SetTime( 0.0 );
	}

	return m_pTexture;	// ムービーテクスチャの取得
}

HRESULT	MovieTexture::Create( IDirect3DDevice9 * pd3dDevice, WCHAR* wFileName, bool en_sound, bool loop, bool begin_play )
{
	if( pd3dDevice && wFileName )
		Init( pd3dDevice, wFileName, en_sound?TRUE:FALSE );

	m_loop	= loop;

	if( begin_play )
		Play();

	return	S_OK;
}

//! @fn HRESULT Init(WCHAR* wFileName, const BOOL bSound)
//! @brief DirectShowからテクスチャへのレンダリングへの初期化
//! @param *pd3dDevice (in) Direct3Dデバイス
//! @param *wFileName (in) ムービーファイルのパス
//! @param bSound (in) サウンド再生フラグ
//! @return 関数の成否
HRESULT MovieTexture::Init(IDirect3DDevice9 * pd3dDevice, WCHAR* wFileName, const BOOL bSound)
{
    HRESULT hr = S_OK;
    SmartPtr<IBaseFilter>    pFSrc;          // Source Filter
    SmartPtr<IPin>           pFSrcPinOut;    // Source Filter Output Pin   
    TextureRenderer        *pCTR=0;        // DirectShow Texture renderer

	m_pd3dDevice = pd3dDevice;
    
    // フィルタグラフの作成
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGB);
    if (FAILED(hr))	return E_FAIL;

#ifdef REGISTER_FILTERGRAPH
    // Register the graph in the Running Object Table (for debug purposes)
    AddToROT(m_pGB);
#endif
    
    // Create the Texture Renderer object
    pCTR = new TextureRenderer (NULL, &hr);
	pCTR->SetDevice(m_pd3dDevice);
    if (FAILED(hr) || !pCTR)
    {
        Msg(TEXT("Could not create texture renderer object!  hr=0x%x"), hr);
        return E_FAIL;
    }
    
    // Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
    m_pRenderer = pCTR;
    if (FAILED(hr = m_pGB->AddFilter(m_pRenderer, L"TextureRenderer")))
    {
        Msg(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);
        return hr;
    }
    
    // Add the source filter to the graph.
    hr = m_pGB->AddSourceFilter (wFileName, L"SOURCE", &pFSrc);
    
    // If the media file was not found, inform the user.
    if (hr == VFW_E_NOT_FOUND)
    {
        Msg(TEXT("Could not add source filter to graph!  (hr==VFW_E_NOT_FOUND)\r\n\r\n")
            TEXT("This sample reads a media file from the DirectX SDK's media path.\r\n")
            TEXT("Please install the DirectX 9 SDK on this machine."));
        return hr;
    }
    else if(FAILED(hr))
    {
        Msg(TEXT("Could not add source filter to graph!  hr=0x%x"), hr);
        return hr;
    }

    if (SUCCEEDED(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut)))
    {
		//AVIとかMPEG1とかVFWでいけるやつ

		//オーディオトラックを再生しない場合　NO_AUDIO_RENDERER　を定義
		if(!bSound)
		{

			// If no audio component is desired, directly connect the two video pins
			// instead of allowing the Filter Graph Manager to render all pins.

			SmartPtr<IPin> pFTRPinIn;      // Texture Renderer Input Pin

			// Find the source's output pin and the renderer's input pin
			if (FAILED(hr = pCTR->FindPin(L"In", &pFTRPinIn)))
			{
				Msg(TEXT("Could not find input pin!  hr=0x%x"), hr);
				return hr;
			}

			// Connect these two filters
			if (FAILED(hr = m_pGB->Connect(pFSrcPinOut, pFTRPinIn)))
			{
				Msg(TEXT("Could not connect pins!  hr=0x%x"), hr);
				return hr;
			}
		}
		else
		{

			// Render the source filter's output pin.  The Filter Graph Manager
			// will connect the video stream to the loaded CTextureRenderer
			// and will load and connect an audio renderer (if needed).

			if (FAILED(hr = m_pGB->Render(pFSrcPinOut)))
			{
				Msg(TEXT("Could not render source output pin!  hr=0x%x"), hr);
				return hr;
			}
		}
	}
	else
	{
		SmartPtr<IBasicAudio> IAudio;
		hr = m_pGB.QueryInterface(&IAudio);

		//WMVとかMPEG2とか
		hr = m_pGB->RenderFile(wFileName,NULL);
		if(FAILED(hr))
			return hr;
		if(!bSound)
		{
			// 無音
			hr = IAudio->put_Volume(-10000);
		}
		else
		{
			//音の出力
			hr = IAudio->put_Volume(0);
		}
	}

    // Get the graph's media control, event & position interfaces
    m_pGB.QueryInterface(&m_pMC);
    m_pGB.QueryInterface(&m_pMP);
    m_pGB.QueryInterface(&m_pME);
    
    // Start the graph running;
    if (FAILED(hr = m_pMC->Run()))
    {
        Msg(TEXT("Could not run the DirectShow graph!  hr=0x%x"), hr);
        return hr;
    }
	
	//テクスチャの取得
	m_pTexture = pCTR->GetTexture();
	pCTR->GetVideoDesc(&m_lWidth, &m_lHeight, &m_lPitch);

	D3DSURFACE_DESC desc;
	m_pTexture->GetLevelDesc(0, &desc);

	//テクスチャが2の累乗でしか確保できない場合に余剰分を表示しないためのUV計算
	m_fu = (FLOAT)m_lWidth / (FLOAT)desc.Width;
	m_fv = (FLOAT)m_lHeight / (FLOAT)desc.Height;

	m_pMC->Stop();

    return S_OK;
}


//-----------------------------------------------------------------------------
// CheckMovieStatus: If the movie has ended, rewind to beginning
//-----------------------------------------------------------------------------
void MovieTexture::CheckMovieStatus(void)
{
    long lEventCode;
    long lParam1;
    long lParam2;
    HRESULT hr;

    if (!m_pME)
        return;
        
    // Check for completion events
    hr = m_pME->GetEvent(&lEventCode, (LONG_PTR *) &lParam1, (LONG_PTR *) &lParam2, 0);
    if (SUCCEEDED(hr))
    {
        // If we have reached the end of the media file, reset to beginning
        if (EC_COMPLETE == lEventCode) 
        {
            hr = m_pMP->put_CurrentPosition(0);
        }

        // Free any memory associated with this event
        hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
    }
}


//-----------------------------------------------------------------------------
// CleanupDShow
//-----------------------------------------------------------------------------
void MovieTexture::CleanupDShow(void)
{
#ifdef REGISTER_FILTERGRAPH
    // Pull graph from Running Object Table (Debug)
    RemoveFromROT();
#endif

    // Shut down the graph
    if (!(!m_pMC)) m_pMC->Stop();

	//各種オブジェクトの解放
    if (!(!m_pMC)) m_pMC.Release();
    if (!(!m_pME)) m_pME.Release();
    if (!(!m_pMP)) m_pMP.Release();
    if (!(!m_pGB)) m_pGB.Release();
    if (!(!m_pRenderer)) m_pRenderer.Release();
}

//ムービーのイベントを取得
VOID MovieTexture::GetEvent(long* lEventCode, long* lParam1, long* lParam2, long msTimeout)
{
	m_pME->GetEvent(lEventCode, lParam1, lParam2, msTimeout);
}

