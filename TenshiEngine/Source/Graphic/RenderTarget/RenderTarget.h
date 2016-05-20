#pragma once

#include <D3D11.h>
#include "Window/Window.h"
#include "Device/DirectX11Device.h"
#include "Graphic/Material/Texture.h"

#include "Game/RenderingSystem.h"

class RenderTarget{
public:
	RenderTarget()
		: mpRenderTargetView(NULL)
		, mpTexture2D(NULL)
		, mpDepthStencilView(NULL)
	{
	}
	~RenderTarget()
	{
	}
	HRESULT Create(UINT Width, UINT Height,DXGI_FORMAT format)
	{
		HRESULT hr = S_OK;

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC tex_desc;
		ZeroMemory(&tex_desc, sizeof(tex_desc));
		tex_desc.Width = Width;
		tex_desc.Height = Height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		//tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex_desc.Format = format;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		//レンダーターゲットとして使用＆シェーダリソースとして利用
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;


		//デプステクスチャの場合
		//tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		//他のメンバは省略
		hr = Device::mpd3dDevice->CreateTexture2D(&tex_desc, nullptr, &mpTexture2D);
		if (FAILED(hr))
			return hr;

		//Width、Heightは全レンダーターゲットとデプスバッファすべて同じにする

		// RenderTargetView作成　MRTに必要な個数
		hr = Device::mpd3dDevice->CreateRenderTargetView(mpTexture2D, nullptr, &mpRenderTargetView);
		if (FAILED(hr))
			return hr;
		// デプスバッファ
		//Device::mpd3dDevice->CreateDepthStencilView(texture, nullptr, &dsview);

		ID3D11ShaderResourceView* pShaderResourceView;
		hr = Device::mpd3dDevice->CreateShaderResourceView(mpTexture2D, nullptr, &pShaderResourceView);
		if (FAILED(hr))
			return hr;

		hr = mTexture.Create(pShaderResourceView);
		if (FAILED(hr))
			return hr;

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		ClearView(render->m_Context);

		return S_OK;
	}
	HRESULT CreateDepth(UINT Width, UINT Height)
	{
		HRESULT hr = S_OK;

		//デプスバッファ
		//◆DXGI_FORMAT_D24_UNORM_S8_UINT
		//精度に問題なければこれを採用
		//◆DXGI_FORMAT_D32_FLOAT
		//デプス値の精度が必要であれば
		//◆DXGI_FORMAT_R24G8_TYPELESS
		//デプス値アクセスのための特殊フォーマット、これでテクスチャ作成
		//CreateDepthStencilView時にD24_UNORM_S8_UINTに変換
		//◆DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//◆DXGI_FORMAT_X24_TYPELESS_G8_UINT
		//シェーダでデプスステンシル値を取得するためのフォーマット
		//CreateShaderResourceViewで指定

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC tex_desc;
		ZeroMemory(&tex_desc, sizeof(tex_desc));
		tex_desc.Width = Width;
		tex_desc.Height = Height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		//tex_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		tex_desc.Format = DXGI_FORMAT_D32_FLOAT;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;


		//他のメンバは省略
		hr = Device::mpd3dDevice->CreateTexture2D(&tex_desc, nullptr, &mpTexture2D);
		if (FAILED(hr))
			return hr;


		//Width、Heightは全レンダーターゲットとデプスバッファすべて同じにする
		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		// デプスバッファ
		hr = Device::mpd3dDevice->CreateDepthStencilView(mpTexture2D, &descDSV, &mpDepthStencilView);
		if (FAILED(hr))
			return hr;

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		ClearView(render->m_Context);

		//ID3D11ShaderResourceView* pShaderResourceView;
		//hr = Device::mpd3dDevice->CreateShaderResourceView(mpTexture2D, nullptr, &pShaderResourceView);
		//if (FAILED(hr))
		//	return hr;
		//
		//mTexture.Create(shared_ptr<ID3D11ShaderResourceView>(pShaderResourceView));


		return S_OK;
	}

	HRESULT CreateBackBuffer(UINT Width, UINT Height)
	{
		HRESULT hr = S_OK;

		// Create a render target view
		ID3D11Texture2D* pBackBuffer = NULL;
		hr = Device::mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr))
			return hr;

		//レンダーターゲットの作成
		hr = Device::mpd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &mpRenderTargetView);
		pBackBuffer->Release();
		if (FAILED(hr))
			return hr;


		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = Width;
		descDepth.Height = Height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//descDepth.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		//descDepth.MiscFlags = D3D11_RESOURCE_MISC_SHARED;


		hr = Device::mpd3dDevice->CreateTexture2D(&descDepth, NULL, &mpTexture2D);
		if (FAILED(hr))
			return hr;


		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;//descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = Device::mpd3dDevice->CreateDepthStencilView(mpTexture2D, &descDSV, &mpDepthStencilView);
		if (FAILED(hr))
			return hr;



		//ID3D11ShaderResourceView* pShaderResourceView;
		//hr = Device::mpd3dDevice->CreateShaderResourceView(mpTexture2D, nullptr, &pShaderResourceView);
		//if (FAILED(hr))
		//	return hr;

		//hr = mTexture.Create(pShaderResourceView);
		//if (FAILED(hr))
		//	return hr;


		return S_OK;
	}

	void ClearView(ID3D11DeviceContext* context) const{
		//
		// Clear the back buffer
		//
		float ClearColor[4] = { 0,0,0, 1.0f };
		if (mpRenderTargetView)context->ClearRenderTargetView(mpRenderTargetView, ClearColor);
	}
	void ClearDepth(ID3D11DeviceContext* context) const{
		if (mpDepthStencilView)context->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	Texture& GetTexture(){
		return mTexture;
	}

	void PSSetShaderResource(ID3D11DeviceContext* context ,UINT Slot) const{
		mTexture.PSSetShaderResources(context,Slot);
	}

	void SetRendererTarget(ID3D11DeviceContext* context) const{
		//レンダーターゲットと深度ステンシルの関連付け
		
		if (mpRenderTargetView)
		context->OMSetRenderTargets(1, &mpRenderTargetView, mpDepthStencilView);
	}
	static void SetRendererTarget(ID3D11DeviceContext* context,UINT num, const RenderTarget* render, const RenderTarget* depth){

		_ASSERT(num<9);

		ID3D11RenderTargetView* r[8];
		for (UINT i = 0; i < num; i++){
			r[i] = render[i].mpRenderTargetView;
		}
		ID3D11DepthStencilView* d = depth ? depth->mpDepthStencilView : NULL;
		//レンダーターゲットと深度ステンシルの関連付け
		context->OMSetRenderTargets(num, r, d);
	}
	static void NullSetRendererTarget(ID3D11DeviceContext* context){

		ID3D11RenderTargetView* r[8];
		for (UINT i = 0; i < 8; i++){
			r[i] = NULL;
		}
		//レンダーターゲットと深度ステンシルの関連付け
		context->OMSetRenderTargets(8, r, NULL);
	}

	void Release()
	{
		if (mpTexture2D){
			mpTexture2D->Release();
			mpTexture2D = NULL;
		}
		//ID3D11RenderTargetView* rt[8];
		//ID3D11DepthStencilView* ds;
		//Device::mpImmediateContext->OMGetRenderTargets(8, rt, &ds);

		if (mpDepthStencilView){
			//if (ds == mpDepthStencilView){
			//	NullSetRendererTarget();
			//}
			mpDepthStencilView->Release();
			mpDepthStencilView = NULL;
		}
		if (mpRenderTargetView){
			//for (int i = 0; i < 8; i++){
			//	if (rt[i] == mpRenderTargetView){
			//		NullSetRendererTarget();
			//	}
			//}
			mpRenderTargetView->Release();
			mpRenderTargetView = NULL;
		}

	}


	ID3D11RenderTargetView*	GetRT(){ return mpRenderTargetView; };
	ID3D11Texture2D* GetTexture2D(){ return mpTexture2D; };

private:
	ID3D11RenderTargetView*	mpRenderTargetView;
	ID3D11Texture2D*		mpTexture2D;
	ID3D11DepthStencilView*	mpDepthStencilView;
	Texture					mTexture;
};

class UAVRenderTarget{
public:
	UAVRenderTarget()
		: m_UAV(NULL)
		, mpTexture2D(NULL)
	{
	}
	~UAVRenderTarget()
	{
	}
	HRESULT Create(UINT Width, UINT Height, DXGI_FORMAT format)
	{
		HRESULT hr = S_OK;

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC tex_desc;
		ZeroMemory(&tex_desc, sizeof(tex_desc));
		tex_desc.Width = Width;
		tex_desc.Height = Height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		//tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tex_desc.Format = format;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		//レンダーターゲットとして使用＆シェーダリソースとして利用
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;


		//デプステクスチャの場合
		//tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		//他のメンバは省略
		hr = Device::mpd3dDevice->CreateTexture2D(&tex_desc, nullptr, &mpTexture2D);
		if (FAILED(hr))
			return hr;

		ID3D11ShaderResourceView* pShaderResourceView;
		hr = Device::mpd3dDevice->CreateShaderResourceView(mpTexture2D, nullptr, &pShaderResourceView);
		if (FAILED(hr))
			return hr;

		hr = mTexture.Create(pShaderResourceView);
		if (FAILED(hr))
			return hr;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = 0;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		Device::mpd3dDevice->CreateUnorderedAccessView(mpTexture2D, &uavDesc, &m_UAV);


		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		ClearView(render->m_Context);

		return S_OK;
	}
	void ClearView(ID3D11DeviceContext* context) const{
		//
		// Clear the back buffer
		//
		float ClearColor[4] = { 0, 0, 0, 1.0f };
		if (m_UAV)context->ClearUnorderedAccessViewFloat(m_UAV, ClearColor);
	}

	void SetUnorderedAccessView(ID3D11DeviceContext* context) const{
		if (m_UAV)
			context->CSSetUnorderedAccessViews(0, 1, &m_UAV, (UINT*)&m_UAV);
	}

	void Release()
	{
		if (mpTexture2D){
			mpTexture2D->Release();
			mpTexture2D = NULL;
		}
		if (m_UAV){
			m_UAV->Release();
			m_UAV = NULL;
		}

	}

	Texture& GetTexture(){
		return mTexture;
	}

	void PSSetShaderResource(ID3D11DeviceContext* context, UINT Slot) const{
		mTexture.PSSetShaderResources(context, Slot);
	}


	ID3D11UnorderedAccessView*	GetUAV(){ return m_UAV; };
	ID3D11Texture2D* GetTexture2D(){ return mpTexture2D; };

private:
	ID3D11Texture2D*		mpTexture2D;
	ID3D11UnorderedAccessView* m_UAV;
	Texture					mTexture;
};