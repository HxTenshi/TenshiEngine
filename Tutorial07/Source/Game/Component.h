#pragma once


#include "MySTL/Deleter.h"
#include "MySTL/Ptr.h"

#include <d3d11.h>
#include <xnamath.h>

#include "Actor.h"
#include "../Graphic/Model/Model.h"
#include "Window/Window.h"
#include "../Input/Input.h"
#include "Window/InspectorWindow.h"
#include "MySTL/File.h"

#include "IComponent.h"
#include "TransformComponent.h"
#include "PhysXColliderComponent.h"

class ModelComponent :public Component{
public:
	ModelComponent(const std::string& FileName,shared_ptr<MaterialComponent> resultMaterial):
		mFileName(FileName){

		mModel = new Model();
		mModel->Create(FileName.c_str(), resultMaterial);
	}
	ModelComponent(){
		mModel = NULL;
	}
	virtual ~ModelComponent(){
		if (mModel){
			mModel->Release();
			delete mModel;
		}
	}
	virtual void Update() override{

	}

	void SetMatrix(){
		if (!mModel)return;
		mModel->mWorld = gameObject->mTransform->GetMatrix();
		mModel->Update();
	}

	virtual bool HitChack(const XMVECTOR& pos, const XMVECTOR& vect){
		XMVECTOR p;
		p.x = mModel->mWorld._41;
		p.y = mModel->mWorld._42;
		p.z = mModel->mWorld._43;
		p.w = mModel->mWorld._44;
		XMVECTOR v = p - pos;
		XMVECTOR lv = XMVector3Length(v);
		float l = XMVectorGetX(lv);

		if (mModel->CheckHitPoint(pos + vect*l)){
			return true;
		}
		return false;
	}

	void CreateInspector() override{
		auto data = Window::CreateInspector();
		Window::AddInspector(new InspectorLabelDataSet("Model"), data);
		Window::AddInspector(new InspectorLabelDataSet(mFileName), data);
		Window::ViewInspector(data);
	}

	void ExportData(File& f) override{
		ExportClassName(f);
		f.Out(mFileName);
	}
	void ImportData(File& f) override{
		f.In(&mFileName);
		mModel = new Model();
		mModel->Create(mFileName.c_str(), shared_ptr<MaterialComponent>());
	}

	Model* mModel;
	std::string mFileName;

};

class TextureModelComponent : public ModelComponent{
public:
	TextureModelComponent(const char* FileName, shared_ptr<MaterialComponent> resultMaterial)
		:ModelComponent(){
		mModel = new ModelTexture();
		mModel->Create(FileName, resultMaterial);
	}
	TextureModelComponent(){
		mModel = NULL;
	}
	~TextureModelComponent(){
	}

	//void Update(Actor* mThis) override{
	//	if (!mTransform){
	//		mTransform = mThis->GetComponent<TransformComponent>();
	//		if (!mTransform)return;
	//	}
	//
	//	if (!mModel)return;
	//
	//	//mModel->mWorld = mTransform->mMatrix;
	//	//mModel->Update();
	//}

	bool HitChack(const XMVECTOR& pos, const XMVECTOR& vect) override{
		XMVECTOR p;
		p.x = mModel->mWorld.m[3][0];
		p.y = mModel->mWorld.m[3][3];
		p.z = mModel->mWorld.m[3][2];
		p.w = mModel->mWorld.m[3][1];

		p.x = p.x * WindowState::mWidth;
		p.y = (1 - p.y) * WindowState::mHeight;

		p.z = p.z * WindowState::mWidth;
		p.w = (1 - p.w) * WindowState::mHeight;

		int x, y;
		Input::MousePosition(&x, &y);

		if (p.x <= x && p.y <= y &&
			p.z >= x && p.w >= y){
			return true;
		}
		return false;
	}

	void CreateInspector() override{
		auto data = Window::CreateInspector();
		Window::AddInspector(new InspectorLabelDataSet("TextureModel"), data);
		Window::ViewInspector(data);
	}

	void ExportData(File& f) override{
		ExportClassName(f);
	}
	void ImportData(File& f) override{
		mModel = new ModelTexture();
		mModel->Create("", shared_ptr<MaterialComponent>());
	}

};

class MaterialComponent :public Component{
public:

	Material LoadAssetResource(const std::string& path){

		Material material;
		auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);
		auto hr = material.Create(cbm, cbt);
		if (FAILED(hr))
			return Material();
		File f(path);
		if (!f)return Material();
		std::string name;
		f.In(&name);
		int num;
		f.In(&num);
		for (int i = 0; i < num; i++){
			int slot;
			f.In(&slot);
			std::string filename;
			f.In(&filename);
			int ioc = filename.find("$");
			while (std::string::npos != ioc){
				filename.replace(ioc, 1, " ");
				ioc = filename.find("$");
			}

			material.SetTexture(filename.c_str(), slot);
		}
		return material;
	}

	MaterialComponent(){
		mThis = NULL;
		mMaterials = new std::vector<Material>();
		mMaterials->resize(1);
		mppMaterials = &mMaterials;
		mUseDelete = true;
	}
	~MaterialComponent(){
		if (mUseDelete)
			delete mMaterials;
	}

	void SetMaterial(UINT SetNo,Material& material){
		if ((**mppMaterials).size() <= SetNo)(**mppMaterials).resize(SetNo+1);
		(**mppMaterials)[SetNo] = material;

	}
	Material GetMaterial(UINT GetNo) const{
		if (!mMaterials){
			return Material();
		}
		if (mMaterials->size()<=GetNo){
			return Material();
		}
		return (**mppMaterials)[GetNo];
	}
	std::string TextureName = "";
	void CreateInspector() override{

		auto data = Window::CreateInspector();
		Window::AddInspector(new InspectorLabelDataSet("Material"), data);
		std::function<void(float)> collbackx = [&](float f){
			(**mppMaterials)[0].mCBMaterial.mParam.Diffuse.x = f;
		};

		std::function<void(float)> collbacky = [&](float f){
			(**mppMaterials)[0].mCBMaterial.mParam.Diffuse.y = f;
		};

		std::function<void(float)> collbackz = [&](float f){
			(**mppMaterials)[0].mCBMaterial.mParam.Diffuse.z = f;
		};
		std::function<void(std::string)> collbacktex = [&](std::string name){
			(**mppMaterials)[0].SetTexture(name.c_str(),0);
			TextureName = name;
		};
		std::function<void(std::string)> collbackpath = [&](std::string name){
			mMaterialPath = name;
			(**mppMaterials)[0] = LoadAssetResource(mMaterialPath);
			TextureName = (**mppMaterials)[0].mTexture[0].mFileName;
		};
		Window::AddInspector(new InspectorStringDataSet("Material", &mMaterialPath, collbackpath), data);
		Window::AddInspector(new InspectorSlideBarDataSet("r", 0.0f, 1.0f, &(**mppMaterials)[0].mCBMaterial.mParam.Diffuse.x, collbackx), data);
		Window::AddInspector(new InspectorSlideBarDataSet("g", 0.0f, 1.0f, &(**mppMaterials)[0].mCBMaterial.mParam.Diffuse.y, collbacky), data);
		Window::AddInspector(new InspectorSlideBarDataSet("b", 0.0f, 1.0f, &(**mppMaterials)[0].mCBMaterial.mParam.Diffuse.z, collbackz), data);
		Window::AddInspector(new InspectorStringDataSet("Textre", &TextureName, collbacktex), data);
		Window::ViewInspector(data);
	}

	void ExportData(File& f) override{
		ExportClassName(f);
		int num = mMaterials->size();
		f.Out(num);
		f.Out(mMaterialPath);
		//for (auto& m : *mMaterials){
		//	m.ExportData(f);
		//}
	}
	void ImportData(File& f) override{
		int num;
		f.In(&num);
		mMaterials->resize(num);
		f.In(&mMaterialPath);
		for (auto& m : *mMaterials){
			m = LoadAssetResource(mMaterialPath);
			if (!m.mCBUseTexture.mBuffer){
				auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
				auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);
				m.Create(cbm, cbt);
			}
			else{
				TextureName = m.mTexture[0].mFileName;
			}
		}
		//for (auto& m : *mMaterials){
		//	f.In(&num);
		//	auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		//	auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);
		//	m.Create(cbm, cbt);
		//	for (int i = 0; i < num; i++){
		//		int slot;
		//		f.In(&slot);
		//		std::string filename;
		//		f.In(&filename);
		//		int ioc = filename.find("$");
		//		while (std::string::npos != ioc){
		//			filename.replace(ioc, 1, " ");
		//			ioc = filename.find("$");
		//		}
		//
		//		m.SetTexture(filename.c_str(), slot);
		//	}
		//}
	}

	bool mUseDelete;
	std::vector<Material>** mppMaterials;
	std::vector<Material>* mMaterials;
	Actor* mThis;
	std::string mMaterialPath;
};

class AnimetionComponent :public Component{
public:
	AnimetionComponent()
		:mTime(148.0f){}
	void Update() override{

		if (!mModel){
			mModel = gameObject->GetComponent<ModelComponent>();
			if (!mModel)return;
		}
		if (mModel->mModel){

			mTime += 1.0f / 2.0f;
			mModel->mModel->PlayVMD(mTime);
		}
	}
	void CreateInspector() override{
		auto data = Window::CreateInspector();
		Window::AddInspector(new InspectorLabelDataSet("Animetion"), data);

		std::function<void(float)> collback = [&](float f){
			mTime = f;
		};
		Window::AddInspector(new InspectorFloatDataSet("Time",&mTime,collback), data);

		Window::ViewInspector(data);
	}

	void ExportData(File& f) override{
		ExportClassName(f);
	}
	void ImportData(File& f) override{
	}
	
	float mTime;
	weak_ptr<ModelComponent> mModel;
};

class MeshDrawComponent :public Component{
public:
	MeshDrawComponent()
		:mWriteDepth(true){
	}

	void Update() override;

	void NoWriteDepth(Model& model){

		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		descDS.DepthFunc = D3D11_COMPARISON_LESS;
		ID3D11DepthStencilState* pDS = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
		Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);

		model.Draw(mMaterial);

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	}

	void CreateInspector() override{
		auto data = Window::CreateInspector();
		Window::AddInspector(new InspectorLabelDataSet("MeshRender"), data);
		Window::ViewInspector(data);
	}

	void ExportData(File& f) override{
		ExportClassName(f);
	}
	void ImportData(File& f) override{
	}

	weak_ptr<ModelComponent> mModel;
	weak_ptr<MaterialComponent> mMaterial;
	bool mWriteDepth;

};

#include "Graphic/Shader/GeometryShader.h"
class ParticleComponent :public Component{
public:
#define NUM_PARTICLE 200
	struct ParticleVertex{
		XMFLOAT3 pos;
		XMFLOAT3 v0;
		FLOAT time;

	};

	ParticleComponent()
		:mFirstDraw(true){
		mInitialize = false;
		UINT stride[] = { sizeof(ParticleVertex) };

		D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // 位置座標
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // 初速度
				{ "TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }   // 時間
		};

		D3D11_SO_DECLARATION_ENTRY decl[] = {
				{ 0, "POSITION", 0, 0, 3, 0 },
				{ 0, "NORMAL", 0, 0, 3, 0 },
				{ 0, "TEXCOORD", 0, 0, 1, 0 },
		};
		mGeometryShader0.Create("ParticleGS.fx", "GS0_Main", decl, _countof(decl), stride, _countof(stride));
		mGeometryShader1.Create("ParticleGS.fx", "GS1_Main");
		mVertexShader.Create("ParticleGS.fx", "VS0_Main", layout, 3);
		mPixelShader.Create("ParticleGS.fx", "PS1_Main");


		// パーティクルの初期値を設定
		ParticleVertex v[NUM_PARTICLE];
		for (int i = 0; i<NUM_PARTICLE; i++)
		{
			v[i].pos = XMFLOAT3(0.5f, 0.5f, 0.0f);
			v[i].v0 = XMFLOAT3((float)(rand() % 50 - 25) * 0.1f, (float)(rand() % 50) * 0.3f, (float)(rand() % 50 - 25) * 0.1f);
			v[i].time = (float)(rand() % 100) * 0.1f;
		};

		// ターゲット入出力バッファーを作成
		D3D11_BUFFER_DESC BufferDesc;
		::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
		BufferDesc.ByteWidth = sizeof(v);  // バッファサイズ
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		BufferDesc.CPUAccessFlags = 0;
		BufferDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA mappedResource;
		mappedResource.pSysMem = v;  // バッファ・データの初期値
		mappedResource.SysMemPitch = 0;
		mappedResource.SysMemSlicePitch = 0;
		for (int i = 0; i<2; i++)
		{
			auto hr = Device::mpd3dDevice->CreateBuffer(&BufferDesc, &mappedResource, &mpSOBuffer[i]);
			if (FAILED(hr))
				return;
		}
		mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
		if (!mCBuffer.mBuffer)
			return;

		mInitialize = true;
	}
	~ParticleComponent(){
		mGeometryShader0.Release();
		mGeometryShader1.Release();
		mpSOBuffer[0]->Release();
		mpSOBuffer[1]->Release();
	}
	void Update() override{
		if (!mInitialize)return;
		if (!mMaterial){
			mMaterial = gameObject->GetComponent<MaterialComponent>();
			if (!mMaterial)return;
		}



		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

		mMaterial->GetMaterial(0).PSSetShaderResources();

		// ターゲット出力バッファーを入れ替える
		ID3D11Buffer *pB = mpSOBuffer[0];
		mpSOBuffer[0] = mpSOBuffer[1];
		mpSOBuffer[1] = pB;

		// 入力アセンブラー ステージに頂点バッファーの配列をバインドする。
		UINT stride = sizeof(ParticleVertex);
		UINT offset = 0;
		Device::mpImmediateContext->IASetVertexBuffers(0, 1, &mpSOBuffer[0], &stride, &offset);

		Device::mpImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		// ピクセルシェーダーを無効にした状態で深度ステンシルを有効にしているとエラーになるので無効にする
		//ID3D11DepthStencilState*  pDepthStencilState = NULL;
		//D3D11_DEPTH_STENCIL_DESC ddsDesc;
		//UINT StencilRef;
		//::ZeroMemory(&ddsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		//Device::mpImmediateContext->OMGetDepthStencilState(&pDepthStencilState, &StencilRef);
		//pDepthStencilState->GetDesc(&ddsDesc);
		//ddsDesc.DepthEnable = FALSE;  // 深度テストを無効にする
		//ddsDesc.StencilEnable = FALSE;
		//pDepthStencilState->Release();
		//Device::mpd3dDevice->CreateDepthStencilState(&ddsDesc, &pDepthStencilState);
		//Device::mpImmediateContext->OMSetDepthStencilState(pDepthStencilState, StencilRef);
		//pDepthStencilState->Release();

		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		descDS.DepthFunc = D3D11_COMPARISON_LESS;
		descDS.DepthEnable = FALSE;
		descDS.StencilEnable = FALSE;

		ID3D11DepthStencilState* pDS = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
		if (!pDS)return;

		Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);


		//Device::mpImmediateContext->OMSetDepthStencilState(NULL, NULL);

		mVertexShader.SetShader();

		mGeometryShader0.SetShader();

		// ピクセルシェーダーをデバイスに無効にする。
		Device::mpImmediateContext->PSSetShader(NULL, NULL, 0);

		//Device::mpImmediateContext->VSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->HSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->DSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->GSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->PSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->CSSetShader(NULL, NULL, 0);

		// 出力バッファーの配列のオフセット値
		// パイプラインのストリーム出力ステージで使用されるターゲット出力バッファーを設定します。
		Device::mpImmediateContext->SOSetTargets(1, &mpSOBuffer[1], &offset);


		mCBuffer.mParam.mWorld = gameObject->mTransform->GetMatrix();
		mCBuffer.UpdateSubresource();
		mCBuffer.GSSetConstantBuffers();

		// 最初は頂点数がわかるので ID3D11DeviceContext::Draw() で描画する
		if (mFirstDraw)
		{
			Device::mpImmediateContext->Draw(NUM_PARTICLE, 0);
			mFirstDraw = false;
		}
		// ２回目移行は処理する頂点数が不明なので ID3D11DeviceContext::DrawAuto() で描画する
		else
		{
			Device::mpImmediateContext->DrawAuto();
		}

		ID3D11Buffer* pNullBuffer[] = { NULL };
		// パイプラインのストリーム出力ステージで使用されるターゲット出力バッファーを無効にする。
		Device::mpImmediateContext->SOSetTargets(1, pNullBuffer, NULL);


		mGeometryShader1.SetShader();
		mPixelShader.SetShader();

		Device::mpImmediateContext->DrawAuto();

		Device::mpImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Device::mpImmediateContext->GSSetShader(NULL, NULL, 0);

		pDS->Release();


		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	}
	void CreateInspector() override{

		auto data = Window::CreateInspector();
		Window::AddInspector(new InspectorLabelDataSet("Particle"), data);
		Window::ViewInspector(data);
	}

	void ExportData(File& f) override{
		ExportClassName(f);
	}
	void ImportData(File& f) override{
	}


	bool mInitialize;
	GeometryStreamOutputShader mGeometryShader0;
	GeometryStreamOutputShader mGeometryShader1;
	VertexShader	mVertexShader;
	PixelShader		mPixelShader;
	ID3D11Buffer*	mpSOBuffer[2];

	weak_ptr<MaterialComponent> mMaterial;
	ConstantBuffer<CBChangesEveryFrame> mCBuffer;

	bool mFirstDraw;

};

class IDllScriptComponent;
class ScriptComponent : public Component{
public:
	ScriptComponent();
	~ScriptComponent();

	void Load();
	void Unload();
	void ReCompile();
	void Update() override;

	void CreateInspector() override{

		auto data = Window::CreateInspector();
		Window::AddInspector(new InspectorLabelDataSet("Script"), data);
		Window::ViewInspector(data);
	}

	void ExportData(File& f) override{
		ExportClassName(f);
	}
	void ImportData(File& f) override{
	}

	HMODULE hModule;
	IDllScriptComponent* pDllClass;

	void* mCreate;
	void* mDelete;
};

class ComponentFactory{

public:
	static
	shared_ptr<Component> Create(const std::string& ClassName){
		if (!mIsInit){
			Init();
		}
		
		return _Create(("class " + ClassName));

	}

private:
	static
	void Init(){
		_NewFunc<TransformComponent>();
		_NewFunc<ModelComponent>();
		_NewFunc<TextureModelComponent>();
		_NewFunc<MaterialComponent>();
		_NewFunc<AnimetionComponent>();
		_NewFunc<MeshDrawComponent>();
		_NewFunc<ParticleComponent>();
		_NewFunc<ScriptComponent>();
		_NewFunc<PhysXComponent>();
		_NewFunc<PhysXColliderComponent>();
		mIsInit = true;
	}

	template<class T>
	static
	void _NewFunc(){
		mFactoryComponents[typeid(T).name()] = [](){return new T(); };
	}

	static
	shared_ptr<Component> _Create(std::string ClassName){
		auto p = mFactoryComponents.find(ClassName);
		if (p != mFactoryComponents.end()){
			return shared_ptr<Component>((*p).second());
		}
		return shared_ptr<Component>();
	}

	static
		std::map<std::string, std::function<Component*()>> mFactoryComponents;
	static
	bool mIsInit;
};