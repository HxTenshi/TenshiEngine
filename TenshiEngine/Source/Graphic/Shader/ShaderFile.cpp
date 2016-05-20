
#include <d3dcompiler.h>
//#include <d3d11.h>
//#include <d3dx11.h>
//#include <D3DX11async.h>

#include "ShaderFile.h"

#pragma comment(lib,"D3D11.lib")
#pragma comment(lib,"D3DCompiler.lib")


#include <string>
//マルチバイト文字列からワイド文字列
//ロケール依存
void widen2(const std::string &src, std::wstring &dest) {
	wchar_t *wcs = new wchar_t[src.length() + 1];
	mbstowcs(wcs, src.c_str(), src.length() + 1);
	dest = wcs;
	delete[] wcs;
}

	//--------------------------------------------------------------------------------------
	// Helper for compiling shaders with D3DX11
	//--------------------------------------------------------------------------------------
HRESULT ShaderFile::CompileShaderFromFile(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	dwShaderFlags |= D3DCOMPILE_DEBUG;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	std::wstring fname;
	widen2(szFileName, fname);

	ID3DBlob* pErrorBlob = NULL;
	hr = D3DCompileFromFile(
		fname.c_str(),
		NULL,	// macro
		NULL,	// include
		szEntryPoint,
		szShaderModel,
		dwShaderFlags,
		0,
		ppBlobOut,
		&pErrorBlob
		);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		*ppBlobOut = NULL;
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}