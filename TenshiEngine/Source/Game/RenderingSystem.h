#pragma once

#include "Device/Directx11Device.h"

#include <vector>
#include <stack>



class DepthStencil{
public:
	enum class Preset{
		//D3D11_DEPTH_WRITE_MASK_ZERO
		//D3D11_COMPARISON_ALWAYS
		DS_Zero_Alawys,

		//D3D11_DEPTH_WRITE_MASK_ZERO
		//D3D11_COMPARISON_LESS_EQUAL
		DS_Zero_LessEqual,

		//D3D11_DEPTH_WRITE_MASK_ALL
		//D3D11_COMPARISON_ALWAYS
		DS_All_Alawys,

		//D3D11_DEPTH_WRITE_MASK_ALL
		//D3D11_COMPARISON_LESS_EQUAL
		DS_All_LessEqual,
		Count,
	};

	using CDescType = CD3D11_DEPTH_STENCIL_DESC;
	using DescType = D3D11_DEPTH_STENCIL_DESC;
	using StateType = ID3D11DepthStencilState*;
	using PresetType = Preset;

	//typedef typename CD3D11_DEPTH_STENCIL_DESC CDescType;
	//typedef typename D3D11_DEPTH_STENCIL_DESC DescType;
	//typedef typename ID3D11DepthStencilState* StateType;
	//typedef typename Preset PresetType;
	
	static void CreateState(DescType& desc, StateType* state){
		Device::mpd3dDevice->CreateDepthStencilState(&desc, state);
	}
	static void Set(ID3D11DeviceContext* context, StateType state){
		context->OMSetDepthStencilState(state, 0);
	}
	DescType Desc;
	StateType State;
};

class Rasterizer{
public:
	enum class Preset{
		//D3D11_CULL_NONE;
		//D3D11_FILL_SOLID;
		RS_None_Solid,

		//D3D11_CULL_BACK;
		//D3D11_FILL_WIREFRAME;
		RS_Back_Wireframe,

		//D3D11_CULL_BACK;
		//D3D11_FILL_SOLID;
		RS_Back_Solid,

		//D3D11_CULL_FRONT
		//D3D11_FILL_SOLID;
		RS_Front_Solid,

		//D3D11_CULL_NONE;
		//D3D11_FILL_SOLID;
		//ScissorEnable = TRUE;
		RS_None_Solid_Rect,

		Count,
	};

	using CDescType = CD3D11_RASTERIZER_DESC;
	using DescType = D3D11_RASTERIZER_DESC;
	using StateType = ID3D11RasterizerState*;
	using PresetType = Preset;

	static void CreateState(DescType& desc, StateType* state){
		Device::mpd3dDevice->CreateRasterizerState(&desc, state);
	}
	static void Set(ID3D11DeviceContext* context, StateType state){
		context->RSSetState(state);
	}
	DescType Desc;
	StateType State;
};


template <class T>
class RenderingStateSetting{
public:

	typedef typename T TemplateType;
	typedef typename TemplateType::CDescType CDescType;
	typedef typename TemplateType::DescType DescType;
	typedef typename TemplateType::StateType StateType;
	typedef typename TemplateType::PresetType PresetType;

	RenderingStateSetting(ID3D11DeviceContext* context){
		mDeviceContext = context;
		mDataBase.resize((int)PresetType::Count);
	}

	~RenderingStateSetting(){
	}

	void Initialize();

	void Release(){

		for (TemplateType& state : mDataBase){
			state.State->Release();
		}
	}

	void Push(const PresetType& preset){

		mStack.push(mDataBase[(int)preset]);
		_TopSet();
	}

	void Pop(){
		mStack.pop();
		_TopSet();
	}

private:

	void _TopSet(){
		if (mStack.size() == 0){
			TemplateType::Set(mDeviceContext, (StateType)NULL);
			return;
		}

		TemplateType& top = mStack.top();
		TemplateType::Set(mDeviceContext, top.State);

	}

	void _Create(DescType desc, PresetType id){
		TemplateType ds;
		ds.Desc = desc;
		TemplateType::CreateState(ds.Desc, &ds.State);
		mDataBase[(int)id] = ds;
	}
	ID3D11DeviceContext* mDeviceContext;
	std::vector<TemplateType> mDataBase;
	std::stack<TemplateType> mStack;
};

class IRenderingEngine{
public:
	virtual ~IRenderingEngine(){}

	virtual void PushSet(DepthStencil::Preset preset) = 0;
	virtual void PopDS() = 0;
	virtual void PushSet(Rasterizer::Preset preset) = 0;
	virtual void PopRS() = 0;

	ID3D11DeviceContext* const m_Context;

protected:
	void _Release(){
		m_Context->Release();
	}
	IRenderingEngine(ID3D11DeviceContext *context)
		:m_Context(context){

	}
};
enum class ContextType{

	MainDeferrd,
	Count,

};

class RenderingEngine : public IRenderingEngine {
public:
	RenderingEngine(ID3D11DeviceContext *context)
		:IRenderingEngine(context)
		, mDSSetting(context)
		, mRSSetting(context)
	{

		mDSSetting.Initialize();
		mRSSetting.Initialize();
	}
	~RenderingEngine(){

	}

	static IRenderingEngine* GetEngine(ContextType type);

	void Release(){
		mDSSetting.Release();
		mRSSetting.Release();

		IRenderingEngine::_Release();
	}

	void PushSet(DepthStencil::Preset preset) override{
		mDSSetting.Push(preset);
	}
	void PopDS() override{
		mDSSetting.Pop();
	}
	void PushSet(Rasterizer::Preset preset) override{
		mRSSetting.Push(preset);
	}
	void PopRS() override{
		mRSSetting.Pop();
	}
private:

	RenderingStateSetting<DepthStencil> mDSSetting;
	RenderingStateSetting<Rasterizer> mRSSetting;
};


class RenderingSystem{
public:
	static RenderingSystem* Instance(){
		static RenderingSystem temp;
		return &temp;
	}

	void PushEngine(RenderingEngine* engine, ContextType type){
		m_RenderList[(int)type] = engine;
	}

	RenderingEngine* GetEngine(ContextType type) const{
		return m_RenderList[(int)type];
	}

	void Release(){
		for (auto e : m_RenderList){
			e->Release();
			delete e;
		}
	}

private:
	RenderingSystem()
		:m_RenderList((int)ContextType::Count)
	{
	}

	std::vector<RenderingEngine*> m_RenderList;
};