#pragma once
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>

#include "d3d9.h"
//#include "d3dx9.h"
#include "D3D10_1.h"
#include <D3D11.h>
//#include "d3dx10.h"


namespace System { namespace Windows { namespace Interop
{
	public enum class D3DResourceTypeEx : int
	{
		ID3D10Texture2D = 0,
		ID3D11Texture2D = 1
	};

	public ref class D3DImageEx : D3DImage
	{
		static D3DImageEx()
		{
			InitD3D9(GetDesktopWindow());
		}
	public:
		void SetBackBufferEx(D3DResourceTypeEx resourceType, IntPtr pResource);

	private:
		static IDirect3D9Ex*               m_D3D9;
		static IDirect3DDevice9Ex*         m_D3D9Device;
		
	private:
		static D3DFORMAT ConvertDXGIToD3D9Format(DXGI_FORMAT format);

		static HRESULT GetSharedSurface(HANDLE hSharedHandle, void** ppUnknown, UINT width, UINT height, DXGI_FORMAT format);

		static HRESULT GetSharedHandle(IUnknown *pUnknown, HANDLE * pHandle);

		static HRESULT InitD3D9(HWND hWnd);
	};
}}}
