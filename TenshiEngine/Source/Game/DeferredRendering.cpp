
#include "DeferredRendering.h"


#include "Game/RenderingSystem.h"

//static
XMVECTOR CascadeShadow::mLightVect = XMVectorSet(0,-1,0,1);

CascadeShadow::CascadeShadow(){
	mWidth = WindowState::mWidth;
	mHeight = WindowState::mHeight;
	//mWidth = 1024;
	//mHeight = 1024;
	for (int i = 0; i < MAX_CASCADE; i++){
		m_ShadowDepthRT[i].Create(mWidth, mHeight, DXGI_FORMAT_R32_FLOAT);
	}
	
	m_ShadowDepthDS.CreateDepth(mWidth, mHeight);


	mCBChangesLightCamera = ConstantBuffer<cbChangesLightCamera>::create(10);
}
CascadeShadow::~CascadeShadow(){

	for (int i = 0; i < MAX_CASCADE; i++){
		m_ShadowDepthRT[i].Release();
	}
	m_ShadowDepthDS.Release();

}

void CascadeShadow::Update(IRenderingEngine* render){
	
	CascadeUpdate();

	{
		mCBChangesLightCamera.mParam.mLViewProjection[0] = XMMatrixTranspose(m_ShadowMatrix[0]);
		mCBChangesLightCamera.mParam.mLViewProjection[1] = XMMatrixTranspose(m_ShadowMatrix[1]);
		mCBChangesLightCamera.mParam.mLViewProjection[2] = XMMatrixTranspose(m_ShadowMatrix[2]);
		mCBChangesLightCamera.mParam.mLViewProjection[3] = XMMatrixTranspose(m_ShadowMatrix[3]);
		mCBChangesLightCamera.mParam.mSplitPosition = XMFLOAT4(m_SplitPos[0], m_SplitPos[1], m_SplitPos[2], m_SplitPos[3]);

		mCBChangesLightCamera.UpdateSubresource(render->m_Context);
		mCBChangesLightCamera.VSSetConstantBuffers(render->m_Context);
		mCBChangesLightCamera.PSSetConstantBuffers(render->m_Context);
	}
}

void CascadeShadow::SetupShadowCB(IRenderingEngine* render, int no){

	mCBChangesLightCamera.mParam.mLViewProjection[0] = XMMatrixTranspose(m_ShadowMatrix[no]);
	mCBChangesLightCamera.UpdateSubresource(render->m_Context);
	mCBChangesLightCamera.VSSetConstantBuffers(render->m_Context);
}

void CascadeShadow::ClearView(IRenderingEngine* render,int no){

	m_ShadowDepthRT[no].ClearView(render->m_Context);
	m_ShadowDepthDS.ClearDepth(render->m_Context);
}

void CascadeShadow::SetRT(IRenderingEngine* render, int no){
	const RenderTarget* r[1] = { &m_ShadowDepthRT[no] };
	RenderTarget::SetRendererTarget(render->m_Context,(UINT)1, r[0], &m_ShadowDepthDS);
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


	float aspect = float(mWidth) / float(mHeight);
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

	//メインシーンのクリップ
	auto cam = Game::GetMainCamera();
	float nearClip = 0.01f;
	float farClip = 100.0f;
	if (cam){
		nearClip = cam->GetNear();
		farClip = cam->GetFar();
	}
	//デプスシーンのクリップ
	float DnearClip = 0.01f;
	float DfarClip = 2000.0f;
	float SlideBack = DfarClip/2;

	float m_Lamda = 0.5f;

	// 平行分割処理.
	float splitPositions[MAX_CASCADE + 1];
	ComputeSplitPositions(MAX_CASCADE, m_Lamda, nearClip, farClip, splitPositions);

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

		float aspect = float(mWidth) / float(mHeight);
		float fov = XM_PIDIV4;
		float farHeight = tanf(fov) * splitPositions[i + 1];
		float farWidth = farHeight * aspect;

		size = farWidth * 1.41421356f * 1.41421356f;
		m_LightProj = XMMatrixOrthographicLH(size, size, DnearClip, DfarClip);

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


DownSample::DownSample(){
	mHDRFilter = false;
	mWidth = 1;
	mHeight = 1;
}
DownSample::~DownSample(){

	mModelTexture.Release();
	m_DownSampleRT.Release();

}
void DownSample::Create(Texture& texture, UINT xy){
	Create(texture, xy, xy);
}
void DownSample::Create(Texture& texture, UINT x, UINT y){

	mWidth = x;
	mHeight = y;

	mModelTexture.Create("EngineResource/TextureModel.tesmesh");
	mModelTexture.mWorld._11 = WindowState::mWidth / (float)x;
	mModelTexture.mWorld._12 = WindowState::mHeight / (float)y;
	if (mHDRFilter){
		mModelTexture.mWorld._12 /= (WindowState::mHeight / (float)WindowState::mWidth);
	}
	mModelTexture.Update();


	std::string sampling = "EngineResource/ScreenTexture.fx";
	if (mHDRFilter){
		sampling = "EngineResource/ScreenTextureHDR.fx";
	}

	mMaterialDownSample.Create(sampling.c_str());
	mMaterialDownSample.SetTexture(texture, 0);
	m_DownSampleRT.Create(x, y, DXGI_FORMAT_R11G11B10_FLOAT);

}
Texture& DownSample::GetRTTexture(){
	return m_DownSampleRT.GetTexture();
}

void DownSample::Draw_DownSample(IRenderingEngine* render){

	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid_Rect);
	{
		D3D11_RECT rect = CD3D11_RECT(0, 0,
			(LONG)mWidth,
			(LONG)mHeight * (WindowState::mHeight / (float)WindowState::mWidth));
		render->m_Context->RSSetScissorRects(1, &rect);

		RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, &m_DownSampleRT, NULL);
		mModelTexture.Draw(render->m_Context, mMaterialDownSample);

		RenderTarget::NullSetRendererTarget(render->m_Context);
	}
	render->PopDS();
	render->PopRS();

}

HDRGaussBulr_AND_DownSample::HDRGaussBulr_AND_DownSample(){
}
HDRGaussBulr_AND_DownSample::~HDRGaussBulr_AND_DownSample(){
	m_GaussBuraUAV_P1.Release();
	m_GaussBuraUAV_P2.Release();
	m_GaussBuraCS.Release();

}
void HDRGaussBulr_AND_DownSample::Create(Texture& texture, UINT xy){
	Create(texture,xy, xy);
}
void HDRGaussBulr_AND_DownSample::Create(Texture& texture, UINT x, UINT y){


	m_DownSample.Create(texture, x, y);

	mCBTextureSize = ConstantBuffer<cbTextureSize>::create(0);
	mCBTextureSize.mParam.TextureSize = XMUINT4(x, y,0, 0);

	UINT xpixel = max(x, y);
	std::string pro = "";
	if (xpixel > 512){
		pro = "CS_768";
	}
	else if(xpixel > 256){
		pro = "CS_512";
	}
	else if (xpixel > 128){
		pro = "CS_256";
	}
	else if (xpixel > 64){
		pro = "CS_128";
	}
	else if (xpixel > 32){
		pro = "CS_64";
	}
	else if (xpixel > 16){
		pro = "CS_32";
	}
	else if (xpixel > 0){
		pro = "CS_16";
	}
	m_GaussBuraCS.Create("EngineResource/GaussCS.fx", pro.c_str());

	m_GaussBuraUAV_P1.Create(x, y, DXGI_FORMAT_R11G11B10_FLOAT);
	m_GaussBuraUAV_P2.Create(x, y, DXGI_FORMAT_R11G11B10_FLOAT);

	bool hdr = m_DownSample.GetHDRFilter();

	std::string sampling = "EngineResource/ScreenTexture.fx";
	if (hdr){
		sampling = "EngineResource/ScreenTextureHDR.fx";
	}

	mResultMaterial.Create("EngineResource/BloomAdd.fx");
	mResultMaterial.SetTexture(m_GaussBuraUAV_P2.GetTexture(), 0);


	mCBBloomParam = ConstantBuffer<cbHDRBloomParam>::create(1);
	mCBBloomParam.mParam.BloomParam = XMFLOAT4(0.5f, 1,1,1);
	if (hdr){
		mCBBloomParam.mParam.BloomParam.x = 1.5;
	}


}
Texture& HDRGaussBulr_AND_DownSample::GetRTTexture(){
	return m_GaussBuraUAV_P2.GetTexture();
}

Material& HDRGaussBulr_AND_DownSample::GetResultMaterial(){
	return mResultMaterial;
}

void HDRGaussBulr_AND_DownSample::Draw_DownSample(IRenderingEngine* render){

	m_DownSample.Draw_DownSample(render);

	UINT x = mCBTextureSize.mParam.TextureSize.x;
	UINT y = mCBTextureSize.mParam.TextureSize.y;
	mCBTextureSize.UpdateSubresource(render->m_Context);
	mCBTextureSize.CSSetConstantBuffers(render->m_Context);

	mCBBloomParam.UpdateSubresource(render->m_Context);
	mCBBloomParam.CSSetConstantBuffers(render->m_Context);

	{

		m_DownSample.GetRTTexture().CSSetShaderResources(render->m_Context, 0);

		m_GaussBuraUAV_P1.SetUnorderedAccessView(render->m_Context);

		m_GaussBuraCS.SetShader(render->m_Context);

		render->m_Context->Dispatch(mCBTextureSize.mParam.TextureSize.x, 1, 1);

	}

	{
		//書き出しが1:1じゃなければ反転
		if (x != y){
			mCBTextureSize.mParam.TextureSize.x = y;
			mCBTextureSize.mParam.TextureSize.y = x;
			mCBTextureSize.UpdateSubresource(render->m_Context);
			mCBTextureSize.CSSetConstantBuffers(render->m_Context);
		}
		m_GaussBuraUAV_P2.SetUnorderedAccessView(render->m_Context);
		m_GaussBuraUAV_P1.GetTexture().CSSetShaderResources(render->m_Context, 0);

		render->m_Context->Dispatch(mCBTextureSize.mParam.TextureSize.x, 1, 1);

	}

	//書き出しが1:1じゃなければ反転を戻す
	if (x != y){
		mCBTextureSize.mParam.TextureSize.x = x;
		mCBTextureSize.mParam.TextureSize.y = y;
	}

	ID3D11ShaderResourceView*	pReses = NULL;
	render->m_Context->CSSetShaderResources(0, 1, &pReses);
	ID3D11UnorderedAccessView*	pUAV = NULL;
	render->m_Context->CSSetUnorderedAccessViews(0, 1, &pUAV, (UINT*)&pUAV);

}


void HDRGaussBulr_AND_DownSample::SetHDRFilter(bool flag){
	m_DownSample.SetHDRFilter(flag);
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


}
void DeferredRendering::Initialize(){

	_SYSTEM_LOG_H("デファードレンダリングの初期化");
	auto w = WindowState::mWidth;
	auto h = WindowState::mHeight;
	m_AlbedoRT.Create(w, h, DXGI_FORMAT_R11G11B10_FLOAT);
	m_SpecularRT.Create(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);//DXGI_FORMAT_R8G8B8A8_UINT
	m_NormalRT.Create(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_DepthRT.Create(w, h, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_VelocityRT.Create(w, h, DXGI_FORMAT_R16G16_FLOAT);
	m_LightRT.Create(w, h, DXGI_FORMAT_R11G11B10_FLOAT);
	m_LightSpecularRT.Create(w, h, DXGI_FORMAT_R11G11B10_FLOAT);

	mEnvironmentMap.Create("EngineResource/Sky/spheremap.jpg");
	//mEnvironmentMap.Create("EngineResource/Sky/a.png");
	mEnvironmentRMap.Create("EngineResource/Sky/spheremapr.jpg");

	mModelTexture.Create("EngineResource/TextureModel.tesmesh");


	mMaterialPrePassEnv.Create("");
	mMaterialPrePassEnv.SetTexture(mEnvironmentMap, 6);
	mMaterialPrePassEnv.SetTexture(mEnvironmentRMap, 7);


	mMaterialLight.Create("EngineResource/DeferredLightRendering.fx");
	mMaterialLight.SetTexture(m_NormalRT.GetTexture(), 0);
	mMaterialLight.SetTexture(m_DepthRT.GetTexture(), 1);
	mMaterialLight.SetTexture(m_SpecularRT.GetTexture(), 2);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(0), 3);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(1), 4);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(2), 5);
	mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(3), 6);

	mMaterialDeferred.Create("EngineResource/DeferredRendering.fx");
	mMaterialDeferred.SetTexture(m_AlbedoRT.GetTexture(), 0);
	mMaterialDeferred.SetTexture(m_SpecularRT.GetTexture(), 1);
	mMaterialDeferred.SetTexture(m_NormalRT.GetTexture(), 2);
	mMaterialDeferred.SetTexture(m_VelocityRT.GetTexture(), 3);
	mMaterialDeferred.SetTexture(m_LightRT.GetTexture(), 4);
	mMaterialDeferred.SetTexture(m_LightSpecularRT.GetTexture(), 5);

	mMaterialDepthShadow.Create("EngineResource/DepthRendering.fx");

	mMaterialParticle.Create("");
	mMaterialParticle.SetTexture(m_NormalRT.GetTexture(), 0);
	mMaterialParticle.SetTexture(m_DepthRT.GetTexture(), 1);

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


	mHDLDownSample[0].SetHDRFilter(true);
	mHDLDownSample[0].Create(Game::GetMainViewRenderTarget().GetTexture(), 768);
	mHDLDownSample[1].Create(mHDLDownSample[0].GetRTTexture(), 512);
	mHDLDownSample[2].Create(mHDLDownSample[1].GetRTTexture(), 256);
	mHDLDownSample[3].Create(mHDLDownSample[2].GetRTTexture(), 128);

}
void DeferredRendering::G_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){

	const RenderTarget* r[5] = { &m_AlbedoRT, &m_SpecularRT, &m_NormalRT, &m_DepthRT, &m_VelocityRT };
	RenderTarget::SetRendererTarget(render->m_Context ,(UINT)5, r[0], Device::mRenderTargetBack);


	mMaterialPrePassEnv.PSSetShaderResources(render->m_Context);

	func();


}
void DeferredRendering::ShadowDepth_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){


	mCascadeShadow.Update(render);

	Model::mForcedMaterialUseTexture = &mMaterialDepthShadow;

	for (int i = 0; i < 4; i++){
		mCascadeShadow.ClearView(render, i);
		mCascadeShadow.SetRT(render, i);
		mCascadeShadow.SetupShadowCB(render, i);

		func();
	}
	Model::mForcedMaterialUseTexture = NULL;
	mCascadeShadow.SetupShadowCB(render, 0);
}

void DeferredRendering::Light_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){

	m_LightRT.ClearView(render->m_Context);
	m_LightSpecularRT.ClearView(render->m_Context);

	const RenderTarget* r[2] = { &m_LightRT, &m_LightSpecularRT };
	RenderTarget::SetRendererTarget(render->m_Context,(UINT)2, r[0], Device::mRenderTargetBack);


	render->PushSet(BlendState::Preset::BS_Add);

	//テクスチャーのセット
	mMaterialLight.PSSetShaderResources(render->m_Context);

	func();


	render->PopBS();
}

#include "Input/Input.h"
void DeferredRendering::Deferred_Rendering(IRenderingEngine* render, RenderTarget* rt){

	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], Device::mRenderTargetBack);

	mModelTexture.Update();

	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);

	mModelTexture.Draw(render->m_Context, mMaterialDeferred);

	render->PopRS();
	render->PopDS();

}

void DeferredRendering::Particle_Rendering(IRenderingEngine* render, RenderTarget* rt, const std::function<void(void)>& func){

	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], Device::mRenderTargetBack);

	mMaterialParticle.GSSetShaderResources(render->m_Context);
	mMaterialParticle.PSSetShaderResources(render->m_Context);

	func();

	RenderTarget::NullSetRendererTarget(render->m_Context);
	//デバッグ用でここにひつよう？
	mMaterialPostEffect.PSSetShaderResources(render->m_Context);
}

void DeferredRendering::Forward_Rendering(IRenderingEngine* render, RenderTarget* rt, const std::function<void(void)>& func){

	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], Device::mRenderTargetBack);

	render->PushSet(BlendState::Preset::BS_Alpha);
	render->PushSet(DepthStencil::Preset::DS_Zero_Less);

	func();

	render->PopBS();
	render->PopDS();
}



void DeferredRendering::Debug_G_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){
	const RenderTarget* r[1] = { &m_AlbedoRT};
	RenderTarget::SetRendererTarget(render->m_Context,(UINT)1, r[0], Device::mRenderTargetBack);

	Model::mForcedMaterialUseTexture = &mMaterialDebugDrawPrePass;

	func();

	Model::mForcedMaterialUseTexture = NULL;
}

void DeferredRendering::Debug_AlbedoOnly_Rendering(IRenderingEngine* render,RenderTarget* rt){

	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], Device::mRenderTargetBack);

	mModelTexture.Update();

	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);

	mModelTexture.Draw(render->m_Context, mMaterialDebugDraw);

	render->PopRS();
	render->PopDS();

}


void DeferredRendering::HDR_Rendering(IRenderingEngine* render){

	for (int i = 0; i < mHDLDownSampleNum; i++){
		mHDLDownSample[i].Draw_DownSample(render);
	}

	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);
	render->PushSet(BlendState::Preset::BS_Add);

	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, &Game::GetMainViewRenderTarget(), NULL);
	
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//render->m_Context->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);
	
	
	for (int i = 0; i < mHDLDownSampleNum; i++){
		mModelTexture.Draw(render->m_Context, mHDLDownSample[i].GetResultMaterial());
	}

	//render->m_Context->OMSetBlendState(NULL, blendFactor, 0xffffffff);

	render->PopBS();
	render->PopRS();
	render->PopDS();


	RenderTarget::NullSetRendererTarget(render->m_Context);
	mMaterialPostEffect.PSSetShaderResources(render->m_Context);

	//Game::AddDrawList(DrawStage::UI, [&](){
	//
	//	auto rend = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	//
	//	rend->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	//	rend->PushSet(Rasterizer::Preset::RS_None_Solid);
	//
	//	mModelTexture.Draw(rend->m_Context, mHDLDownSample[0].GetResultMaterial());
	//
	//	rend->PopRS();
	//	rend->PopDS();
	//});
	
}