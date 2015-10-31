
#include "Font.h"
#include <Shlwapi.h>
#include "Window/Window.h"
#include <D3D11.h>
#include "Device/DirectX11Device.h"

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}


#include < locale.h >
#include <string>

void init_setlocale(){
	setlocale(LC_CTYPE, "Japanese");
}

//ワイド文字列からマルチバイト文字列
//ロケール依存
void narrow(const std::wstring &src, std::string &dest) {
	char *mbs = new char[src.length() * MB_CUR_MAX + 1];
	wcstombs(mbs, src.c_str(), src.length() * MB_CUR_MAX + 1);
	dest = mbs;
	delete[] mbs;
}

//マルチバイト文字列からワイド文字列
//ロケール依存
void widen(const std::string &src, std::wstring &dest) {
	wchar_t *wcs = new wchar_t[src.length() + 1];
	mbstowcs(wcs, src.c_str(), src.length() + 1);
	dest = wcs;
	delete[] wcs;
}

HFONT new_font, old_font;

#define _FONT_
//#define D2D_USE_C_DEFINITIONS

#include <DWrite.h>
#include <d2d1.h>
#pragma comment(lib,"lib/dwrite.lib")
#pragma comment(lib,"lib/d2d1.lib")


#ifndef _FONT_


IDWriteFactory* pDWriteFactory_=NULL;
IDWriteTextFormat* pTextFormat_ = NULL;

ID2D1Factory* pD2DFactory_ = NULL;

ID2D1RenderTarget;
ID2D1HwndRenderTarget* pRT_ = NULL;
ID2D1SolidColorBrush* pBlackBrush_ = NULL;

IDXGIDevice1           *dxgiDev_;

const wchar_t* szText_ = L"Hello World using  DirectWrite!";

#else

#define FONT_DEFAULT_SIZE 48.0f

IDWriteFactory* pDWriteFactory_ = NULL;
IDWriteTextFormat* dwriteTextFormat_ = NULL;
ID2D1RenderTarget* d2dRenderTarget_ = NULL;

IDXGIDevice1           *dxgiDev_ = NULL;

ID3D11Texture2D			*sharedSurfTex_ = NULL;
IDXGIKeyedMutex			*mutex11_ = NULL, *mutex101_ = NULL;
IDXGISurface			*dxgiBackBuffer_ = NULL;

ID3D11RenderTargetView*	mpRenderTargetView = NULL;
Texture*					mTexture = NULL;

#endif

static bool initok=false;


Texture Font::GetFontTexture(){
	if (mTexture)
		return *mTexture;
	return Texture();
}

HRESULT InitF_(){
	init_setlocale();
#ifndef _FONT_

	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory_);
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory_));
	}
	if (SUCCEEDED(hr))
	{
		hr = pDWriteFactory_->CreateTextFormat(
			L"Gabriola",                // Font family name.
			NULL,                       // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			72.0f,
			L"en-us",
			&pTextFormat_
			);
	}
	if (SUCCEEDED(hr))
	{
		hr = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	return hr;

#else

	HRESULT hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = WindowState::mWidth;
	descDepth.Height = WindowState::mHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	descDepth.CPUAccessFlags = 0;
	//descDepth.MiscFlags = 0;
	descDepth.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;


	hr = Device::mpd3dDevice->CreateTexture2D(&descDepth, NULL, &sharedSurfTex_);
	if (FAILED(hr))
		return hr;

	// DXGIデバイスの作成
	hr = Device::mpd3dDevice->QueryInterface<IDXGIDevice1>(&dxgiDev_);
	if (FAILED(hr))return hr;

	// キューに格納されていく描画コマンドをスワップ時に全てフラッシュする
	dxgiDev_->SetMaximumFrameLatency(1);

	// DXGIアダプタ（GPU）の取得
	//IDXGIAdapter *adapter;
	//hr = dxgiDev_->GetAdapter(&adapter);
	//if (FAILED(hr))return hr;


	hr = sharedSurfTex_->QueryInterface(IID_PPV_ARGS(&mutex11_));
	if (FAILED(hr))return hr;

	IDXGIResource *dxgiRes;
	hr = sharedSurfTex_->QueryInterface(IID_PPV_ARGS(&dxgiRes));
	if (FAILED(hr))return hr;


	//hr = sharedSurfTex_->QueryInterface(IID_PPV_ARGS(&dxgiBackBuffer_));
	//if (FAILED(hr))return hr;

	HANDLE hShared;
	hr = dxgiRes->GetSharedHandle(&hShared);
	dxgiRes->Release();
	if (FAILED(hr))return hr;

	hr = Device::mpd3dDevice->OpenSharedResource(hShared, IID_PPV_ARGS(&dxgiBackBuffer_));
	if (FAILED(hr))return hr;

	hr = dxgiBackBuffer_->QueryInterface(IID_PPV_ARGS(&mutex101_));
	if (FAILED(hr))return hr;


	//IDXGISurface *pDxgiSurface = NULL;
	//hr = m_pOffscreenTexture->QueryInterface(&pDxgiSurface);

	// Direct2Dのファクトリーの作成
	D2D1_FACTORY_OPTIONS d2dOpt;
	ZeroMemory(&d2dOpt, sizeof d2dOpt);
	ID2D1Factory *d2dFactory;
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory),
		&d2dOpt,
		reinterpret_cast<void**>(&d2dFactory));
	if (FAILED(hr))return hr;

	// DPIの取得
	float dpiX, dpiY;
	d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	// Direct2Dの描画先となるレンダーターゲットを作成
	D2D1_RENDER_TARGET_PROPERTIES d2dProp =
		D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_HARDWARE,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY);
	// Direct2Dの描画先となるレンダーターゲットを作成
	//D2D1_RENDER_TARGET_PROPERTIES d2dProp =
	//	D2D1::RenderTargetProperties(
	//	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	//	D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
	//	dpiX,
	//	dpiY);

	hr = d2dFactory->CreateDxgiSurfaceRenderTarget(
		dxgiBackBuffer_, &d2dProp, &d2dRenderTarget_);
	if (FAILED(hr))return hr;


	// DirectWriteのファクトリの作成
	IDWriteFactory *dwriteFactory;
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(dwriteFactory),
		reinterpret_cast<IUnknown**>(&dwriteFactory));
	if (FAILED(hr))return hr;

	// テキストフォーマットの作成
	hr = dwriteFactory->CreateTextFormat(
		L"Meiryo UI", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		FONT_DEFAULT_SIZE, L"", &dwriteTextFormat_);
	dwriteFactory->Release();
	if (FAILED(hr))return hr;

	// 文字の位置の設定
	hr = dwriteTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr))return hr;

	// パラグラフの指定
	hr = dwriteTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if (FAILED(hr))return hr;



	// RenderTargetView作成　MRTに必要な個数
	hr = Device::mpd3dDevice->CreateRenderTargetView(sharedSurfTex_, nullptr, &mpRenderTargetView);
	if (FAILED(hr))
		return hr;
	// デプスバッファ
	//Device::mpd3dDevice->CreateDepthStencilView(texture, nullptr, &dsview);

	ID3D11ShaderResourceView* pShaderResourceView;
	hr = Device::mpd3dDevice->CreateShaderResourceView(sharedSurfTex_, nullptr, &pShaderResourceView);
	if (FAILED(hr))
		return hr;
	mTexture = new Texture();
	hr = mTexture->Create(pShaderResourceView);
	if (FAILED(hr))
		return hr;

	//ClearView();

	initok = true;
	return hr;
#endif
}

HRESULT Create(){

#ifndef _FONT_
	HRESULT hr = S_OK;
	RECT rc;
	HWND hwnd_ = Window::GetGameScreenHWND();
	GetClientRect(hwnd_, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	//size.height /= 10;
	//size.width /= 10;
	if (!pRT_)
	{
		// Create a Direct2D render target.
		hr = pD2DFactory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd_,size),
			&pRT_
			);

		// Create a black brush.
		if (SUCCEEDED(hr))
		{
			hr = pRT_->CreateSolidColorBrush(
				D2D1::ColorF(RGB(255,0,0),1.0f),
				&pBlackBrush_
				);
		}
	}
	return hr;
#else
	return S_OK;
#endif
}


HRESULT Draw(std::string text){
	if (!initok)return;

#ifndef _FONT_
	HRESULT hr = S_OK;

	RECT rc;
	HWND hwnd_ = Window::GetGameScreenHWND();
	GetClientRect(hwnd_, &rc);

	UINT32 cTextLength_ = (UINT32)wcslen(szText_);

	float dpiScaleX_ = 1.0f;
	float dpiScaleY_ = 1.0f;

	D2D1_RECT_F layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.left) / dpiScaleX_,
		static_cast<FLOAT>(rc.top) / dpiScaleY_,
		static_cast<FLOAT>(rc.right - rc.left) / dpiScaleX_,
		static_cast<FLOAT>(rc.bottom - rc.top) / dpiScaleY_
		);

	pRT_->DrawText(
		szText_,        // The string to render.
		cTextLength_,    // The string's length.
		pTextFormat_,    // The text format.
		layoutRect,       // The region of the window where the text will be rendered.
		pBlackBrush_     // The brush used to draw the text.
		);

	return hr;
#else
	HRESULT hr;
	std::wstring wtext;
	widen(text, wtext);

	// Direct3D

	hr = mutex11_->AcquireSync(0, INFINITE);
	if (FAILED(hr))return hr;

	//float clearColor[] = { 0.1f, 0.2f, 0.4f, 1.0f };
	//g_d3d.cont_->ClearRenderTargetView(g_d3d.backBufferRTV_, clearColor);

	hr = mutex11_->ReleaseSync(1);
	if (FAILED(hr))return hr;

	// Direct2D

	hr = mutex101_->AcquireSync(1, INFINITE);
	if (FAILED(hr))return hr;

	D2D1_COLOR_F color;
	color = D2D1::ColorF(1,1,1,1.0f);

	ID2D1SolidColorBrush *brush;
	hr = d2dRenderTarget_->CreateSolidColorBrush(color, &brush);
	if (FAILED(hr))return hr;

	D2D1_RECT_F rect;
	//rect = D2D1::RectF(0 + el, 500, 440 + el, 510);

	bool flag = true;
	if (flag){
		d2dRenderTarget_->BeginDraw();
		d2dRenderTarget_->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
		//d2dRenderTarget_->FillRectangle(&rect, brush);

		// DirectWrite

		rect = D2D1::RectF(0, 200, 1000, 201);

		UINT s = wcslen(wtext.c_str());
		d2dRenderTarget_->DrawText(
			wtext.c_str(), s, dwriteTextFormat_, &rect, brush);

		hr = d2dRenderTarget_->EndDraw();
		flag = false;

	}
	brush->Release();

	hr = mutex101_->ReleaseSync(2);
	if (FAILED(hr))return hr;

	// Direct3D

	hr = mutex11_->AcquireSync(2, INFINITE);
	if (FAILED(hr))return hr;

	//Device::mpImmediateContext->CopyResource(BackBufferTex2D_, sharedSurfTex_);

	return hr;
#endif
}

void Font::DrawEnd(){

	mutex11_->ReleaseSync(0);
}


void Release_(){

#ifndef _FONT_

	SafeRelease(&pRT_);
	SafeRelease(&pBlackBrush_);
#else

	if (mutex11_)mutex11_->ReleaseSync(0);

	SafeRelease(&dxgiDev_);
	SafeRelease(&d2dRenderTarget_);
	SafeRelease(&dwriteTextFormat_);
	SafeRelease(&sharedSurfTex_);
	SafeRelease(&mutex11_);
	SafeRelease(&mutex101_);
	SafeRelease(&dxgiBackBuffer_);
	SafeRelease(&mpRenderTargetView);
	SafeRelease(&pDWriteFactory_);
	//if (mTexture)
	//	delete mTexture;
#endif
}



//static
void  Font::Init(){
	HRESULT hr;
	int height = 60;
	char font_name[] = "Comic Sans MS";

	//フォント作成
	new_font = CreateFont(
		height, //高さ
		0,  //横幅
		0,  //角度
		0,  //よくわからなんだ
		FW_NORMAL,  //太さ
		0,  //斜体
		0,  //下線
		0,  //打ち消し
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, font_name);

	//hdc = GetDC(Window::GetHWND());            //デバイスコンテキスト取得
	//hdc = BeginPaint(Window::GetHWND(), &ps);

	InitF_();

}

//static
void  Font::Release(){
	//ReleaseDC(Window::GetHWND(), hdc);         //デバイスコンテキスト解放
	//EndPaint(Window::GetHWND(), &ps);


	//pSurface1->ReleaseDC(&rc);

	Release_();

	DeleteObject(new_font);         //フォント消去
}

//static
void Font::TextOutFont(int x, int y, int height,const char *text_str, char *font_name, unsigned char col){
	
	HRESULT hr;

	//PAINTSTRUCT ps;


	//const char* szText_ = "Hello World using  DirectWrite!";
	//UINT32 cTextLength_ = (UINT32)strlen(szText_);


#ifndef _FONT_

	static bool initf = true;
	if (initf){
		hr = InitF();
		initf = false;
	}

	hr = Create();
	
	if (SUCCEEDED(hr))
	{
		pRT_->BeginDraw();

		pRT_->SetTransform(D2D1::IdentityMatrix());

		pRT_->Clear(D2D1::ColorF(RGB(0,0,0),0.0f));

		// Call the DrawText method of this class.
		hr = Draw();

		if (SUCCEEDED(hr))
		{
			hr = pRT_->EndDraw();
		}
	}

	if (FAILED(hr))
	{
		Release();
	}

	return;

#else

	Draw(text_str);
	return;
#endif


	IDXGISurface1* pSurface1 = NULL;
	HDC hdc;
	hr = Device::mpSwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pSurface1);

	if (SUCCEEDED(hr))
	{
		hr = pSurface1->GetDC(FALSE, &hdc);
		if (SUCCEEDED(hr))
		{
			RECT rc = { x, y, x + height*strlen(text_str), y + height };
			//InvalidateRect(Window::GetHWND(), &rc, true);
			SetBkMode(hdc, TRANSPARENT);         //背景モード設定
			old_font = (HFONT)SelectObject(hdc, new_font); //フォント選択
			//フォントカラー設定
			SetTextColor(hdc, RGB(255, 0, 0));

			//BeginPath(hdc);
			TextOut(hdc, x, y, text_str, strlen(text_str)); //出力

			//EndPath(hdc);
			// パスから一部の領域を抽出
			//SelectClipPath(hdc, RGN_AND);
			// 次のコードでも SelectClipPath() と同じ結果が得られる
			// SelectClipRgn(hdc, PathToRegion(hdc));
			// 得られた領域をグレイで塗りつぶす
			//FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
			//DrawText(hdc, text_str, strlen(text_str), &rc, 0);
			SelectObject(hdc, old_font);         //フォント復元

			pSurface1->ReleaseDC(NULL);
		}

		pSurface1->Release();
	}



	//EndPaint(Window::GetHWND(), &ps);

	//InvalidateRect(Window::GetHWND(), NULL, true);

}