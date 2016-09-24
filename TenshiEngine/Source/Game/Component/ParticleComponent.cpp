#include"ParticleComponent.h"


#include "Game/Actor.h"

#include "Game/Component/MaterialComponent.h"
#include "Window/Window.h"
#include "Game/RenderingSystem.h"
#include "Game/Game.h"

#include "Engine/Inspector.h"

#define _MAX_CAPACITY_PARTICLE 5000


class ParticleInitializeHelper{
public:
	ParticleInitializeHelper(){
		// パーティクルの初期値を設定
		for (int i = 0; i<_MAX_CAPACITY_PARTICLE; i++)
		{
			mInitParticle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			mInitParticle[i].v0 = XMFLOAT3(0, rand() * 0.001f, 0);
			mInitParticle[i].v = XMFLOAT3(0, 0, 0);
			mInitParticle[i].time = XMFLOAT3(-10000.0f,0,0);
		};

		// ターゲット入出力バッファーを作成
		::ZeroMemory(&mBufferDesc, sizeof(mBufferDesc));
		mBufferDesc.ByteWidth = sizeof(mInitParticle);  // バッファサイズ
		mBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		mBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		mBufferDesc.CPUAccessFlags = 0;
		mBufferDesc.MiscFlags = 0;
		mAppedResource.pSysMem = mInitParticle;  // バッファ・データの初期値
		mAppedResource.SysMemPitch = 0;
		mAppedResource.SysMemSlicePitch = 0;


		UINT stride[] = { sizeof(ParticleComponent::ParticleVertex) };

		D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // 位置座標
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // 初速度
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // 初速度
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }   // 時間
		};

		D3D11_SO_DECLARATION_ENTRY decl[] = {
				{ 0, "POSITION", 0, 0, 3, 0 },
				{ 0, "NORMAL", 0, 0, 3, 0 },
				{ 0, "BINORMAL", 0, 0, 3, 0 },
				{ 0, "TEXCOORD", 0, 0, 3, 0 },
		};
		mGeometryShader0.Create("EngineResource/ParticleGS.fx", "GS0_Main", decl, _countof(decl), stride, _countof(stride));
		mGeometryShader1.Create("EngineResource/ParticleGS.fx", "GS1_Main");
		mVertexShader.Create("EngineResource/ParticleGS.fx", "VS0_Main", layout, 4);
		mPixelShader.Create("EngineResource/ParticleGS.fx", "PS1_Main");
	}
	~ParticleInitializeHelper(){
		mGeometryShader0.Release();
		mGeometryShader1.Release();
	}

	static ParticleInitializeHelper* Instance(){
		static ParticleInitializeHelper This;
		return &This;
	}


	void CreateBuffer(UINT num,ID3D11Buffer** buf){
		mBufferDesc.ByteWidth = sizeof(ParticleComponent::ParticleVertex) * num;  // バッファサイズ
		Device::mpd3dDevice->CreateBuffer(&mBufferDesc, &mAppedResource, buf);
	}

	GeometryStreamOutputShader mGeometryShader0;
	GeometryStreamOutputShader mGeometryShader1;
	VertexShader	mVertexShader;
	PixelShader		mPixelShader;
private:
	ParticleComponent::ParticleVertex mInitParticle[_MAX_CAPACITY_PARTICLE];
	D3D11_BUFFER_DESC mBufferDesc;
	D3D11_SUBRESOURCE_DATA mAppedResource;
};

ParticleComponent::ParticleComponent()
	:mFirstDraw(true){
	mpSOBuffer[0] = NULL;
	mpSOBuffer[1] = NULL;
	mInitialize = false;


	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	if (!mCBuffer.mBuffer)
		return;

	mCParticleBuffer = ConstantBuffer<CBChangesPaticleParam>::create(10);
	if (!mCParticleBuffer.mBuffer)
		return;

	mParticleCapacity = 256;

	mParticleParam.Vector = XMFLOAT4(0, 0.1f, 0, 0.5f);
	mParticleParam.Point = XMFLOAT4(0, 0, 0, 0);
	mParticleParam.Rot = XMFLOAT4(90, 360, 0, 0.99f);
	mParticleParam.MinMaxScale = XMFLOAT4(0.05f, 0.05f, 0.5f, 1.0f);
	mParticleParam.G = XMFLOAT4(0, -0.0098f, 0, 0);
	mParticleParam.Time = XMFLOAT4(1.0f, 3.0f, 1.0f, 1.0f);
	mParticleParam.Param = XMFLOAT4((float)mParticleCapacity, 0, 0, 0);
	mParticleParam.Wind = XMFLOAT4(0, 0, 0, 0);
	mParticleParam.SmoothAlpha = XMFLOAT4(0.2f, 0.5f, 0, 0);

	mParticleParam.Param.w = (float)rand();

	//ParticleCapacityChange((UINT)mParticleCapacity);

	mBlendAdd = true;
	mTimer = 0.0f;
	mAutoDestroy = false;
	mEngineUpdate = false;

	mInitialize = true;
}
ParticleComponent::~ParticleComponent(){

	if (mpSOBuffer[0])mpSOBuffer[0]->Release();
	if (mpSOBuffer[1])mpSOBuffer[1]->Release();
}

void ParticleComponent::ParticleCapacityChange(UINT num){

	mParticleCapacity = max(min(num, _MAX_CAPACITY_PARTICLE),1);

	auto helper = ParticleInitializeHelper::Instance();

	for (int i = 0; i < 2; i++){
		if (mpSOBuffer[i])mpSOBuffer[i]->Release();
		ID3D11Buffer* buf;
		helper->CreateBuffer((UINT)mParticleCapacity,&buf);
		mpSOBuffer[i] = buf;
		
	}
	mFirstDraw = true;
}


void ParticleComponent::Initialize(){
	mTimer = 0.0f;
	ParticleCapacityChange((UINT)mParticleCapacity);
}

void ParticleComponent::EngineUpdate(){
	mTimer = -1.0f;
	mEngineUpdate = true;
	Update();
	mEngineUpdate = false;
}
void ParticleComponent::Update(){
	if (!mInitialize)return;
	if (!mMaterial){
		mMaterial = gameObject->GetComponent<MaterialComponent>();
		if (!mMaterial)return;
	}

	if (!mEngineUpdate){
		//エンジンからスタートした
		if (mTimer == -1.0f){
			Initialize();
		}
		mTimer += Game::GetDeltaTime()->GetDeltaTime();
		if (mAutoDestroy){
			//maxtime * impact + maxtime * loop = 最大時間
			float maxtime = (float)mParticleParam.Time.y * (1 - mParticleParam.Param.y) + mParticleParam.Time.y * max(mParticleParam.Param.z, 1)*1.01f;
			if (maxtime <= mTimer){
				Game::DestroyObject(gameObject);
			}
		}
	}

	mParticleParam.Param.w++;
	// ターゲット出力バッファーを入れ替える
	std::swap(mpSOBuffer[0], mpSOBuffer[1]);
	//mpSOBufferVector[0].swap(mpSOBufferVector[1]);

	//for (int i = 0; i < 2; i++){
	//	for (int j = 0; j < mpSOBufferVector[i].size(); j++){
	//		mpSOBufferArray[i][j] = &mpSOBufferVector[i].data()[j];
	//	}
	//}

	Game::AddDrawList(DrawStage::Particle, [&](){

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		render->PushSet(DepthStencil::Preset::DS_Zero_Less);



		if (mBlendAdd){
			render->PushSet(BlendState::Preset::BS_Add, 0xFFFFFFFF);
		}
		else{
			render->PushSet(BlendState::Preset::BS_Alpha,0xFFFFFFFF);
		}


		mCBuffer.mParam.mWorld = XMMatrixTranspose(gameObject->mTransform->GetMatrix());
		mCBuffer.UpdateSubresource(render->m_Context);
		mCBuffer.GSSetConstantBuffers(render->m_Context);


		mCParticleBuffer.mParam = mParticleParam;
		mCParticleBuffer.UpdateSubresource(render->m_Context);
		mCParticleBuffer.GSSetConstantBuffers(render->m_Context);

		mMaterial->GetMaterial(0).PSSetShaderResources(render->m_Context);


		// 入力アセンブラー ステージに頂点バッファーの配列をバインドする。
		UINT stride = sizeof(ParticleVertex);
		UINT offset = 0;

		//ID3D11Buffer* buf1[] = { mpSOBufferVector[0][0], mpSOBufferVector[0][1], mpSOBufferVector[0][2], mpSOBufferVector[0][3], mpSOBufferVector[0][4], mpSOBufferVector[0][5], mpSOBufferVector[0][6] };
		render->m_Context->IASetVertexBuffers(0, 1, &mpSOBuffer[0], &stride, &offset);

		render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

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




		// ピクセルシェーダーを無効にした状態で深度ステンシルを有効にしているとエラーになるので無効にする
		//ddsDesc.DepthEnable = FALSE;  // 深度テストを無効にする
		//ddsDesc.StencilEnable = FALSE;




		ParticleInitializeHelper::Instance()->mVertexShader.SetShader(render->m_Context);

		ParticleInitializeHelper::Instance()->mGeometryShader0.SetShader(render->m_Context);

		// ピクセルシェーダーをデバイスに無効にする。
		render->m_Context->PSSetShader(NULL, NULL, 0);

		//Device::mpImmediateContext->VSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->HSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->DSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->GSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->PSSetShader(NULL, NULL, 0);
		//Device::mpImmediateContext->CSSetShader(NULL, NULL, 0);
		// 出力バッファーの配列のオフセット値
		// パイプラインのストリーム出力ステージで使用されるターゲット出力バッファーを設定します。

		render->m_Context->SOSetTargets(1, &mpSOBuffer[1], &offset);


		// 最初は頂点数がわかるので ID3D11DeviceContext::Draw() で描画する
		if (mFirstDraw)
		{
			render->m_Context->Draw(mParticleCapacity, 0);
			mFirstDraw = false;
		}
		// ２回目移行は処理する頂点数が不明なので ID3D11DeviceContext::DrawAuto() で描画する
		else
		{
			render->m_Context->DrawAuto();
		}

		ID3D11Buffer* pNullBuffer[] = { NULL};
		// パイプラインのストリーム出力ステージで使用されるターゲット出力バッファーを無効にする。
		render->m_Context->SOSetTargets(1, pNullBuffer, NULL);


		ParticleInitializeHelper::Instance()->mGeometryShader1.SetShader(render->m_Context);
		ParticleInitializeHelper::Instance()->mPixelShader.SetShader(render->m_Context);

		render->m_Context->DrawAuto();

		render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		render->m_Context->GSSetShader(NULL, NULL, 0);

		render->PopDS();
		render->PopBS();
	});
}
#ifdef _ENGINE_MODE
void ParticleComponent::CreateInspector(){

	Inspector ins("Particle",this);
	ins.AddEnableButton(this);
	ins.AddLabel("Particle");

	std::function<void(Vector3)> collbackV = [&](Vector3 f){
		mParticleParam.Vector.x = f.x;
		mParticleParam.Vector.y = f.y;
		mParticleParam.Vector.z = f.z;
	};
	std::function<void(float)> collbackSpeed = [&](float f){
		mParticleParam.Vector.w = f;
	};
	std::function<void(Vector3)> collbackMinV = [&](Vector3 f){
		mParticleParam.Rot.x = f.z;
		mParticleParam.Rot.y = f.y;
		mParticleParam.Rot.z = f.z;
	};
	std::function<void(Vector3)> collbackP = [&](Vector3 f){
		mParticleParam.Point.x = f.x;
		mParticleParam.Point.y = f.y;
		mParticleParam.Point.z = f.z;
	};
	std::function<void(float)> collbackPT = [&](float f){
		mParticleParam.Point.w = f;
	};

	std::function<void(Vector3)> collbackG = [&](Vector3 f){
		mParticleParam.G.x = f.z;
		mParticleParam.G.y = f.y;
		mParticleParam.G.z = f.z;
	};
	std::function<void(float)> collbackMinS = [&](float f){
		mParticleParam.MinMaxScale.x = f;
	};
	std::function<void(float)> collbackMaxS = [&](float f){
		mParticleParam.MinMaxScale.y = f;
	};

	std::function<void(float)> collbackRef = [&](float f){
		mParticleParam.MinMaxScale.z = f;
	};
	std::function<void(float)> collbackFri = [&](float f){
		mParticleParam.MinMaxScale.w = f;
	};
	std::function<void(float)> collbackAirFri = [&](float f){
		mParticleParam.Rot.w = f;
	};

	std::function<void(float)> collbackMinT = [&](float f){
		mParticleParam.Time.x = f;
	};
	std::function<void(float)> collbackMaxT = [&](float f){
		mParticleParam.Time.y = f;
	};
	std::function<void(float)> collbackInp = [&](float f){
		mParticleParam.Param.y = f;
	};
	std::function<void(float)> collbackZCol = [&](float f){
		mParticleParam.Time.z = f;
	};
	std::function<void(float)> collbackVB = [&](float f){
		mParticleParam.Time.w = f;
	};
	std::function<void(float)> collbackPosG = [&](float f){
		mParticleParam.G.w = f;
	};
	std::function<void(float)> collbackNum = [&](float f){
		mParticleParam.Param.x = (float)max(min((int)f,mParticleCapacity),0);
	};
	std::function<void(int)> collbackCap = [&](int f){
		ParticleCapacityChange((UINT)f);
	};
	std::function<void(float)> collbackLoop = [&](float f){
		mParticleParam.Param.z = f;
	};
	std::function<void(bool)> collbackAdd = [&](bool f){
		mBlendAdd = f;
	};

	ins.Add("Capacity", &mParticleCapacity, collbackCap);
	ins.Add("Num", &mParticleParam.Param.x, collbackNum);
	auto v = Vector3(mParticleParam.Vector);
	ins.Add("Vector", &v, collbackV);

	auto r = Vector3(mParticleParam.Rot);
	ins.Add("RandRot", &r, collbackMinV);

	ins.Add("SubSpeed", &mParticleParam.Vector.w, collbackSpeed);

	auto e = Vector3(mParticleParam.Point);
	ins.Add("EmitPoint", &e, collbackP);

	ins.Add("EmitType", &mParticleParam.Point.w, collbackPT);

	auto g = Vector3(mParticleParam.G);
	ins.Add("Gravity", &g, collbackG);
	ins.Add("MinScale", &mParticleParam.MinMaxScale.x, collbackMinS);
	ins.Add("MaxScale", &mParticleParam.MinMaxScale.y, collbackMaxS);
	ins.Add("MinTime", &mParticleParam.Time.x, collbackMinT);
	ins.Add("MaxTime", &mParticleParam.Time.y, collbackMaxT);
	ins.Add("Impact", &mParticleParam.Param.y, collbackInp);
	ins.Add("Z-Collision", &mParticleParam.Time.z, collbackZCol);
	ins.Add("Bound", &mParticleParam.MinMaxScale.z, collbackRef);
	ins.Add("Friction", &mParticleParam.MinMaxScale.w, collbackFri);
	ins.Add("AirFriction", &mParticleParam.Rot.w, collbackAirFri);
	ins.Add("PointGravity", &mParticleParam.G.w, collbackPosG);
	ins.Add("V-Rot&Bura", &mParticleParam.Time.w, collbackVB);
	ins.Add("Loop", &mParticleParam.Param.z, collbackLoop);
	ins.Add("PopAlpha", &mParticleParam.SmoothAlpha.x, [&](float f){mParticleParam.SmoothAlpha.x = f; });
	ins.Add("DeathAlpha", &mParticleParam.SmoothAlpha.y, [&](float f){mParticleParam.SmoothAlpha.y = f; });

	auto w = Vector3(mParticleParam.Wind);
	ins.Add("Gravity", &w, [&](Vector3 f){
			mParticleParam.Wind.x = f.x;
			mParticleParam.Wind.y = f.y;
			mParticleParam.Wind.z = f.z;
	});
	ins.Add("BlendModeAdd", &mBlendAdd, collbackAdd);
	ins.Add("AutoDestroy", &mAutoDestroy, [&](bool f){mAutoDestroy = f; });

	ins.Complete();
}
#endif

void ParticleComponent::IO_Data(I_ioHelper* io){
	
	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)

	_KEY(mParticleCapacity);
	_KEY(mParticleParam.Vector.x);
	_KEY(mParticleParam.Vector.y);
	_KEY(mParticleParam.Vector.z);
	_KEY(mParticleParam.Vector.w);
	_KEY(mParticleParam.Rot.x);
	_KEY(mParticleParam.Rot.y);
	_KEY(mParticleParam.Rot.z);
	_KEY(mParticleParam.Rot.w);
	_KEY(mParticleParam.Point.x);
	_KEY(mParticleParam.Point.y);
	_KEY(mParticleParam.Point.z);
	_KEY(mParticleParam.Point.w);
	_KEY(mParticleParam.G.x);
	_KEY(mParticleParam.G.y);
	_KEY(mParticleParam.G.z);
	_KEY(mParticleParam.G.w);
	_KEY(mParticleParam.MinMaxScale.x);
	_KEY(mParticleParam.MinMaxScale.y);
	_KEY(mParticleParam.MinMaxScale.z);
	_KEY(mParticleParam.MinMaxScale.w);
	_KEY(mParticleParam.Time.x);
	_KEY(mParticleParam.Time.y);
	_KEY(mParticleParam.Time.z);
	_KEY(mParticleParam.Time.w);
	_KEY(mParticleParam.Param.x);
	_KEY(mParticleParam.Param.y);
	_KEY(mParticleParam.Param.z);
	_KEY(mParticleParam.Wind.x);
	_KEY(mParticleParam.Wind.y);
	_KEY(mParticleParam.Wind.z);
	_KEY(mParticleParam.Wind.w);
	_KEY(mParticleParam.SmoothAlpha.x);
	_KEY(mParticleParam.SmoothAlpha.y);
	_KEY(mParticleParam.SmoothAlpha.z);
	_KEY(mParticleParam.SmoothAlpha.w);
	_KEY(mBlendAdd);
	_KEY(mAutoDestroy);

#undef _KEY
}