
#include <D3D11.h>
#include <string>
#include <DWrite.h>
#include <d2d1.h>
#include <Shlwapi.h>

#include "Font.h"
#include "Window/Window.h"
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

#pragma comment(lib,"dwrite.lib")
#pragma comment (lib, "d3d10_1.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dxgi.lib")


#define FONT_DEFAULT_SIZE 48.0f


ID3D10Device1*		FontManager::device101 = NULL;
ID2D1Factory*		FontManager::d2dfactory = NULL;

/*
HRESULT Draw2(std::string text){
	if (!initok)return S_FALSE;

	HRESULT hr;
	std::wstring wtext;
	widen(text, wtext);



	// D3D 11 側のテクスチャの使用を完了。
	keyedmutex11->ReleaseSync(0);


	// D3D 10.1 (D2D) 側のテクスチャの使用を開始。
	hr = keyedmutex10->AcquireSync(0, INFINITE);
	if (FAILED(hr))return hr;



	//D2D1_COLOR_F color;
	//color = D2D1::ColorF(1, 1, 1, 1.0f);

	//ID2D1SolidColorBrush *brush2;
	//hr = rendertarget->CreateSolidColorBrush(color, &brush);
	//if (FAILED(hr))return hr;

	
	D2D1_SIZE_F		size;
	// サイズ等計算
	size = rendertarget->GetSize();

	D2D1_RECT_F rect;
	rect = D2D1::RectF(0, 200, size.width/2, size.height/2);


	rendertarget->BeginDraw();
	{
		rendertarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

		brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f));
		rendertarget->FillRectangle(&rect, brush);
		brush->SetColor(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f));

		rect = D2D1::RectF(0, 200, 1000, 501);

		//微妙に重い描画しないとClearとかより先に描画して消される？
		//rendertarget->FillRectangle(&rect, brush);で調整中
		UINT s = wcslen(wtext.c_str());
		rendertarget->DrawText(
			wtext.c_str(), s, dwriteTextFormat_, &rect, brush);
	}
	hr = rendertarget->EndDraw();


	//static float	angle = 0.0f;
	//const float		strokeWidth = 16.0f;
	//
	//D2D1_SIZE_F		size;
	//D2D1_POINT_2F	pos_size_2;
	//
	//angle += 0.005f;
	//
	//rendertarget->BeginDraw();
	//rendertarget->Clear(D2D1::ColorF(0.0f, 0.7f, 0.0f, 1.0f));
	//
	//// サイズ等計算
	//size = rendertarget->GetSize();
	//pos_size_2.x = size.width / 2.0f; pos_size_2.y = size.height / 2.0f;
	//
	//rendertarget->SetTransform(D2D1::Matrix3x2F::Rotation(cosf(angle) * 2000, pos_size_2));
	//
	//brush->SetColor(D2D1::ColorF(1.0f, 0.0f, 0.0f, 0.5f));
	//rendertarget->DrawLine(
	//	D2D1::Point2F(pos_size_2.x - size.height * 0.4f, pos_size_2.y),
	//	D2D1::Point2F(pos_size_2.x + size.height * 0.4f, pos_size_2.y),
	//	brush, strokeWidth);
	//
	//brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 1.0f, 0.5f));
	//rendertarget->DrawLine(
	//	D2D1::Point2F(pos_size_2.x, pos_size_2.y - size.height * 0.4f),
	//	D2D1::Point2F(pos_size_2.x, pos_size_2.y + size.height * 0.4f),
	//	brush, strokeWidth);
	//
	//hr = rendertarget->EndDraw();
	//if (FAILED(hr))return hr;


	//brush2->Release();

	// D3D 10.1 (D2D) 側のテクスチャの使用を完了。
	hr = keyedmutex10->ReleaseSync(1);
	if (FAILED(hr))return hr;
	// D3D 11 側のテクスチャの使用を開始。
	hr = keyedmutex11->AcquireSync(1, INFINITE);
	if (FAILED(hr))return hr;

	//Device::mpImmediateContext->CopyResource(backbuffer, texture11);


	return hr;
}
*/
/*

HRESULT InitF_(){
	init_setlocale();

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
}

HRESULT Draw(std::string text){
	if (!initok)return S_FALSE;

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
}
*/




IDWriteTextFormat* TextFormat::GetTextFormat(){
	return mWriteTextFormat;
}

//static
HRESULT FontManager::Init(){
	init_setlocale();


	HRESULT hr;

	// 各デバイスの作成
	{
		// D3D 10.1 を作成
		{
			// D3D10_DRIVER_TYPE_HARDWARE と D3D10_CREATE_DEVICE_BGRA_SUPPORT は必ず指定。
			// なお、こちらの環境では D3D10_FEATURE_LEVEL_9_3 にしないと動作しませんでした。
			hr = D3D10CreateDevice1(
				Device::mpAdapter,
				D3D10_DRIVER_TYPE_HARDWARE,
				NULL,
				D3D10_CREATE_DEVICE_BGRA_SUPPORT,
				D3D10_FEATURE_LEVEL_9_3,
				D3D10_1_SDK_VERSION,
				&device101
				);
			if FAILED(hr)
			{
				return hr;
			}
		}
		// D2DFactory の生成
		{
			if FAILED(hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory), (LPVOID*)&d2dfactory))
			{
				return hr;
			}
		}
	}
	return hr;
}

//static
void FontManager::Release(){
	SafeRelease(&device101);
	SafeRelease(&d2dfactory);
}


TextFormat::TextFormat(){

	HRESULT hr;

	// DirectWriteのファクトリの作成
	IDWriteFactory *dwriteFactory;
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(dwriteFactory),
		reinterpret_cast<IUnknown**>(&dwriteFactory));
	if (FAILED(hr))return;

	// テキストフォーマットの作成
	hr = dwriteFactory->CreateTextFormat(
		L"Meiryo UI", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		FONT_DEFAULT_SIZE, L"", &mWriteTextFormat);
	dwriteFactory->Release();
	if (FAILED(hr))return;

	// 文字の位置の設定
	hr = mWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr))return;

	// パラグラフの指定
	hr = mWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if (FAILED(hr))return;
}
TextFormat::~TextFormat(){

	SafeRelease(&mWriteTextFormat);
}

Font::Font(){

	texture11 = NULL;
	keyedmutex11 = NULL;
	keyedmutex10 = NULL;
	rendertarget = NULL;
	brush = NULL;
	surface10 = NULL;
	Initialize();

}
Font::~Font(){

	SafeRelease(&texture11);
	SafeRelease(&keyedmutex11);
	SafeRelease(&keyedmutex10);
	SafeRelease(&rendertarget);
	SafeRelease(&brush);
	SafeRelease(&surface10);
}

void Font::Initialize(){
	SafeRelease(&texture11);
	SafeRelease(&keyedmutex11);
	SafeRelease(&keyedmutex10);
	SafeRelease(&rendertarget);
	SafeRelease(&brush);
	SafeRelease(&surface10);
	mInitializeComplete = false;

	HRESULT hr;
	// 共有するテクスチャを D3D 11 から用意。
	{
		D3D11_TEXTURE2D_DESC std;

		// 作成するテクスチャ情報の設定。
		// ・DXGI_FORMAT_B8G8R8A8_UNORM は固定。
		// ・D3D11_BIND_RENDER_TARGET は D2D での描画対象とするために必須。
		// ・D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX はテクスチャを共有するのに必須。
		ZeroMemory(&std, sizeof(std));
		std.Width = 1024;
		std.Height = 1024;
		std.MipLevels = 1;
		std.ArraySize = 1;
		std.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		std.SampleDesc.Count = 1;
		std.Usage = D3D11_USAGE_DEFAULT;
		std.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		std.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

		if FAILED(Device::mpd3dDevice->CreateTexture2D(&std, NULL, &texture11))
		{
			return;
		}
	}
	// 共有するための D3D 11 のキーミューテックスを取得
	if FAILED(texture11->QueryInterface(__uuidof(IDXGIKeyedMutex), (LPVOID*)&keyedmutex11))
	{
		return;
	}

	HANDLE			sharedHandle;
	// 共有のためのハンドルを取得。
	{
		IDXGIResource* resource11;
		if FAILED(texture11->QueryInterface(__uuidof(IDXGIResource), (LPVOID*)&resource11))
		{
			return;
		}
		if FAILED(resource11->GetSharedHandle(&sharedHandle))
		{
			resource11->Release();
			return;
		}
		resource11->Release();
	}
	// D3D 10.1 で共有サーフェイスを生成。
	if FAILED(hr = FontManager::device101->OpenSharedResource(sharedHandle, __uuidof(IDXGISurface1), (LPVOID*)&surface10))
	{
		return;
	}
	// 共有するための D3D 10.1 のキーミューテックスを取得
	if FAILED(surface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (LPVOID*)&keyedmutex10))
	{
		return;
	}
	// D2D のレンダーターゲットを D3D 10.1 の共有サーフェイスから生成。
	{
		D2D1_RENDER_TARGET_PROPERTIES	rtp;
		ZeroMemory(&rtp, sizeof(rtp));
		rtp.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
		rtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
		if FAILED(FontManager::d2dfactory->CreateDxgiSurfaceRenderTarget(surface10, &rtp, &rendertarget))
		{
			return;
		}
	}

	// ここで共有の準備は整います。

	// D2D の描画用ブラシを生成
	if FAILED(hr = rendertarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &brush))
	{
		return;
	}


	ID3D11ShaderResourceView* pShaderResourceView;
	hr = Device::mpd3dDevice->CreateShaderResourceView(texture11, nullptr, &pShaderResourceView);
	if (FAILED(hr))
		return;
	hr = mTexture.Create(pShaderResourceView);
	if (FAILED(hr))
		return;

	mInitializeComplete = true;

}
HRESULT Font::SetText(const std::string& text){
	HRESULT hr;
	std::wstring wtext;
	widen(text, wtext);
	if (!mInitializeComplete){
		Initialize();
	}
	if (!mInitializeComplete){
		return S_FALSE;
	}
	// D3D 11 側のテクスチャの使用を完了。
	keyedmutex11->ReleaseSync(0);


	// D3D 10.1 (D2D) 側のテクスチャの使用を開始。
	hr = keyedmutex10->AcquireSync(0, INFINITE);
	if (FAILED(hr))return hr;


	D2D1_SIZE_F		size;
	// サイズ等計算
	size = rendertarget->GetSize();

	D2D1_RECT_F rect;
	rect = D2D1::RectF(0, 200, size.width / 2, size.height / 2);


	rendertarget->BeginDraw();
	{
		rendertarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

		brush->SetColor(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f));
		
		rect = D2D1::RectF(0, 0, 1000, 201);
		
		//毎フレーム描画してると上手く描画されない？
		UINT s = wcslen(wtext.c_str());
		rendertarget->DrawText(
			wtext.c_str(), s, mTextFormat.GetTextFormat(), &rect, brush);
	}
	hr = rendertarget->EndDraw();

	// D3D 10.1 (D2D) 側のテクスチャの使用を完了。
	hr = keyedmutex10->ReleaseSync(1);
	if (FAILED(hr))return hr;
	//２回目を呼ぶと描画が反映される
	{
		// D3D 10.1 (D2D) 側のテクスチャの使用を開始。
		hr = keyedmutex10->AcquireSync(1, INFINITE);
		if (FAILED(hr))return hr;
		// D3D 10.1 (D2D) 側のテクスチャの使用を完了。
		hr = keyedmutex10->ReleaseSync(2);
	}
	if (FAILED(hr))return hr;
	// D3D 11 側のテクスチャの使用を開始。
	hr = keyedmutex11->AcquireSync(2, INFINITE);
	if (FAILED(hr))return hr;

	return hr;
}
Texture Font::GetTexture(){
	return mTexture;
}