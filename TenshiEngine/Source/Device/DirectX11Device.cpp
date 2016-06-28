
#include "DirectX11Device.h"

#include "Window/Window.h"
#include "Graphic/RenderTarget/RenderTarget.h"

#include "Application/DefineDrawMultiThread.h"

#include "Sound/Sound.h"

//static
D3D_DRIVER_TYPE			Device::mDriverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL		Device::mFeatureLevel = D3D_FEATURE_LEVEL_11_1;
ID3D11Device*			Device::mpd3dDevice = NULL;
ID3D11DeviceContext*	Device::mpImmediateContext = NULL;
IDXGISwapChain*			Device::mpSwapChain = NULL;
RenderTarget*			Device::mRenderTargetBack = NULL;

#include "Game/RenderingSystem.h"

//static
HRESULT Device::Init(const Window& window)
{

	_SYSTEM_LOG_H("デバイスの初期化");
	HRESULT hr = S_OK;

	//RECT rc;
	//GetClientRect(window.GetGameScreenHWND(), &rc);
	//UINT width = rc.right - rc.left;
	//UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
//#ifdef _DEBUG
//	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif


	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_SOFTWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = window.GetGameScreenHWND();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	sd.Windowed = TRUE;
	sd.BufferDesc.Width = WindowState::mWidth;
	sd.BufferDesc.Height = WindowState::mHeight;
	sd.Flags = 0;

	//sd.Windowed = FALSE;
	//sd.BufferDesc.Width = 0;// WindowState::mWidth;
	//sd.BufferDesc.Height = 0;//WindowState::mHeight;
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	//有効なドライバータイプでブレイク
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		mDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, mDriverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &mpSwapChain, &mpd3dDevice, &mFeatureLevel, &mpImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr)){
		Window::AddLog("D3Dドライバー作成失敗");
		_SYSTEM_LOG_ERROR("D3Dドライバーの作成");
		_SYSTEM_LOG_H_ERROR();
		return hr;
	}

	
	
	IDXGIFactory* pfac = nullptr;
	hr = mpSwapChain->GetParent(__uuidof(IDXGIFactory), (void**)&pfac);
	if (pfac){
		// 余計な機能を無効にする設定をする。(ALT+ENTER)
		pfac->MakeWindowAssociation(window.GetMainHWND(), DXGI_MWA_NO_WINDOW_CHANGES
			| DXGI_MWA_NO_ALT_ENTER);
	}




	ID3D11DeviceContext *context;
	hr = mpd3dDevice->CreateDeferredContext(NULL, &context);
	if (FAILED(hr)){
		Window::AddLog("デファードコンテキスト作成失敗");
		_SYSTEM_LOG_ERROR("デファードコンテキストの作成");
		_SYSTEM_LOG_H_ERROR();
		return hr;
	}

	auto render = RenderingSystem::Instance();
#if _DRAW_MULTI_THREAD

	render->PushEngine(new RenderingEngine(mpImmediateContext), ContextType::Immediate);
	render->PushEngine(new RenderingEngine(context), ContextType::MainDeferrd);
#else
	render->PushEngine(new RenderingEngine(context), ContextType::Immediate);
	render->PushEngine(new RenderingEngine(mpImmediateContext), ContextType::MainDeferrd);
#endif



	//Device::mpSwapChain->SetFullscreenState(TRUE, NULL);


	mRenderTargetBack = new RenderTarget();
	hr = mRenderTargetBack->CreateBackBuffer(WindowState::mWidth, WindowState::mHeight);
	if (FAILED(hr)){
		Window::AddLog("バックバッファー作成失敗");
		_SYSTEM_LOG_ERROR("バックバッファーの作成");
		_SYSTEM_LOG_H_ERROR();
		return hr;
	}

	//レンダーターゲットと深度ステンシルの関連付け
	mRenderTargetBack->SetRendererTarget(mpImmediateContext);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)WindowState::mWidth;
	vp.Height = (FLOAT)WindowState::mHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mpImmediateContext->RSSetViewports(1, &vp);
	mRenderTargetBack->ClearView(mpImmediateContext);

	// Set primitive topology
	//インデックスの並び　Z字など
	mpImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SoundDevice::Initialize();

	return S_OK;

	//mpImmediateContext->OMSetRenderTargets(0, 0, 0);
	//mRenderTargetBack->Release();
	//
	//Device::mpSwapChain->ResizeBuffers(1,
	//	WindowState::mWidth,
	//	WindowState::mHeight,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	//CreateBackBuffer();
}


//static
void Device::CleanupDevice()
{
	RenderingSystem::Instance()->Release();

	if (mpImmediateContext) mpImmediateContext->ClearState();

	if (mpSwapChain) mpSwapChain->Release();
	if (mpImmediateContext) mpImmediateContext->Release();
	if (mpd3dDevice) mpd3dDevice->Release();

	if (mRenderTargetBack){
		mRenderTargetBack->Release();
		delete mRenderTargetBack;
	}

	SoundDevice::Release();
}