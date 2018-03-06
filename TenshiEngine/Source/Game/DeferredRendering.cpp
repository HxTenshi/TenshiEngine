#undef _XM_SSE_INTRINSICS_
#include "DeferredRendering.h"


#include "Game/RenderingSystem.h"

//static
XMVECTOR CascadeShadow::mLightVect = XMVectorSet(0,-1,0,1);

CascadeShadow::CascadeShadow(){

	//mWidth = WindowState::mWidth;
	//mHeight = WindowState::mHeight;
	mWidth = 1024;
	mHeight = 1024;
	for (int i = 0; i < MAX_CASCADE; i++){
		m_ShadowDepthRT[i].Create(mWidth, mHeight, DXGI_FORMAT_R32_FLOAT);
	}
	
	m_ShadowDepthDS.CreateDepth(mWidth, mHeight);
	mWidth = 1920;
	mHeight = 1080;


	mCBNeverChanges = ConstantBuffer<CBNeverChanges>::create(0);
	mCBChangeOnResize = ConstantBuffer<CBChangeOnResize>::create(1);

	mCBChangeOnResize.mParam.mProjection = XMMatrixIdentity();
	mCBChangeOnResize.mParam.mProjectionInv = XMMatrixIdentity();

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

	mCBNeverChanges.mParam.mView = XMMatrixTranspose(m_ShadowMatrix[no]);
	mCBNeverChanges.mParam.mViewInv = XMMatrixTranspose(m_ShadowMatrix[no]);

	mCBNeverChanges.UpdateSubresource(render->m_Context);
	mCBNeverChanges.VSSetConstantBuffers(render->m_Context);
	mCBChangeOnResize.UpdateSubresource(render->m_Context);
	mCBChangeOnResize.VSSetConstantBuffers(render->m_Context);

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
//      ���s�����ʒu�����߂܂�.
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
	// ���������P�̏ꍇ�́C���ʂ̃V���h�E�}�b�v�Ɠ���.
	if (splitCount == 1)
	{
		positions[0] = nearClip;
		positions[1] = farClip;
		return;
	}

	float inv_m = 1.0f / float(splitCount);    // splitCount���[���łȂ����Ƃ͕ۏ؍ς�.

	// (f/n)���v�Z.
	float f_div_n = farClip / nearClip;

	// (f-n)���v�Z.
	float f_sub_n = farClip - nearClip;

	// ���p�����X�L�[����K�p.
	// �� GPU Gems 3, Chapter 10. Parallel-Split Shadow Maps on Programmable GPUs.
	//    http://http.developer.nvidia.com/GPUGems3/gpugems3_ch10.html ���Q��.
	for (int i = 1; i<splitCount + 1; ++i)
	{
		// �ΐ������X�L�[���Ōv�Z.
		float Ci_log = nearClip * powf(f_div_n, inv_m * i);

		// ��l�����X�L�[���Ōv�Z.
		float Ci_uni = nearClip + f_sub_n * i * inv_m;

		// ��L�̂Q�̉��Z���ʂ���`��Ԃ���.
		positions[i] = lamda * Ci_log + Ci_uni * (1.0f - lamda);
	}

	// �ŏ���, �j�A���ʂ܂ł̋�����ݒ�.
	positions[0] = nearClip;

	// �Ō��, �t�@�[���ʂ܂ł̋�����ݒ�.
	positions[splitCount] = farClip;
}

#include "Game/Game.h"
#include "Game/Component/CameraComponent.h"
//---------------------------------------------------------------------------------------
//      �������8�p�����߂āC�r���[�ˉe�s���������AABB�����߂�. 
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
	XMVECTOR vZ = camera->gameObject->mTransform->Forward();
	XMVECTOR vX = camera->gameObject->mTransform->Left();
	XMVECTOR vY = camera->gameObject->mTransform->Up();


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

	corners[0] = nearPlaneCenter - vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight - mLightVect*50.0f;
	corners[1] = nearPlaneCenter - vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight - mLightVect*50.0f;
	corners[2] = nearPlaneCenter + vX * nearPlaneHalfWidth + vY * nearPlaneHalfHeight - mLightVect*50.0f;
	corners[3] = nearPlaneCenter + vX * nearPlaneHalfWidth - vY * nearPlaneHalfHeight - mLightVect*50.0f;

	corners[4] = farPlaneCenter - vX * farPlaneHalfWidth - vY * farPlaneHalfHeight + mLightVect*50.0f;
	corners[5] = farPlaneCenter - vX * farPlaneHalfWidth + vY * farPlaneHalfHeight + mLightVect*50.0f;
	corners[6] = farPlaneCenter + vX * farPlaneHalfWidth + vY * farPlaneHalfHeight + mLightVect*50.0f;
	corners[7] = farPlaneCenter + vX * farPlaneHalfWidth - vY * farPlaneHalfHeight + mLightVect*50.0f;


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
//      �N���b�v�s����쐬���܂�.
//---------------------------------------------------------------------------------------
XMMATRIX CascadeShadow::CreateCropMatrix(const XMVECTOR& mini, const XMVECTOR& maxi)
{
	/* �قڒP�ʃL���[�u�N���b�s���O�Ɠ������� */
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

	// 1.0�������ƃV���h�E�}�b�v�Ɉڂ镔�����������Ȃ��Ă��܂��̂ŁC
	// ������������.
	scaleX = max(1.0f, scaleX);
	scaleY = max(1.0f, scaleY);
#if 0
	// GPU Gems 3�̎����ł͉��L�������s���Ă���.
	// maxi.z�̒l��0�߂Â����Ƃ�����, �V���h�E�}�b�v���^�����ɂȂ�Ȃǂ��������Ȃ邱�Ƃ���������̂�,
	// ���̎�����Z�����ɂ��Ă͉����������s��Ȃ�.
	//mini.z = 0.0f;

	scaleZ = 1.0f / (maxi.z - mini.z);
	offsetZ = -mini.z * scaleZ;
#endif

	return XMMatrixSet(
		scaleX, 0.0f, 0.0f, 0.0f,
		0.0f, scaleY, 0.0f, 0.0f,
		0.0f, 0.0f, scaleZ, 0.0f,
		offsetX, offsetY, offsetZ, 1.0f);
}


void CascadeShadow::OrthographicSize(XMMATRIX& matrix, XMVECTOR& mini, XMVECTOR& maxi) {

	float ratio = float(WindowState::mWidth) / float(WindowState::mHeight);
	float fovy = XM_PIDIV4;

	auto cam = Game::GetMainCamera();
	if (!cam)return;
	
	float near_distance = cam->GetNear();
	float far_distance = cam->GetFar();

	//XM_PI / 180.0
	float tang = tanf(fovy * 0.5f);//������������萔�̂ق����������� 
	float half_near_height = near_distance * tang;
	float half_near_width = half_near_height * ratio;
	float half_far_height = far_distance  * tang;
	float half_far_width = half_far_height *ratio;

	XMVECTOR _p = cam->gameObject->mTransform->WorldPosition();
	XMVECTOR X = cam->gameObject->mTransform->Left();
	XMVECTOR Y = cam->gameObject->mTransform->Up();
	XMVECTOR Z = cam->gameObject->mTransform->Forward();

 	//���E���W� 
    XMVECTOR near_center = _p + Z * near_distance;
    XMVECTOR far_center = _p + Z * far_distance;

	XMVECTOR point[8];
	point[0] = near_center + Y * half_near_height - X * half_near_width;//near top left 
    point[1] = near_center + Y * half_near_height + X * half_near_width;//near top right 
	point[2] = near_center - Y * half_near_height - X * half_near_width;//near bottom left 
	point[3] = near_center - Y * half_near_height + X * half_near_width;//near bottom right 
    point[4] = far_center + Y * half_far_height - X * half_far_width;//far top left 
    point[5] = far_center + Y * half_far_height + X * half_far_width;//far top right 
    point[6] = far_center - Y * half_far_height - X * half_far_width;//far bottom left 
    point[7] = far_center - Y * half_far_height + X * half_far_width;//far bottom right 

	XMVECTOR p = XMVector3TransformCoord(point[0], matrix);
	mini = p;
	maxi = p;
	{
		for (int i = 1; i<8; ++i)
		{
			p = XMVector3TransformCoord(point[i], matrix);
			mini.x = min(mini.x, p.x);
			mini.y = min(mini.y, p.y);
			mini.z = min(mini.z, p.z);
			maxi.x = max(maxi.x, p.x);
			maxi.y = max(maxi.y, p.y);
			maxi.z = max(maxi.z, p.z);
		}
	}
}


void CascadeShadow::CascadeUpdate(){

	//---------------------------------------
	// ���C�g�̃r���[�s��Ǝˉe�s������߂�.
	//---------------------------------------
	{
		XMVECTOR LightBasisW;
		XMVECTOR LightBasisV;
		{
			XMVECTOR n = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
			XMVECTOR m = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

			XMVECTOR w = XMVector3Normalize(mLightVect);
			XMVECTOR u = XMVector3Cross(w, n);
			if (XMVector3Length(u).x < FLT_EPSILON)
			{
				u = XMVector3Cross(w, m);
			}
			u = XMVector3Normalize(u);

			XMVECTOR v = XMVector3Cross(u, w);
			v = XMVector3Normalize(v);
			LightBasisW = w;
			LightBasisV = u;
		}
		// ���C�g�̃r���[�s��𐶐�.
		m_LightView = XMMatrixLookToLH(
			XMVectorSet(0.0f,0.0f,0.0f,1.0f),
			LightBasisW,
			LightBasisV);


		// ���C�g�r���[��Ԃł�AABB�����߂�.
		XMVECTOR mini;
		XMVECTOR maxi;
		OrthographicSize(m_LightView, mini, maxi);

		// ���C�g�r���[��Ԃł̒��S�����߂�.
		XMVECTOR center = (mini + maxi) * 0.5f;

		// �j�A�N���b�v���ʂƃt�@�[�N���b�v���ʂ̋��������߂�.
		float addfar = 100.0f;
		float nearClip = 0.01f;
		float farClip = fabs(maxi.z - mini.z) * 1.001f + nearClip + addfar;

		// ��ޗʂ����߂�.
		float slideBack = fabs(center.z - mini.z) + nearClip + addfar;
		//float slideBack = farClip/2.0f + nearClip;

		// ���������C�g�ʒu�����߂�.
		XMVECTOR lightPos = center - (LightBasisW * slideBack);

		// ���C�g�̃r���[�s����Z�o������.
		m_LightView = XMMatrixLookToLH(
			lightPos,
			LightBasisW,
			LightBasisV);

		// ���ߒ��������C�g�̃r���[�s����g����AABB�����߂�.
		OrthographicSize(m_LightView, mini, maxi);

		// �T�C�Y�����߂�.
		float size = XMVector3Length(maxi - mini).x;

		// ���C�g�̎ˉe�s��.
		m_LightProj = XMMatrixOrthographicLH(
			size,
			size,
			nearClip,
			farClip);

		// ���C�g�̃r���[�ˉe�s������߂�.
		XMMATRIX lightViewProj = m_LightView * m_LightProj;
		//----------------------------------
		//�@�P�ʃL���[�u�N���b�s���O.
		//----------------------------------
		{
			OrthographicSize(lightViewProj, mini, maxi);
			
			
			// �ɒ[�ɂ䂪�܂Ȃ��悤�ɐ������|����.
			mini.x = min(max(mini.x, -1.0f), 1.0f);
			maxi.x = min(max(maxi.x, -1.0f), 1.0f);
			mini.y = min(max(mini.y, -1.0f), 1.0f);
			maxi.y = min(max(maxi.y, -1.0f), 1.0f);
			
			XMMATRIX clip;
			{
				// �P�ʃL���[�u�N���b�v�s������߂�.
				clip._11 = 2.0f / (maxi.x - mini.x);
				clip._12 = 0.0f;
				clip._13 = 0.0f;
				clip._14 = 0.0f;
			
				clip._21 = 0.0f;
				clip._22 = 2.0f / (maxi.y - mini.y);
				clip._23 = 0.0f;
				clip._24 = 0.0f;
			
				clip._31 = 0.0f;
				clip._32 = 0.0f;
				clip._33 = 1.0f / (maxi.z - mini.z);
				clip._34 = 0.0f;
			
				clip._41 = -(maxi.x + mini.x) / (maxi.x - mini.x);
				clip._42 = -(maxi.y + mini.y) / (maxi.y - mini.y);
				clip._43 = -mini.z / (maxi.z - mini.z);
				clip._44 = 1.0f;
			
			}
			
			// �V���h�E�}�b�v�߂����ς��ɉf��悤�Ƀt�B�b�e�B���O.
			m_LightProj = m_LightProj * clip;
		}
	}


	//���C���V�[���̃N���b�v
	auto cam = Game::GetMainCamera();
	float nearClip = 0.01f;
	float farClip = 100.0f;
	if (cam) {
		nearClip = cam->GetNear();
		farClip = cam->GetFar();
		//�f�v�X�V�[���̃N���b�v
		//float DnearClip = 0.01f;
		//float DfarClip = 1000.0f;
		//float SlideBack = DfarClip/2;
	}

	float m_Lamda = 0.9f;

	// ���s��������.
	float splitPositions[MAX_CASCADE + 1];
	ComputeSplitPositions(MAX_CASCADE, m_Lamda, nearClip, farClip, splitPositions);

	float forward = splitPositions[0];

	// �J�X�P�[�h����.
	for (int i = 0; i<MAX_CASCADE; ++i)
	{
		//auto campos = cam->gameObject->mTransform->WorldPosition();
		//auto camvec = cam->gameObject->mTransform->Forward();
		//float size = splitPositions[i + 1] - splitPositions[i + 0];
		//float slideBack = farClip / 2.0f;
		//auto lightPos = campos + camvec * (forward + size / 2) + mLightVect * -slideBack;
		//
		//m_LightView = XMMatrixLookToLH(lightPos, mLightVect, XMVectorSet(0, 1, 0, 1));
		//
		//forward += size;
		//
		//float aspect = float(mWidth) / float(mHeight);
		//float fov = XM_PIDIV4;
		//float farHeight = tanf(fov) * splitPositions[i + 1];
		//float farWidth = farHeight * aspect;
		//
		//size = farWidth * 1.41421356f * 1.41421356f;
		//m_LightProj = XMMatrixOrthographicLH(size, size, nearClip, farClip);

		// ���C�g�̃r���[�ˉe�s��.
		m_ShadowMatrix[i] = m_LightView * m_LightProj;

		XMVECTOR min, max;

		// ���������������8�p�����Ƃ߂āC���C�g�̃r���[�ˉe��Ԃ�AABB�����߂�.
		CalculateFrustum(
			splitPositions[i + 0],
			splitPositions[i + 1],
			m_ShadowMatrix[i],
			&min,
			&max);

		// �N���b�v�s������߂�.
		XMMATRIX crop = CreateCropMatrix(min, max);

		// �V���h�E�}�b�v�s��ƕ����ʒu��ݒ�.
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
	//mModelTexture.mWorld._11 = WindowState::mWidth / (float)x;
	//mModelTexture.mWorld._12 = WindowState::mHeight / (float)y;
	//if (mHDRFilter){
	//	mModelTexture.mWorld._12 /= (WindowState::mHeight / (float)WindowState::mWidth);
	//}
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
	render->PushSet(Rasterizer::Preset::RS_None_Solid);
	{

		// Setup the viewport
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)mWidth;
		vp.Height = (FLOAT)mHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		render->m_Context->RSSetViewports(1, &vp);

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

	mResultMaterial.Create("EngineResource/BloomAdd.fx");
	mResultMaterial.SetTexture(m_GaussBuraUAV_P2.GetTexture(), 0);


	mCBBloomParam = ConstantBuffer<cbHDRBloomParam>::create(1);
	mCBBloomParam.mParam.BloomParam = XMFLOAT4(0.5f, 1,1,1);
	if (hdr){
		mCBBloomParam.mParam.BloomParam.x = 1.5f;
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
		//�����o����1:1����Ȃ���Δ��]
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

	//�����o����1:1����Ȃ���Δ��]��߂�
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


DeferredRendering::DeferredRendering(){

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

#include "../DirectXTex/DirectXTex.h"
void LoadImg(const std::string& filename, DirectX::ScratchImage& img){

	WCHAR f[256];
	size_t wLen = 0;
	//���P�[���w��
	setlocale(LC_ALL, "japanese");
	//�ϊ�
	mbstowcs_s(&wLen, f, 255, filename.c_str(), _TRUNCATE);

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr;
	for (;;){
		hr = DirectX::LoadFromWICFile(f, 0, &metadata, img);
		if (SUCCEEDED(hr)){
			break;
		}
		hr = DirectX::LoadFromTGAFile(f, &metadata, img);
		if (SUCCEEDED(hr)){
			break;
		}
		hr = DirectX::LoadFromDDSFile(f, 0, &metadata, img);
		if (SUCCEEDED(hr)){
			break;
		}
		break;
	}

}
void CreateCubeMap(const std::string& filename,Texture& out){
		HRESULT hr;
		//DirectX::TexMetadata metadata;
		DirectX::ScratchImage image[6];
		//LoadImg(filename + "_ft.tga", image[0]);
		//LoadImg(filename + "_bk.tga", image[1]);
		//LoadImg(filename + "_up.tga", image[2]);
		//LoadImg(filename + "_dn.tga", image[3]);
		//LoadImg(filename + "_rt.tga", image[4]);
		//LoadImg(filename + "_lf.tga", image[5]);
		LoadImg(filename + "_c00.hdr", image[0]);
		LoadImg(filename + "_c01.hdr", image[1]);
		LoadImg(filename + "_c02.hdr", image[2]);
		LoadImg(filename + "_c03.hdr", image[3]);
		LoadImg(filename + "_c04.hdr", image[4]);
		LoadImg(filename + "_c05.hdr", image[5]);


		DirectX::ScratchImage cubeimage;
		DirectX::Image images[] =
		{ 
			*image[0].GetImage(0, 0, 0),
			*image[1].GetImage(0, 0, 0),
			*image[2].GetImage(0, 0, 0),
			*image[3].GetImage(0, 0, 0),
			*image[4].GetImage(0, 0, 0),
			*image[5].GetImage(0, 0, 0)
		};
		cubeimage.InitializeCubeFromImages(images, 6);
		ID3D11ShaderResourceView* temp = NULL;

		DirectX::ScratchImage mipChain;
		hr = DirectX::GenerateMipMaps(cubeimage.GetImages(), cubeimage.GetImageCount(), cubeimage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
		if (FAILED(hr)){
			mipChain.Release();
			for (int i = 0; i < 6; i++){
				image[i].Release();
			}
			cubeimage.Release();
		}

		// �摜����V�F�[�_���\�[�XView DirectXTex
		hr = DirectX::CreateShaderResourceView(Device::mpd3dDevice, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), &temp);
		for (int i = 0; i < 6; i++){
			image[i].Release();
		}
		cubeimage.Release();
		mipChain.Release();
		if (FAILED(hr)){
			return;
		}

		out.Create(temp);
}
void CreateCubeMapDDS(const std::string& filename, Texture& out){
	HRESULT hr;
	//DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	LoadImg(filename, image);


	DirectX::ScratchImage cubeimage;
	cubeimage.InitializeCubeFromImages(image.GetImages(), 6);
	ID3D11ShaderResourceView* temp = NULL;

	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(cubeimage.GetImages(), cubeimage.GetImageCount(), cubeimage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)){
		mipChain.Release();
		image.Release();
		cubeimage.Release();
	}

	// �摜����V�F�[�_���\�[�XView DirectXTex
	hr = DirectX::CreateShaderResourceView(Device::mpd3dDevice, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), &temp);

	image.Release();
	cubeimage.Release();
	mipChain.Release();
	if (FAILED(hr)){
		return;
	}

	out.Create(temp);
}

#include "Graphic/Loader/HDR.h"

//int GetNumMipLevels(int width, int height)
//{
//	int numLevels = 1;
//	while ((width > 1) || (height > 1))
//	{
//		width = max(width / 2, 1);
//		height = max(height / 2, 1);
//		++numLevels;
//	}
//
//	return numLevels;
//}
//
//void CreatePanoramaHDR(const std::string& filename, Texture& out){
//	HRESULT hr;
//	HDRLoaderResult result;
//	bool temp = true;
//	temp = temp&&HDRLoader::load(filename.c_str(), result);
//	if (!temp)return;
//	int mipnum = GetNumMipLevels(result.width, result.height);
//
//	D3D11_TEXTURE2D_DESC tex_desc;
//	ZeroMemory(&tex_desc, sizeof(tex_desc));
//	tex_desc.ArraySize = 1;
//	tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	tex_desc.SampleDesc.Count = 1;
//	tex_desc.SampleDesc.Quality = 0;
//	tex_desc.Usage = D3D11_USAGE_DEFAULT;
//	//�����_�[�^�[�Q�b�g�Ƃ��Ďg�p���V�F�[�_���\�[�X�Ƃ��ė��p
//	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
//	tex_desc.CPUAccessFlags = 0;
//	tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
//
//	tex_desc.Width = result.width;
//	tex_desc.Height = result.height;
//
//	tex_desc.MipLevels = mipnum;
//
//	std::vector<D3D11_SUBRESOURCE_DATA> data(mipnum);
//	for (auto& d : data){
//		d.pSysMem = result.cols;
//		d.SysMemPitch = result.width * sizeof(float) * 4;
//		d.SysMemSlicePitch = 0;
//	}
//
//	ID3D11Texture2D *tex2d;
//	hr = Device::mpd3dDevice->CreateTexture2D(&tex_desc, &data.data()[0], &tex2d);
//	if (FAILED(hr)){
//		delete[] result.cols;
//		return;
//	}
//
//	
//
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
//	memset(&srv_desc, 0, sizeof(srv_desc));
//	srv_desc.Format = tex_desc.Format;
//	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srv_desc.Texture2D.MostDetailedMip = 0;
//	srv_desc.Texture2D.MipLevels = tex_desc.MipLevels;
//
//	ID3D11ShaderResourceView* pShaderResourceView;
//	hr = Device::mpd3dDevice->CreateShaderResourceView(tex2d, &srv_desc, &pShaderResourceView);
//	if (FAILED(hr)){
//		return;
//	}
//
//	tex2d->Release();
//	delete[] result.cols;
//
//	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
//	render->m_Context->GenerateMips(pShaderResourceView);
//
//	out.Create(pShaderResourceView);
//}

void CreateCubeMapHDR(const std::string& filename, Texture& out){
	HRESULT hr;
	D3D11_TEXTURE2D_DESC tex_desc;
	ZeroMemory(&tex_desc, sizeof(tex_desc));
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 6;
	tex_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	//�����_�[�^�[�Q�b�g�Ƃ��Ďg�p���V�F�[�_���\�[�X�Ƃ��ė��p
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HDRLoaderResult result[6];

	bool temp = true;
	temp = temp&&HDRLoader::load((filename + "_c00.hdr").c_str(), result[0]);
	temp = temp&&HDRLoader::load((filename + "_c01.hdr").c_str(), result[1]);
	temp = temp&&HDRLoader::load((filename + "_c02.hdr").c_str(), result[2]);
	temp = temp&&HDRLoader::load((filename + "_c03.hdr").c_str(), result[3]);
	temp = temp&&HDRLoader::load((filename + "_c04.hdr").c_str(), result[4]);
	temp = temp&&HDRLoader::load((filename + "_c05.hdr").c_str(), result[5]);
	
	//temp = temp&&HDRLoader::load("EngineResource/Sky/SkyBox2/nave.hdr", result[0]);
	if (!temp)return;

	//FILE* f[6];
	//f[0] = fopen((filename + "_c00.hdr").c_str(), "rb");
	//f[1] = fopen((filename + "_c01.hdr").c_str(), "rb");
	//f[2] = fopen((filename + "_c02.hdr").c_str(), "rb");
	//f[3] = fopen((filename + "_c03.hdr").c_str(), "rb");
	//f[4] = fopen((filename + "_c04.hdr").c_str(), "rb");
	//f[5] = fopen((filename + "_c05.hdr").c_str(), "rb");
	//for (int i = 0; i < 6; i++){
	//	ReadHeader(f[i], result[i]);
	//	//RGBE_ReadHeader(f[i], &result[i].width, &result[i].height, NULL);
	//	//result[i].cols = (float *)malloc(sizeof(float) * 3 * result[i].width*result[i].height);
	//	RGBE_ReadPixels_RLE(f[i], result[i].cols, result[i].width,result[i].height);
	//	fclose(f[i]);
	//}

	D3D11_SUBRESOURCE_DATA data[6];
	for (int i = 0; i < 6; i++){

		data[i].pSysMem = result[i].cols;
		auto size = sizeof(float);
		data[i].SysMemPitch = result[i].width * size * 3;
		data[i].SysMemSlicePitch = 0;
	}

	tex_desc.Width = result[0].width;
	tex_desc.Height = result[0].height;


	ID3D11Texture2D *tex2d;
	hr = Device::mpd3dDevice->CreateTexture2D(&tex_desc,&data[0], &tex2d);
	if (FAILED(hr)){
		for (int i = 0; i < 6; i++){
			delete[] result[i].cols;
		}
		return;
	}
	

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	memset(&srv_desc, 0, sizeof(srv_desc));
	srv_desc.Format = tex_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srv_desc.TextureCube.MostDetailedMip = 0;
	srv_desc.TextureCube.MipLevels = tex_desc.MipLevels;

	ID3D11ShaderResourceView* pShaderResourceView;
	hr = Device::mpd3dDevice->CreateShaderResourceView(tex2d, &srv_desc, &pShaderResourceView);
	if (FAILED(hr)){
		return;
	}

	tex2d->Release();
	for (int i = 0; i < 6; i++){
		delete[] result[i].cols;
	}

	out.Create(pShaderResourceView);
}
void DeferredRendering::Initialize(){

	_SYSTEM_LOG_H("�f�t�@�[�h�����_�����O�̏�����");
	auto w = WindowState::mWidth;
	auto h = WindowState::mHeight;
	m_AlbedoRT.Create(w, h, DXGI_FORMAT_R11G11B10_FLOAT);
	m_SpecularRT.Create(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);//DXGI_FORMAT_R8G8B8A8_UINT
	m_NormalRT.Create(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_DepthRT.Create(w, h, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_VelocityRT.Create(w, h, DXGI_FORMAT_R16G16_FLOAT);
	m_LightRT.Create(w, h, DXGI_FORMAT_R11G11B10_FLOAT);
	m_LightSpecularRT.Create(w, h, DXGI_FORMAT_R11G11B10_FLOAT);

	//mEnvironmentMap.Create("EngineResource/Sky/SkyBox/wells6_hd.hdr");
	//mEnvironmentMap.Create("EngineResource/Sky/a.png");
	//mEnvironmentRMap.Create("EngineResource/Sky/spheremapr.jpg");

	//CreateCubeMap("EngineResource/Sky/SkyBox2/SkyBox", mEnvironmentMap);
	//CreateCubeMapHDR("EngineResource/Sky/SkyBox2/skybox", mEnvironmentMap);
	//CreatePanoramaHDR("EngineResource/Sky/SkyBox/wells6_hd.hdr", mEnvironmentMap);

	mModelTexture.Create("EngineResource/TextureModel.tesmesh");


	mMaterialPrePassEnv.Create("");
	mMaterialPrePassEnv.SetTexture(mEnvironmentMap, 6);

	mMaterialSkyBox.Create("EngineResource/ScreenClearSkyBox.fx");
	mMaterialSkyBox.SetTexture(mEnvironmentMap, 6);


	mMaterialLight.Create("EngineResource/DeferredLightRendering.fx");
	mMaterialLight.SetTexture(m_NormalRT.GetTexture(), 0);
	mMaterialLight.SetTexture(m_DepthRT.GetTexture(), 1);
	mMaterialLight.SetTexture(m_SpecularRT.GetTexture(), 2);
	for (int i = 0; i < mCascadeShadow.MAX_CASCADE; i++) {
		mMaterialLight.SetTexture(mCascadeShadow.GetRTTexture(i), i+3);
	}

	mMaterialSkyLight.Create("EngineResource/SkyLightRendering.fx");
	mMaterialSkyLight.SetTexture(m_NormalRT.GetTexture(), 0);
	mMaterialSkyLight.SetTexture(m_DepthRT.GetTexture(), 1);
	mMaterialSkyLight.SetTexture(m_SpecularRT.GetTexture(), 2);
	mMaterialSkyLight.SetTexture(mEnvironmentMap, 7);

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

	mCBBloomParam = ConstantBuffer<cbFreeParam>::create(10);

}


void DeferredRendering::SetSkyTexture(const TextureAsset& texture){
	mEnvironmentMap = texture;
	mMaterialPrePassEnv.SetTexture(mEnvironmentMap, 6);
	mMaterialSkyBox.SetTexture(mEnvironmentMap, 6);
	mMaterialSkyLight.SetTexture(mEnvironmentMap, 7);

}
void DeferredRendering::G_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){

	const RenderTarget* r[5] = { &m_AlbedoRT, &m_SpecularRT, &m_NormalRT, &m_DepthRT, &m_VelocityRT };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)5, r[0], &Game::GetMainViewRenderTarget());

	mMaterialPrePassEnv.PSSetShaderResources(render->m_Context);

	func();

	render->PushSet(DepthStencil::Preset::DS_Zero_Less);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);
	mModelTexture.Update();
	mModelTexture.Draw(render->m_Context, mMaterialSkyBox);
	render->PopRS();
	render->PopDS();


}
void DeferredRendering::ShadowDepth_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){


	D3D11_VIEWPORT backvp;
	UINT num = 1;
	render->m_Context->RSGetViewports(&num, &backvp);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)mCascadeShadow.GetWidth();
	vp.Height = (FLOAT)mCascadeShadow.GetHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	render->m_Context->RSSetViewports(1, &vp);

	render->PushSet(Rasterizer::Preset::RS_Back_Solid);
	mCascadeShadow.Update(render);

	Model::mForcedMaterial = &mMaterialDepthShadow;
	Model::mForcedMaterialFilter = (ForcedMaterialFilter::Enum)(ForcedMaterialFilter::Shader_PS | ForcedMaterialFilter::Texture_ALL | ForcedMaterialFilter::Color ^ ForcedMaterialFilter::Texture_0);

	for (int i = 0; i < mCascadeShadow.MAX_CASCADE; i++){
		mCascadeShadow.ClearView(render, i);
		mCascadeShadow.SetRT(render, i);
		mCascadeShadow.SetupShadowCB(render, i);

		func();
	}
	Model::mForcedMaterial = NULL;
	Model::mForcedMaterialFilter = ForcedMaterialFilter::None;
	mCascadeShadow.SetupShadowCB(render, 0);

	render->PopRS();

	render->m_Context->RSSetViewports(1, &backvp);
}

void DeferredRendering::Light_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){

	m_LightRT.ClearView(render->m_Context);
	m_LightSpecularRT.ClearView(render->m_Context);

	const RenderTarget* r[2] = { &m_LightRT, &m_LightSpecularRT };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)2, r[0], &Game::GetMainViewRenderTarget());


	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(BlendState::Preset::BS_Add,0xFFFFFFFF);

	//�e�N�X�`���[�̃Z�b�g
	mMaterialLight.PSSetShaderResources(render->m_Context);

	func();


	mModelTexture.Draw(render->m_Context, mMaterialSkyLight);


	render->PopBS();
	render->PopDS();
}

#include "Input/Input.h"
void DeferredRendering::Deferred_Rendering(IRenderingEngine* render, RenderTarget* rt){

	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], &Game::GetMainViewRenderTarget());

	mModelTexture.Update();

	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);

	mModelTexture.Draw(render->m_Context, mMaterialDeferred);

	render->PopRS();
	render->PopDS();

}

void DeferredRendering::Particle_Rendering(IRenderingEngine* render, RenderTarget* rt, const std::function<void(void)>& func){

	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], &Game::GetMainViewRenderTarget());

	mMaterialParticle.GSSetShaderResources(render->m_Context);
	mMaterialParticle.PSSetShaderResources(render->m_Context);

	func();

	RenderTarget::NullSetRendererTarget(render->m_Context);
	//�f�o�b�O�p�ł����ɂЂ悤�H
	mMaterialPostEffect.PSSetShaderResources(render->m_Context);
}

void DeferredRendering::Forward_Rendering(IRenderingEngine* render, RenderTarget* rt, const std::function<void(void)>& func){

	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], &Game::GetMainViewRenderTarget());

	render->PushSet(BlendState::Preset::BS_Alpha, 0xFFFFFFFF);
	render->PushSet(DepthStencil::Preset::DS_Zero_Less);

	func();

	render->PopBS();
	render->PopDS();
}

void DeferredRendering::Debug_G_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func){
	const RenderTarget* r[1] = { &m_AlbedoRT};
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], &Game::GetMainViewRenderTarget());

	Model::mForcedMaterial = &mMaterialDebugDrawPrePass;
	Model::mForcedMaterialFilter = (ForcedMaterialFilter::Enum)(ForcedMaterialFilter::Shader_PS | ForcedMaterialFilter::Shader_VS);

	func();

	Model::mForcedMaterial = NULL;
	Model::mForcedMaterialFilter = ForcedMaterialFilter::None;
	{
		const RenderTarget* r[5] = { &m_AlbedoRT, &m_SpecularRT, &m_NormalRT, &m_DepthRT, &m_VelocityRT };
		RenderTarget::SetRendererTarget(render->m_Context, (UINT)5, r[0], &Game::GetMainViewRenderTarget());

		render->PushSet(DepthStencil::Preset::DS_Zero_Less);
		render->PushSet(Rasterizer::Preset::RS_None_Solid);
		mModelTexture.Update();
		mModelTexture.Draw(render->m_Context, mMaterialSkyBox);
		render->PopRS();
		render->PopDS();
	}
}

void DeferredRendering::Debug_AlbedoOnly_Rendering(IRenderingEngine* render,RenderTarget* rt){
	RenderTarget::NullSetRendererTarget(render->m_Context);
	RenderTarget* r[1] = { rt };
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, r[0], &Game::GetMainViewRenderTarget());

	mModelTexture.Update();

	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);

	mModelTexture.Draw(render->m_Context, mMaterialDebugDraw);

	render->PopRS();
	render->PopDS();

}


void DeferredRendering::HDR_Rendering(IRenderingEngine* render){

	for (UINT i = 0; i < mHDLDownSampleNum; i++){
		mHDLDownSample[i].Draw_DownSample(render);
	}

	render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	render->PushSet(Rasterizer::Preset::RS_None_Solid);
	render->PushSet(BlendState::Preset::BS_Add, 0xFFFFFFFF);

	// Setup the viewport
	//D3D11_VIEWPORT vp;
	//vp.Width = (FLOAT)WindowState::mWidth;
	//vp.Height = (FLOAT)WindowState::mHeight;
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;
	//vp.TopLeftX = 0;
	//vp.TopLeftY = 0;
	//render->m_Context->RSSetViewports(1, &vp);

	auto rt = Game::GetMainViewRenderTarget();
	RenderTarget::SetRendererTarget(render->m_Context, (UINT)1, &rt, NULL);
	
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//render->m_Context->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);
	
	float pow[] = { 0.5f,0.5f,0.5f,0.5f };
	for (UINT i = 0; i < mHDLDownSampleNum; i++){

		mCBBloomParam.mParam.free = XMFLOAT4(pow[i], pow[i], pow[i], pow[i]);
		mCBBloomParam.UpdateSubresource(render->m_Context);
		mCBBloomParam.PSSetConstantBuffers(render->m_Context);

		mModelTexture.Draw(render->m_Context, mHDLDownSample[i].GetResultMaterial());
	}

	//render->m_Context->OMSetBlendState(NULL, blendFactor, 0xffffffff);

	render->PopRS();
	render->PopDS();
	render->PopBS();


	RenderTarget::NullSetRendererTarget(render->m_Context);
	mMaterialPostEffect.PSSetShaderResources(render->m_Context);


	//		if (Input::Trigger(KeyCode::Key_6)) {
	//			mMaterialLight.CreateShader("EngineResource/DeferredLightRendering.fx");
	//		}
	//if (Input::Down(KeyCode::Key_1)) {
	//	Game::AddDrawList(DrawStage::UI, [&]() {


	//		auto rend = RenderingEngine::GetEngine(ContextType::MainDeferrd);

	//		rend->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
	//		rend->PushSet(Rasterizer::Preset::RS_None_Solid);
	//		if (Input::Trigger(KeyCode::Key_2)) {
	//			mMaterialDebugDraw.SetTexture(mCascadeShadow.GetRTTexture(0));
	//		}
	//		if (Input::Trigger(KeyCode::Key_3)) {
	//			mMaterialDebugDraw.SetTexture(mCascadeShadow.GetRTTexture(1));
	//		}
	//		if (Input::Trigger(KeyCode::Key_4)) {
	//			mMaterialDebugDraw.SetTexture(mCascadeShadow.GetRTTexture(2));
	//		}
	//		if (Input::Trigger(KeyCode::Key_5)) {
	//			mMaterialDebugDraw.SetTexture(mCascadeShadow.GetRTTexture(3));
	//		}
	//		auto s = XMMatrixScaling(512.0f/1920.0f, 512.0f / 1080.0f, 1);
	//		auto t = XMMatrixTranslation(-0.5, -0.5, 0);
	//		mModelTexture.mWorld = XMMatrixMultiply(s, t);
	//		mModelTexture.Update();

	//		mModelTexture.Draw(rend->m_Context, mMaterialDebugDraw);

	//		rend->PopRS();
	//		rend->PopDS();
	//	});
	//}
	
}


void DeferredRendering::Resize(UINT Width, UINT Height){
	m_AlbedoRT.Resize(Width, Height);
	m_SpecularRT.Resize(Width, Height);
	m_NormalRT.Resize(Width, Height);
	m_DepthRT.Resize(Width, Height);
	m_VelocityRT.Resize(Width, Height);
	m_LightRT.Resize(Width, Height);
	m_LightSpecularRT.Resize(Width, Height);
}