#pragma once

#include <D3D11.h>
#include "Device/DirectX11Device.h"
#include "Graphic/Material/Texture.h"

#include "Game/RenderingSystem.h"

class RenderTarget{
public:
	RenderTarget();
	~RenderTarget();
	HRESULT Create(UINT Width, UINT Height, DXGI_FORMAT format);

	HRESULT CreateRTandDepth(UINT Width, UINT Height, DXGI_FORMAT format);
	HRESULT CreateCUBE(UINT Width, UINT Height, DXGI_FORMAT format);

	HRESULT CreateDepth(UINT Width, UINT Height);

	HRESULT CreateBackBuffer(UINT Width, UINT Height);

	bool Resize(UINT Width, UINT Height);

	void ClearView(ID3D11DeviceContext* context) const;
	void ClearDepth(ID3D11DeviceContext* context) const;

	Texture& GetTexture();

	void PSSetShaderResource(ID3D11DeviceContext* context, UINT Slot) const;

	void SetRendererTarget(ID3D11DeviceContext* context) const;
	static void SetRendererTarget(ID3D11DeviceContext* context, UINT num, const RenderTarget* render, const RenderTarget* depth);
	static void NullSetRendererTarget(ID3D11DeviceContext* context);
	void Release();

	ID3D11RenderTargetView*	GetRT(){ return mpRenderTargetView; };
	ID3D11Texture2D* GetTexture2D(){ return mpTexture2D; };
private:

	static void _SetViewport(ID3D11DeviceContext* context, const RenderTarget& rt);

	bool _CreateTexture2D(const D3D11_TEXTURE2D_DESC* tex_desc);
	bool _CreateDepthTexture2D(const D3D11_TEXTURE2D_DESC* tex_desc);

	bool _CreateRenderTargetView();
	bool _CreateTexture();

	bool _CreateDepthStencilView(const D3D11_DEPTH_STENCIL_VIEW_DESC* desc);

	ID3D11Texture2D*		mpTexture2D;
	ID3D11RenderTargetView*	mpRenderTargetView;
	ID3D11Texture2D*		mpDepthTexture2D;
	ID3D11DepthStencilView*	mpDepthStencilView;
	Texture					mTexture;

	UINT m_Width;
	UINT m_Height;
	DXGI_FORMAT m_Format;
};

class UAVRenderTarget{
public:
	UAVRenderTarget();
	~UAVRenderTarget();
	HRESULT Create(UINT Width, UINT Height, DXGI_FORMAT format);
	void ClearView(ID3D11DeviceContext* context) const;

	void SetUnorderedAccessView(ID3D11DeviceContext* context) const;
	void Release();
	Texture& GetTexture();

	void PSSetShaderResource(ID3D11DeviceContext* context, UINT Slot) const;


	ID3D11UnorderedAccessView*	GetUAV(){ return m_UAV; };
	ID3D11Texture2D* GetTexture2D(){ return mpTexture2D; };

private:
	ID3D11Texture2D*		mpTexture2D;
	ID3D11UnorderedAccessView* m_UAV;
	Texture					mTexture;
};