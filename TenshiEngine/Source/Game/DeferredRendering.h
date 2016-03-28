#pragma once

#include "Graphic/Model/Model.h"
#include "Graphic/RenderTarget/RenderTarget.h"

class CascadeShadow{
public:
	CascadeShadow();
	~CascadeShadow();
	void Update();

	void ClearView(int no);
	void SetupShadowCB(int no);
	void SetRT(int no);
	Texture& GetRTTexture(int no);

	static void SetLightVect(const XMVECTOR& vect);

private:
	void CascadeUpdate();
	void ComputeSplitPositions(int splitCount, float lamda, float nearClip, float farClip, float* positions);
	void CalculateFrustum(float nearClip, float farClip, const XMMATRIX& viewProj, XMVECTOR* minPos, XMVECTOR* maxPos);
	XMMATRIX CreateCropMatrix(const XMVECTOR& mini, const XMVECTOR& maxi);

	static const int MAX_CASCADE = 4;
	XMMATRIX m_ShadowMatrix[MAX_CASCADE];
	float m_SplitPos[MAX_CASCADE];
	XMMATRIX m_LightView;
	XMMATRIX m_LightProj;


	RenderTarget m_ShadowDepthRT[MAX_CASCADE];
	RenderTarget m_ShadowDepthDS;

	ConstantBuffer<cbChangesLightCamera> mCBChangesLightCamera;

	static XMVECTOR mLightVect;
};


class DeferredRendering{
public:
	~DeferredRendering();
	void Initialize();
	void Start_G_Buffer_Rendering();
	void Start_ShadowDepth_Buffer_Rendering(int no);
	void End_ShadowDepth_Buffer_Rendering();
	void Start_Light_Rendering();
	void End_Light_Rendering();
	void Start_Deferred_Rendering(RenderTarget* rt);

private:
	RenderTarget m_AlbedoRT;
	RenderTarget m_NormalRT;
	RenderTarget m_DepthRT;
	RenderTarget m_LightRT;

	Model mModelTexture;
	//Material mMaterialDebugDraw;
	Material mMaterialLight;
	Material mMaterialDeferred;
	Material mMaterialDepthShadow;

	CascadeShadow mCascadeShadow;

	ID3D11BlendState* pBlendState = NULL;
};