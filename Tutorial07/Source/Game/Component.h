#pragma once


#include "MySTL/Deleter.h"
#include "MySTL/Ptr.h"

#include <d3d11.h>
#include "XNAMath/xnamath.h"

#include "Actor.h"
#include "../Graphic/Model/Model.h"
#include "Window/Window.h"
#include "../Input/Input.h"
#include "MySTL/File.h"

#include "IComponent.h"
#include "TransformComponent.h"
#include "PhysXColliderComponent.h"
#include "ScriptComponent.h"
#include "TextComponent.h"

#include "Graphic/Font/Font.h"
class EditorCamera;

class CameraComponent :public Component{
public:
	CameraComponent(){}
	~CameraComponent(){}

	void Initialize() override
	{

		mCBNeverChanges = ConstantBuffer<CBNeverChanges>::create(0);
		mCBChangeOnResize = ConstantBuffer<CBChangeOnResize>::create(1);
		mCBBillboard = ConstantBuffer<CBBillboard>::create(8);

		// Initialize the view matrix
		Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
		At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		UpdateView();


		UINT width = WindowState::mWidth;
		UINT height = WindowState::mHeight;
		// Initialize the projection matrix
		mProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);

		mCBChangeOnResize.mParam.mProjection = XMMatrixTranspose(mProjection);
		XMVECTOR null;
		mCBChangeOnResize.mParam.mProjectionInv = XMMatrixTranspose(XMMatrixInverse(&null, mProjection));
		mCBChangeOnResize.UpdateSubresource();

	}

	void Update() override;

	void CreateInspector() override{
		auto data = Window::CreateInspector();
		Window::ViewInspector("Camera",this, data);
	}

	void IO_Data(I_ioHelper* io) override{
		(void)io;
#define _KEY(x) io->func( x , #x)

#undef _KEY
	}

	void VSSetConstantBuffers() const
	{
		mCBNeverChanges.VSSetConstantBuffers();
		mCBChangeOnResize.VSSetConstantBuffers();
		mCBBillboard.VSSetConstantBuffers();
	}
	void PSSetConstantBuffers() const
	{
		mCBNeverChanges.PSSetConstantBuffers();
		mCBChangeOnResize.PSSetConstantBuffers();
		mCBBillboard.PSSetConstantBuffers();
	}
	void GSSetConstantBuffers() const
	{
		mCBNeverChanges.GSSetConstantBuffers();
		mCBChangeOnResize.GSSetConstantBuffers();
		mCBBillboard.GSSetConstantBuffers();
	}

	const XMMATRIX& GetViewMatrix(){
		return mView;
	}

private:

	void UpdateView(){
		mView = XMMatrixLookAtLH(Eye, At, Up);
		
		
		auto Billboard = XMMatrixLookAtLH(XMVectorZero(), Eye - At, Up);
		XMVECTOR null;
		Billboard = XMMatrixTranspose(XMMatrixInverse(&null, Billboard));
		mCBBillboard.mParam.mBillboardMatrix = Billboard;
		mCBBillboard.UpdateSubresource();
		
		mCBNeverChanges.mParam.mView = XMMatrixTranspose(mView);
		mCBNeverChanges.mParam.mViewInv = XMMatrixTranspose(XMMatrixInverse(&null, mView));
		mCBNeverChanges.UpdateSubresource();
		
		//gameObject->mTransform->Position(Eye);
		
		//XMVECTOR zaxis = XMVector3Normalize(At - Eye);
		//XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(Up, zaxis));
		//XMVECTOR yaxis = XMVector3Cross(zaxis, xaxis);
		//
		//float yaw = 0.0f;
		//float roll = 0.0f;
		//float pitch = asin(mView._31);
		//if (cos(pitch) == 0.0f){
		//	yaw = atan2(mView._23, mView._22);
		//}
		//else{
		//	float roll = asin(-mView._21 / cos(pitch));
		//	if (mView._11 < 0) roll = 180 - roll;
		//
		//	yaw = atan2(-mView._32, mView._33);
		//}
		
		//gameObject->mTransform->Rotate(XMVectorSet(roll, pitch, yaw, 1.0f));
	}

	ConstantBuffer<CBNeverChanges>		mCBNeverChanges;
	ConstantBuffer<CBChangeOnResize>	mCBChangeOnResize;
	ConstantBuffer<CBBillboard>			mCBBillboard;

	XMMATRIX                            mView;
	XMMATRIX                            mProjection;
	XMVECTOR							Eye;
	XMVECTOR							At;
	XMVECTOR							Up;

	friend EditorCamera;
};

class ModelComponent :public Component{
public:
	ModelComponent(){
		mModel = NULL;
	}
	virtual ~ModelComponent(){
		if (mModel){
			mModel->Release();
			delete mModel;
		}
	}
	virtual void Initialize() override{
		if (!mModel){
			mModel = new Model();
		}
		if (!mModel->IsCreate()){
			mModel->Create(mFileName.c_str());
		}
	}
	virtual void Update() override{

	}

	void SetMatrix(){
		if (!mModel)return;
		mModel->mWorld = gameObject->mTransform->GetMatrix();
		mModel->Update();
	}

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override{
#define _KEY(x) io->func( x , #x)
		_KEY(mFileName);
#undef _KEY
	}

	Model* mModel;
	std::string mFileName;

};

class TextureModelComponent : public ModelComponent{
public:
	TextureModelComponent(){
		mModel = NULL;
	}
	~TextureModelComponent(){
		//親で開放してる
		//if (mModel)mModel->Release();
	}

	void Initialize() override{
		if (!mModel){
			mModel = new ModelTexture();
		}
		if (!mModel->IsCreate()){
			mModel->Create("");
		}
	}


	void CreateInspector() override{
		auto data = Window::CreateInspector();
		Window::ViewInspector("TextureModel", this, data);
	}

	void IO_Data(I_ioHelper* io) override{
		(void)io;
#define _KEY(x) io->func( x , #x)
#undef _KEY
	}

};

class MaterialComponent :public Component{
public:

	void LoadAssetResource(const std::string& path){

		File f(path);
		if (!f)return;

		mMaterials.clear();

		std::string name;
		f.In(&name);
		int materialnum;
		f.In(&materialnum);
		mMaterials.resize(materialnum);
		for (int i = 0; i < materialnum; i++){
			auto& material = mMaterials[i];
			auto hr = material.Create();
			if (FAILED(hr))return;

			int texnum;
			f.In(&texnum);
			for (int i = 0; i < texnum; i++){
				int slot;
				f.In(&slot);
				std::string filename;
				f.In(&filename);
				auto ioc = filename.find("$");
				while (std::string::npos != ioc){
					filename.replace(ioc, 1, " ");
					ioc = filename.find("$");
				}

				material.SetTexture(filename.c_str(), slot);
			}
		}
	}
	void SaveAssetResource(const std::string& path){

		File f;
		if (!f.Open(path))
			f.FileCreate();
		f.Clear();
		if (!f)return;
		std::string name = "Material";
		f.Out(name);
		int num = mMaterials.size();
		f.Out(num);
		for (int i = 0; i < num; i++){
			Material &mate = mMaterials[i];
			mate.ExportData(f);
		}
	}

	MaterialComponent(){
		mMaterials.resize(1);
	}
	~MaterialComponent(){
	}

	void Initialize() override{
		auto& mate = mMaterials[0];
		if (!mate.IsCreate()){
			if (mMaterialPath != ""){
				LoadAssetResource(mMaterialPath);
				for (auto& m : mMaterials){
					if (!m.IsCreate()){
						m.Create();
					}
				}
			}
			else{
				auto& m = mMaterials[0];
				m.Create();
			}
		}
	}

	void SetMaterial(UINT SetNo,Material& material){
		if (mMaterials.size() <= SetNo)mMaterials.resize(SetNo+1);
		mMaterials[SetNo] = material;

	}
	Material GetMaterial(UINT GetNo) const{
		if (mMaterials.size()<=GetNo){
			return Material();
		}
		return mMaterials[GetNo];
	}
	std::string TextureName = "";
	void CreateInspector() override{

		auto data = Window::CreateInspector();
		std::function<void(float)> collbackx = [&](float f){
			mMaterials[0].mCBMaterial.mParam.Diffuse.x = f;
		};

		std::function<void(float)> collbacky = [&](float f){
			mMaterials[0].mCBMaterial.mParam.Diffuse.y = f;
		};

		std::function<void(float)> collbackz = [&](float f){
			mMaterials[0].mCBMaterial.mParam.Diffuse.z = f;
		};
		std::function<void(std::string)> collbacktex = [&](std::string name){
			mMaterials[0].SetTexture(name.c_str(),0);
			TextureName = name;
		};
		std::function<void(std::string)> collbackpath = [&](std::string name){
			mMaterialPath = name;
			LoadAssetResource(mMaterialPath);
			//TextureName = (**mppMaterials)[0].mTexture[0].mFileName;
		};
		Window::AddInspector(new InspectorStringDataSet("Material", &mMaterialPath, collbackpath), data);
		Window::AddInspector(new InspectorSlideBarDataSet("r", 0.0f, 1.0f, &mMaterials[0].mCBMaterial.mParam.Diffuse.x, collbackx), data);
		Window::AddInspector(new InspectorSlideBarDataSet("g", 0.0f, 1.0f, &mMaterials[0].mCBMaterial.mParam.Diffuse.y, collbacky), data);
		Window::AddInspector(new InspectorSlideBarDataSet("b", 0.0f, 1.0f, &mMaterials[0].mCBMaterial.mParam.Diffuse.z, collbackz), data);
		Window::AddInspector(new InspectorStringDataSet("Textre", &TextureName, collbacktex), data);
		Window::ViewInspector("Material", this, data);
	}

	void IO_Data(I_ioHelper* io) override{
#define _KEY(x) io->func( x , #x)
		_KEY(mMaterialPath);
#undef _KEY
	}

	std::vector<Material> mMaterials;
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

		std::function<void(float)> collback = [&](float f){
			mTime = f;
		};
		Window::AddInspector(new InspectorFloatDataSet("Time",&mTime,collback), data);

		Window::ViewInspector("Animetion", this, data);
	}


	void IO_Data(I_ioHelper* io) override{
		(void)io;
#define _KEY(x) io->func( x , #x)
#undef _KEY
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
		Window::ViewInspector("MeshRender", this, data);
	}

	void IO_Data(I_ioHelper* io) override{
		(void)io;
#define _KEY(x) io->func( x , #x)

#undef _KEY
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
		Window::ViewInspector("Particle", this, data);
	}

	void IO_Data(I_ioHelper* io) override{
		(void)io;
#define _KEY(x) io->func( x , #x)

#undef _KEY
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

#include "Graphic/RenderTarget/RenderTarget.h"
class PostEffectComponent :public Component{
public:
	PostEffectComponent()
	{
	}

	void Initialize() override;
	
	void Update() override{
		PostDraw();
	}

	void Finish() override;

	void PostDraw();

	void CreateInspector() override{
		auto data = Window::CreateInspector();
		Window::ViewInspector("PostEffect", this, data);
	}


	void IO_Data(I_ioHelper* io) override{
		(void)io;
#define _KEY(x) io->func( x , #x)

#undef _KEY
	}
private:

	RenderTarget mRenderTarget;

	ModelTexture mModelTexture;

	Material mMaterial;
	Material mMaterialEnd;
	
};

class PointLightComponent :public Component{
public:

	PointLightComponent()
	{
		//D3D11_INPUT_ELEMENT_DESC layout[] = {
		//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // 座標
		//};
		//mVertexShader.Create("DeferredPointLightRendering.fx", "VS", layout, 1);
		//mPixelShader.Create("DeferredPointLightRendering.fx", "PS");

		mPointLightBuffer = ConstantBuffer<cbChangesPointLight>::create(8);
		mModel.Create("Assets/ball/ball.pmx.tesmesh");
		m_Radius = 1;
		m_AttenuationStart = 0;
		m_AttenuationParam = 1;
		m_Color = XMFLOAT3(1, 1, 1);

		mMaterial.Create("EngineResource/DeferredPointLightRendering.fx");

	}
	~PointLightComponent(){
		mModel.Release();
	}

	void Initialize() override{
	}

	void Finish() override{

	}
	void Update() override;
	void CreateInspector() override{


		auto data = Window::CreateInspector();
		std::function<void(float)> collbackx = [&](float f){
			m_Color.x = f;
		};

		std::function<void(float)> collbacky = [&](float f){
			m_Color.y = f;
		};

		std::function<void(float)> collbackz = [&](float f){
			m_Color.z = f;
		};

		std::function<void(float)> collbackr = [&](float f){
			m_Radius = f;
		};
		std::function<void(float)> collbackas = [&](float f){
			m_AttenuationStart = f;
		};
		std::function<void(float)> collbackap = [&](float f){
			m_AttenuationParam = f;
		};

		Window::AddInspector(new InspectorFloatDataSet("radius", &m_Radius, collbackr), data);
		Window::AddInspector(new InspectorFloatDataSet("AttenuationStart", &m_AttenuationStart, collbackas), data);
		Window::AddInspector(new InspectorFloatDataSet("AttenuationParam", &m_AttenuationParam, collbackap), data);
		Window::AddInspector(new InspectorSlideBarDataSet("r", 0.0f, 1.0f, &m_Color.x, collbackx), data);
		Window::AddInspector(new InspectorSlideBarDataSet("g", 0.0f, 1.0f, &m_Color.y, collbacky), data);
		Window::AddInspector(new InspectorSlideBarDataSet("b", 0.0f, 1.0f, &m_Color.z, collbackz), data);
		Window::ViewInspector("PointLight", this, data);
	}

	void IO_Data(I_ioHelper* io) override{
#define _KEY(x) io->func( x , #x)
		_KEY(m_Radius);
		_KEY(m_AttenuationStart);
		_KEY(m_AttenuationParam);
		_KEY(m_Color.x);
		_KEY(m_Color.y);
		_KEY(m_Color.z);

#undef _KEY
	}

	//影響範囲半径
	float m_Radius;
	//減衰開始
	float m_AttenuationStart;
	//減衰曲線パラメータ
	float m_AttenuationParam;
	//カラー
	XMFLOAT3 m_Color;

	Material mMaterial;

	Model mModel;

	ConstantBuffer<cbChangesPointLight> mPointLightBuffer;

};

#include <map>
class ComponentFactory{

public:
	static
	shared_ptr<Component> Create(const std::string& ClassName){		
		return _Create(("class " + ClassName));

	}

	static 
	std::map<std::string, std::function<shared_ptr<Component>()>>& GetComponents(){
		return mFactoryComponents;
	}
	static
	void Initialize(){
		_NewFunc<CameraComponent>();
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
		_NewFunc<TextComponent>();
		_NewFunc<PostEffectComponent>();
		_NewFunc<PointLightComponent>();
	}
private:
	template<class T>
	static 
	shared_ptr<Component> _Make(){
		return make_shared<T>();
	}

	template<class T>
	static
	void _NewFunc(){
		mFactoryComponents[typeid(T).name()] = &ComponentFactory::_Make<T>;
	}

	static
	shared_ptr<Component> _Create(std::string ClassName){
		auto p = mFactoryComponents.find(ClassName);
		if (p != mFactoryComponents.end()){
			return p->second();
		}
		return NULL;
	}

	static
		std::map<std::string, std::function<shared_ptr<Component>()>> mFactoryComponents;
};