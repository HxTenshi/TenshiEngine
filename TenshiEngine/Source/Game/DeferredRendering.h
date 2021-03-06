#pragma once

#include "Graphic/Model/Model.h"
#include "Graphic/RenderTarget/RenderTarget.h"

#include "Graphic/Shader/ComputeShader.h"

class IRenderingEngine;





class DownSample{
public:
	DownSample();
	~DownSample();
	void Create(Texture& texture, UINT xy);
	void Create(Texture& texture, UINT x, UINT y);

	void Draw_DownSample(IRenderingEngine* render);

	Texture& GetRTTexture();

	void SetHDRFilter(bool flag){
		mHDRFilter = flag;
	}
	bool GetHDRFilter(){
		return mHDRFilter;
	}

private:
	bool mHDRFilter;
	Model mModelTexture;
	RenderTarget m_DownSampleRT;
	Material mMaterialDownSample;
	UINT mWidth;
	UINT mHeight;
};


class HDRGaussBulr_AND_DownSample{
public:
	HDRGaussBulr_AND_DownSample();
	~HDRGaussBulr_AND_DownSample();
	void Create(Texture& texture, UINT xy);
	void Create(Texture& texture, UINT x, UINT y);

	void Draw_DownSample(IRenderingEngine* render);

	Texture& GetRTTexture();
	Material& GetResultMaterial();

	void SetHDRFilter(bool flag);

private:
	ComputeShader m_GaussBuraCS;
	UAVRenderTarget m_GaussBuraUAV_P1;
	UAVRenderTarget m_GaussBuraUAV_P2;
	Material mResultMaterial;

	ConstantBuffer<cbTextureSize> mCBTextureSize;
	ConstantBuffer<cbHDRBloomParam> mCBBloomParam;

	DownSample m_DownSample;
};



class CascadeShadow{
public:
	CascadeShadow();
	~CascadeShadow();
	void Update(IRenderingEngine* render);

	void ClearView(IRenderingEngine* render,int no);
	void SetupShadowCB(IRenderingEngine* render,int no);
	void SetRT(IRenderingEngine* render,int no);
	Texture& GetRTTexture(int no);

	static void SetLightVect(const XMVECTOR& vect);

	UINT GetWidth() {
		return mWidth;
	}
	UINT GetHeight() {
		return mHeight;
	}
	static const int MAX_CASCADE = 4;

private:
	void OrthographicSize(XMMATRIX& matrix, XMVECTOR& mini, XMVECTOR& maxi);
	void CascadeUpdate();
	void ComputeSplitPositions(int splitCount, float lamda, float nearClip, float farClip, float* positions);
	void CalculateFrustum(float nearClip, float farClip, const XMMATRIX& viewProj, XMVECTOR* minPos, XMVECTOR* maxPos);
	XMMATRIX CreateCropMatrix(const XMVECTOR& mini, const XMVECTOR& maxi);

	XMMATRIX m_ShadowMatrix[MAX_CASCADE];
	float m_SplitPos[MAX_CASCADE];
	XMMATRIX m_LightView;
	XMMATRIX m_LightProj;

	UINT mWidth;
	UINT mHeight;

	RenderTarget m_ShadowDepthRT[MAX_CASCADE];
	RenderTarget m_ShadowDepthDS;

	ConstantBuffer<CBNeverChanges> mCBNeverChanges;
	ConstantBuffer<CBChangeOnResize> mCBChangeOnResize;
	ConstantBuffer<cbChangesLightCamera> mCBChangesLightCamera;

	static XMVECTOR mLightVect;
};


class DeferredRendering{
public:
	DeferredRendering();
	~DeferredRendering();
	void Initialize();
	void SetSkyTexture(const TextureAsset& texture);
	void G_Buffer_Rendering(IRenderingEngine* render,const std::function<void(void)>& func);
	void ShadowDepth_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func);
	//void End_ShadowDepth_Buffer_Rendering();
	void Light_Rendering(IRenderingEngine* render, const std::function<void(void)>& func);
	//void End_Light_Rendering(IRenderingEngine* render);
	void Deferred_Rendering(IRenderingEngine* render, RenderTarget* rt);

	void Particle_Rendering(IRenderingEngine* render, RenderTarget* rt, const std::function<void(void)>& func);

	void Forward_Rendering(IRenderingEngine* render, RenderTarget* rt, const std::function<void(void)>& func);


	void HDR_Rendering(IRenderingEngine* render);


	void Debug_G_Buffer_Rendering(IRenderingEngine* render, const std::function<void(void)>& func);
	void Debug_AlbedoOnly_Rendering(IRenderingEngine* render,RenderTarget* rt);


	void Resize(UINT Width, UINT Height);

private:
	RenderTarget m_AlbedoRT;
	RenderTarget m_SpecularRT;
	RenderTarget m_NormalRT;
	RenderTarget m_DepthRT;
	RenderTarget m_VelocityRT;
	RenderTarget m_LightRT;
	RenderTarget m_LightSpecularRT;

	TextureAsset mEnvironmentMap;
	Model mModelTexture;
	Material mMaterialDebugDraw;
	Material mMaterialDebugDrawPrePass;
	Material mMaterialPrePassEnv;
	Material mMaterialLight;
	Material mMaterialSkyLight;
	Material mMaterialDeferred;
	Material mMaterialDepthShadow;
	Material mMaterialParticle;
	Material mMaterialPostEffect;
	Material mMaterialSkyBox;

	CascadeShadow mCascadeShadow;

	const UINT mHDLDownSampleNum = 4;
	HDRGaussBulr_AND_DownSample mHDLDownSample[4];


	ConstantBuffer<cbFreeParam> mCBBloomParam;


	DeferredRendering(const DeferredRendering &);
	DeferredRendering& operator=(const DeferredRendering &);
};