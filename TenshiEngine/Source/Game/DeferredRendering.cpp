
#include "DeferredRendering.h"



//static
XMVECTOR CascadeShadow::mLightVect = XMVectorSet(0,-1,0,1);

CascadeShadow::CascadeShadow(){
	auto w = WindowState::mWidth;
	auto h = WindowState::mHeight;
	for (int i = 0; i < MAX_CASCADE; i++){
		m_ShadowDepthRT[i].Create(w, h);
	}
	
	m_ShadowDepthDS.CreateDepth(w, h);


	mCBChangesLightCamera = ConstantBuffer<cbChangesLightCamera>::create(10);
}
CascadeShadow::~CascadeShadow(){

	for (int i = 0; i < MAX_CASCADE; i++){
		m_ShadowDepthRT[i].Release();
	}
	m_ShadowDepthDS.Release();

}

void CascadeShadow::Update(){
	
	CascadeUpdate();

	{
		mCBChangesLightCamera.mParam.mLViewProjection[0] = XMMatrixTranspose(m_ShadowMatrix[0]);
		mCBChangesLightCamera.mParam.mLViewProjection[1] = XMMatrixTranspose(m_ShadowMatrix[1]);
		mCBChangesLightCamera.mParam.mLViewProjection[2] = XMMatrixTranspose(m_ShadowMatrix[2]);
		mCBChangesLightCamera.mParam.mLViewProjection[3] = XMMatrixTranspose(m_ShadowMatrix[3]);
		mCBChangesLightCamera.mParam.mSplitPosition = XMFLOAT4(m_SplitPos[0], m_SplitPos[1], m_SplitPos[2], m_SplitPos[3]);

		mCBChangesLightCamera.UpdateSubresource();
		mCBChangesLightCamera.VSSetConstantBuffers();
		mCBChangesLightCamera.PSSetConstantBuffers();
	}
}

void CascadeShadow::SetupShadowCB(int no){

	mCBChangesLightCamera.mParam.mLViewProjection[0] = XMMatrixTranspose(m_ShadowMatrix[no]);
	mCBChangesLightCamera.UpdateSubresource();
	mCBChangesLightCamera.VSSetConstantBuffers();
}

void CascadeShadow::ClearView(int no){

	m_ShadowDepthRT[no].ClearView();
	m_ShadowDepthDS.ClearDepth();
}

void CascadeShadow::SetRT(int no){
	const RenderTarget* r[1] = { &m_ShadowDepthRT[no] };
	RenderTarget::SetRendererTarget((UINT)1, r[0], &m_ShadowDepthDS);
}
Texture& CascadeShadow::GetRTTexture(int no){
	return m_ShadowDepthRT[no].GetTexture();
}


//---------------------------------------------------------------------------------------
//      平行分割位置を求めます.
//---------------------------------------------------------------------------------------
void CascadeShadow::ComputeSplitPositions
(
int splitCount,
float lamda,
float nearClip,
float farClip,
float* positions
)
{
	// 分割数が１の場合は，普通のシャドウマップと同じ.
	if (splitCount == 1)
	{
		positions[0] = nearClip;
		positions[1] = farClip;
		return;
	}

	float inv_m = 1.0f / float(splitCount);    // splitCountがゼロでないことは保証済み.

	// (f/n)を計算.
	float f_div_n = farClip / nearClip;

	// (f-n)を計算.
	float f_sub_n = farClip - nearClip;

	// 実用分割スキームを適用.
	// ※ GPU Gems 3, Chapter 10. Parallel-Split Shadow Maps on Programmable GPUs.
	//    http://http.developer.nvidia.com/GPUGems3/gpugems3_ch10.html を参照.
	for (int i = 1; i<splitCount + 1; ++i)
	{
		// 対数分割スキームで計算.
		float Ci_log = nearClip * powf(f_div_n, inv_m * i);

		// 一様分割スキームで計算.
		float Ci_uni = nearClip + f_sub_n * i * inv_m;

		// 上記の２つの演算結果を線形補間する.
		positions[i] = lamda * Ci_log + Ci_uni * (1.0f - lamda);
	}

	// 最初は, ニア平面までの距離を設定.
	positions[0] = nearClip;

	// 最後は, ファー平面までの距離を設定.
	positions[splitCount] = farClip;
}

#include "Game/Game.h"
#include "Game/Component/CameraComponent.h"
//---------------------------------------------------------------------------------------
//      視錘台の8角を求めて，ビュー射影行列をかけてAABBを求める. 
//---------------------------------------------------------------------------------------
void CascadeShadow::CalculateFrustum
(
float nearClip,
float farClip,
const XMMATRIX& viewProj,
XMVECTOR* minPos,
XMVECTOR* maxPos
)
{
	auto camera = Game::GetMainCamera();
	XMVECTOR Up = XMVectorSet(0, 1, 0, 1);
	XMVECTOR vZ = XMVector3Normalize(camera->gameObject->mTransform->Forward());
	XMVECTOR vX = XMVector3Normalize(XMVector3Cross(Up, vZ));
	XMVECTOR vY = XMVector3Normalize(XMVector3Cross(vZ, vX));


	float aspect = float(WindowState::mWidth) / float(WindowState::mHeight);
	float fov = XM_PIDIV4;

	float nearPlaneHalfHeight = tanf(fov * 0.5f) * nearClip;
	float nearPlaneHalfWidth = nearPlaneHalfHeight * aspect;

	float farPlaneHalfHeight = tanf(fov * 0.5f) * farClip;
	float farPlaneHalfWidth = farPlaneHalfHeight * aspect;

	auto campos = camera->gameObject->mTransform->WorldPosition();
	XMVECTOR nearPlaneCenter = campos + vZ * nearClip;
	XMVECTOR farPlaneCenter = campos + vZ * farClip;

	XMVECTOR corners[8];

	corners[0] = nearPlaneCenter - vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;
	corners[1] = nearPlaneCenter - vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
	corners[2] = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight;
	corners[3] = nearPlaneCenter + vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight;

	corners[4] = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;
	corners[5] = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
	corners[6] = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight;
	corners[7] = farPlaneCenter + vX * farPlaneHalfWidth - vY * farPlaneHalfHeight;


	XMVECTOR point = XMVector3TransformCoord(corners[0], viewProj);
	XMVECTOR mini = point;
	XMVECTOR maxi = point;
	for (int i = 1; i<8; ++i)
	{
		point = XMVector3TransformCoord(corners[i], viewProj);
		mini.x = min(point.x, mini.x);
		mini.y = min(point.y, mini.y);
		mini.z = min(point.z, mini.z);
		maxi.x = max(point.x, maxi.x);
		maxi.y = max(point.y, maxi.y);
		maxi.z = max(point.z, maxi.z);
	}

	*minPos = mini;
	*maxPos = maxi;
}

//---------------------------------------------------------------------------------------
//      クロップ行列を作成します.
//---------------------------------------------------------------------------------------
XMMATRIX CascadeShadow::CreateCropMatrix(const XMVECTOR& mini, const XMVECTOR& maxi)
{
	/* ほぼ単位キューブクリッピングと同じ処理 */
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float scaleZ = 1.0f;
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float offsetZ = 0.0f;

	scaleX = 2.0f / (maxi.x - mini.x);
	scaleY = 2.0f / (maxi.y - mini.y);

	offsetX = -0.5f * (maxi.x + mini.x) * scaleX;
	offsetY = -0.5f * (maxi.y + mini.y) * scaleY;

	// 1.0を下回るとシャドウマップに移る部分が小さくなってしまうので，
	// 制限をかける.
	scaleX = max(1.0f, scaleX);
	scaleY = max(1.0f, scaleY);
#if 0
	// GPU Gems 3の実装では下記処理を行っている.
	// maxi.zの値が0近づくことがあり, シャドウマップが真っ黒になるなどおかしくなることが発生するので,
	// この実装はZ成分については何も処理を行わない.
	mini.z = 0.0f;

	scaleZ = 1.0f / (maxi.z - mini.z);
	offsetZ = -mini.z * scaleZ;
#endif

	return XMMatrixSet(
		scaleX, 0.0f, 0.0f, 0.0f,
		0.0f, scaleY, 0.0f, 0.0f,
		0.0f, 0.0f, scaleZ, 0.0f,
		offsetX, offsetY, offsetZ, 1.0f);
}

void CascadeShadow::CascadeUpdate(){
	float nearClip = 0.01f;
	float farClip = 100.0f;

	float m_Lamda = 0.8f;

	// 平行分割処理.
	float splitPositions[MAX_CASCADE + 1];
	ComputeSplitPositions(MAX_CASCADE, m_Lamda, nearClip, farClip, splitPositions);

	float SlideBack = 50.0f;

	auto camera = Game::GetMainCamera();
	auto campos = camera->gameObject->mTransform->WorldPosition();
	auto camvec = camera->gameObject->mTransform->Forward();
	float forward = splitPositions[0];

	// カスケード処理.
	for (int i = 0; i<MAX_CASCADE; ++i)
	{
		float size = splitPositions[i + 1] - splitPositions[i + 0];

		auto lightPos = campos + camvec * (forward + size / 2) + mLightVect * -SlideBack;

		m_LightView = XMMatrixLookToLH(lightPos, mLightVect, XMVectorSet(0, 1, 0, 1));

		forward += size;

		float aspect = float(WindowState::mWidth) / float(WindowState::mHeight);
		float fov = XM_PIDIV4;
		float farHeight = tanf(fov) * splitPositions[i + 1];
		float farWidth = farHeight * aspect;

		size = farWidth * 1.41421356f * 1.41421356f;
		m_LightProj = XMMatrixOrthographicLH(size, size, 0.01f, 100.0f);

		// ライトのビュー射影行列.
		m_ShadowMatrix[i] = m_LightView * m_LightProj;

		XMVECTOR min, max;

		// 分割した視錘台の8角をもとめて，ライトのビュー射影空間でAABBを求める.
		CalculateFrustum(
			splitPositions[i + 0],
			splitPositions[i + 1],
			m_ShadowMatrix[i],
			&min,
			&max);

		// クロップ行列を求める.
		XMMATRIX crop = CreateCropMatrix(min, max);

		// シャドウマップ行列と分割位置を設定.
		m_ShadowMatrix[i] = m_ShadowMatrix[i] * crop;
		m_SplitPos[i] = splitPositions[i + 1];
	}
}

//static
void CascadeShadow::SetLightVect(const XMVECTOR& vect){
	mLightVect = vect;
}




DeferredRendering::~DeferredRendering(){
	m_AlbedoRT.Release();
	m_SpecularRT.Release();
	m_NormalRT.Release();
	m_DepthRT.Release();
	m_VelocityRT.Release();
	m_LightRT.Release();
	m_LightSpecularRT.Release();
	mModelTexture.Release();

	pBlendState->Release();
}
void DeferredRendering::Initialize(){
	auto w = WindowState::mWidth;
	auto h = WindowState::mHeight;
	m_AlbedoRT.Create(w, h);
	m_SpecularRT.Create(w,h);
	m_NormalRT.Create(w, h);
	m_DepthRT.Create(w, h);
	m_VelocityRT.Create(w, h);
	m_LightRT.Create(w, h);
	m_LightSpecularRT.Create(w, h);

	mEnvironmentMap.Create("EngineResource/Sky/spheremap.jpg");
	//mEnvironmentMap.Create("EngineResource/Sky/a.png");
	mEnvironmentRMap.Create("EngineResource/Sky/spheremapr.jpg");

	mModelTexture.Create("EngineResource/TextureModel.tesmesh");


	mMaterialLight.Create("EngineResource/DeferredLightRendering.fx");
	mMaterialLight.SetTexture(m_NormalRT.GetTexture(), 0);
	mMaterialLight.SetTexture(m_DepthRT.GetTexture(), 1);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(0), 2);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(1), 3);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(2), 4);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(3), 5);

	mMaterialDeferred.Create("EngineResource/DeferredRendering.fx");
	mMaterialDeferred.SetTexture(m_AlbedoRT.GetTexture(), 0);
	mMaterialDeferred.SetTexture(m_SpecularRT.GetTexture(), 1);
	mMaterialDeferred.SetTexture(m_NormalRT.GetTexture(), 2);
	mMaterialDeferred.SetTexture(m_VelocityRT.GetTexture(), 3);
	mMaterialDeferred.SetTexture(m_LightRT.GetTexture(), 4);
	mMaterialDeferred.SetTexture(m_LightSpecularRT.GetTexture(), 5);
	mMaterialDeferred.SetTexture(mEnvironmentMap, 6);
	mMaterialDeferred.SetTexture(mEnvironmentRMap, 7);

	mMaterialDepthShadow.Create("EngineResource/DepthRendering.fx");

	mMaterialPostEffect.Create("");
	mMaterialPostEffect.SetTexture(Game::GetMainViewRenderTarget().GetTexture(),1);
	mMaterialPostEffect.SetTexture(m_AlbedoRT.GetTexture(), 2);
	mMaterialPostEffect.SetTexture(m_SpecularRT.GetTexture(), 3);
	mMaterialPostEffect.SetTexture(m_NormalRT.GetTexture(), 4);
	mMaterialPostEffect.SetTexture(m_DepthRT.GetTexture(), 5);
	mMaterialPostEffect.SetTexture(m_LightRT.GetTexture(), 6);
	mMaterialPostEffect.SetTexture(m_LightSpecularRT.GetTexture(), 7);


	mMaterialDebugDrawPrePass.Create("EngineResource/DebugDeferredPrePass.fx");
	mMaterialDebugDraw.Create("EngineResource/Texture.fx");
	mMaterialDebugDraw.SetTexture(m_AlbedoRT.GetTexture(), 0);

	//ブレンドモード設定

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;

	// TRUEの場合、マルチレンダーターゲットで各レンダーターゲットのブレンドステートの設定を個別に設定できる
	// FALSEの場合、0番目のみが使用される
	BlendDesc.IndependentBlendEnable = FALSE;

	//加算合成設定
	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;
	RenderTarget.BlendEnable = TRUE;
	RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	RenderTarget.DestBlend = D3D11_BLEND_ONE;
	RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BlendDesc.RenderTarget[0] = RenderTarget;

	Device::mpd3dDevice->CreateBlendState(&BlendDesc, &pBlendState);


}
void DeferredRendering::Start_G_Buffer_Rendering(){

	//m_AlbedoRT.ClearView();
	//m_NormalRT.ClearView();
	//m_DepthRT.ClearView();

	mCascadeShadow.Update();


	const RenderTarget* r[5] = { &m_AlbedoRT, &m_SpecularRT, &m_NormalRT, &m_DepthRT, &m_VelocityRT };
	RenderTarget::SetRendererTarget((UINT)5, r[0], Device::mRenderTargetBack);
}
void DeferredRendering::Start_ShadowDepth_Buffer_Rendering(int no){

	mCascadeShadow.ClearView(no);
	mCascadeShadow.SetRT(no);
	mCascadeShadow.SetupShadowCB(no);

	Model::mForcedMaterial = &mMaterialDepthShadow;
}
void DeferredRendering::End_ShadowDepth_Buffer_Rendering(){
	Model::mForcedMaterial = NULL;
}
void DeferredRendering::Start_Light_Rendering(){

	m_LightRT.ClearView();
	m_LightSpecularRT.ClearView();

	const RenderTarget* r[2] = { &m_LightRT, &m_LightSpecularRT };
	RenderTarget::SetRendererTarget((UINT)2, r[0], Device::mRenderTargetBack);


	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Device::mpImmediateContext->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);

	//テクスチャーのセット
	mMaterialLight.PSSetShaderResources();

}

void DeferredRendering::End_Light_Rendering(){

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Device::mpImmediateContext->OMSetBlendState(NULL, blendFactor, 0xffffffff);


}

#include "Input/Input.h"
void DeferredRendering::Start_Deferred_Rendering(RenderTarget* rt){

	rt->SetRendererTarget();

	mModelTexture.Update();

	ID3D11DepthStencilState* pBackDS;
	UINT ref;
	Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

	D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11DepthStencilState* pDS_tex = NULL;
	Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
	Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);


	D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	descRS.CullMode = D3D11_CULL_NONE;
	descRS.FillMode = D3D11_FILL_SOLID;

	ID3D11RasterizerState* pRS = NULL;
	Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

	Device::mpImmediateContext->RSSetState(pRS);

	mModelTexture.Draw(mMaterialDeferred);

	Device::mpImmediateContext->RSSetState(NULL);
	if (pRS)pRS->Release();

	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
	pDS_tex->Release();

	Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
	if (pBackDS)pBackDS->Release();


	RenderTarget::NullSetRendererTarget();
	mMaterialPostEffect.PSSetShaderResources();
}



void DeferredRendering::Debug_G_Buffer_Rendering(){
	const RenderTarget* r[1] = { &m_AlbedoRT};
	RenderTarget::SetRendererTarget((UINT)1, r[0], Device::mRenderTargetBack);

	Model::mForcedMaterialUseTexture = &mMaterialDebugDrawPrePass;

}

void DeferredRendering::Debug_AlbedoOnly_Rendering(RenderTarget* rt){

	Model::mForcedMaterialUseTexture = NULL;

	rt->SetRendererTarget();

	mModelTexture.Update();

	ID3D11DepthStencilState* pBackDS;
	UINT ref;
	Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

	D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11DepthStencilState* pDS_tex = NULL;
	Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
	Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);


	D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	descRS.CullMode = D3D11_CULL_NONE;
	descRS.FillMode = D3D11_FILL_SOLID;

	ID3D11RasterizerState* pRS = NULL;
	Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

	Device::mpImmediateContext->RSSetState(pRS);

	mModelTexture.Draw(mMaterialDebugDraw);

	Device::mpImmediateContext->RSSetState(NULL);
	if (pRS)pRS->Release();

	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
	pDS_tex->Release();

	Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
	if (pBackDS)pBackDS->Release();

	RenderTarget::NullSetRendererTarget();
	mMaterialPostEffect.PSSetShaderResources();
}