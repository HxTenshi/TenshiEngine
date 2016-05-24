#include "RenderingSystem.h"

template <>
void RenderingStateSetting<BlendState>::Initialize(){

	auto Desc = CDescType(CD3D11_DEFAULT());

	Desc.AlphaToCoverageEnable = FALSE;
	// TRUEの場合、マルチレンダーターゲットで各レンダーターゲットのブレンドステートの設定を個別に設定できる
	// FALSEの場合、0番目のみが使用される
	Desc.IndependentBlendEnable = FALSE;
	Desc.RenderTarget[0].BlendEnable = TRUE;

	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	_Create(Desc, PresetType::BS_Alpha);

	Desc.IndependentBlendEnable = TRUE;
	_Create(Desc, PresetType::BS_Alpha_RT1);

	Desc.IndependentBlendEnable = FALSE;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	_Create(Desc, PresetType::BS_Add);


}

template <>
void RenderingStateSetting<DepthStencil>::Initialize(){

	auto Desc = CDescType(CD3D11_DEFAULT());

	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	_Create(Desc, PresetType::DS_Zero_Alawys);

	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Desc.DepthFunc = D3D11_COMPARISON_LESS;
	_Create(Desc, PresetType::DS_Zero_Less);

	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	_Create(Desc, PresetType::DS_All_Alawys);

	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Desc.DepthFunc = D3D11_COMPARISON_LESS;
	_Create(Desc, PresetType::DS_All_Less);

}
template <>
void RenderingStateSetting<Rasterizer>::Initialize(){

	auto Desc = CDescType(CD3D11_DEFAULT());

	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_SOLID;
	_Create(Desc, PresetType::RS_None_Solid);

	Desc.CullMode = D3D11_CULL_BACK;
	Desc.FillMode = D3D11_FILL_WIREFRAME;
	_Create(Desc, PresetType::RS_Back_Wireframe);

	Desc.CullMode = D3D11_CULL_BACK;
	Desc.FillMode = D3D11_FILL_SOLID;
	_Create(Desc, PresetType::RS_Back_Solid);

	Desc.CullMode = D3D11_CULL_FRONT;
	Desc.FillMode = D3D11_FILL_SOLID;
	_Create(Desc, PresetType::RS_Front_Solid);

	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_SOLID;
	Desc.ScissorEnable = TRUE;
	_Create(Desc, PresetType::RS_None_Solid_Rect);
}


//static
IRenderingEngine* RenderingEngine::GetEngine(ContextType type){
	return (IRenderingEngine*)RenderingSystem::Instance()->GetEngine(type);
}