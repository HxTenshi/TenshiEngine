#pragma once

class Window;
class RenderTarget;
#include <d3d11.h>

class Device{
private:
	Device();

public:
	static
	HRESULT Init(const Window& window);

	static
	void CleanupDevice();

	static D3D_DRIVER_TYPE			mDriverType;
	static D3D_FEATURE_LEVEL		mFeatureLevel;
	//スレッドセーフ
	static ID3D11Device*			mpd3dDevice;
	static ID3D11DeviceContext*		mpImmediateContext;
	static IDXGISwapChain*			mpSwapChain;

	//フォント用
	static IDXGIAdapter1* mpAdapter;
	static RenderTarget* mRenderTargetBack;
};