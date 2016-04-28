//#include "RenderingSystem.h"
//
//#include "Device/Directx11Device.h"
//
//#include <vector>
//#include <stack>
//
//struct DepthStencil{
//	D3D11_DEPTH_STENCIL_DESC Desc;
//	ID3D11DepthStencilState* State;
//};
//
//
//enum class DepthStencilType{
//	//D3D11_DEPTH_WRITE_MASK_ZERO
//	//D3D11_COMPARISON_ALWAYS
//	DS_Zero_Alawys,
//
//	//D3D11_DEPTH_WRITE_MASK_ZERO
//	//D3D11_COMPARISON_LESS_EQUAL
//	DS_Zero_LessEqual,
//	Count,
//};
//
//template <class T>
//class StateSetting{
//public:
//
//	StateSetting(){
//
//	}
//
//	~StateSetting(){
//
//	}
//
//	void Initialize();
//
//	void Release(){
//
//	}
//
//	void Push(){
//
//		mStack.push();
//	}
//
//	void Pop(){
//		mStack.pop();
//	}
//
//private:
//	std::vector<T> mDataBase;
//	std::stack<T> mStack;
//};
//
//
//void StateSetting<DepthStencil>::Initialize(){
//
//	auto create = [&](D3D11_DEPTH_STENCIL_DESC& desc){
//
//		DepthStencil ds;
//		ds.Desc = desc;
//		Device::mpd3dDevice->CreateDepthStencilState(&ds.Desc, &ds.State);
//		mDataBase.push_back(ds);
//	};
//
//
//	auto Desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
//
//	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//	Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
//	create(Desc);
//
//	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//	Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//	create(Desc);
//
//
//
//}
//
//void c(){
//
//	//std::vector<DepthStencil> mDataBase;
//	//std::stack<DepthStencil> mStack;
//	//
//	//ID3D11DepthStencilState* pBackDS;
//	//UINT ref;
//	//Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);
//	//
//	//D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
//	//descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//	//descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
//	//
//	//ID3D11DepthStencilState* pDS_tex = NULL;
//	//Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
//	//Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);
//	//
//	//
//	//D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
//	//descRS.CullMode = D3D11_CULL_NONE;
//	//descRS.FillMode = D3D11_FILL_SOLID;
//	//
//	//ID3D11RasterizerState* pRS = NULL;
//	//Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);
//	//
//	//Device::mpImmediateContext->RSSetState(pRS);
//	//
//	//mModelTexture.Draw(mMaterialDebugDraw);
//	//
//	//Device::mpImmediateContext->RSSetState(NULL);
//	//if (pRS)pRS->Release();
//	//
//	//Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
//	//pDS_tex->Release();
//	//
//	//Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
//	//if (pBackDS)pBackDS->Release();
//
//
//
//}