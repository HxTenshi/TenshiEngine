#pragma once

#include "Graphic/Model/Model.h"
#include "Graphic/RenderTarget/RenderTarget.h"


class DeferredRendering{
public:
	~DeferredRendering();
	void Initialize();
	void Start_G_Buffer_Rendering();
	void Start_Light_Rendering();
	void End_Light_Rendering();
	void Start_Deferred_Rendering(RenderTarget* rt);

private:
	RenderTarget m_AlbedoRT;
	RenderTarget m_NormalRT;
	RenderTarget m_DepthRT;
	RenderTarget m_LightRT;
	Model mModelTexture;
	Material mMaterialLight;
	Material mMaterialDeferred;

	ID3D11BlendState* pBlendState = NULL;
};