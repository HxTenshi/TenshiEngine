#pragma once

#include "Graphic/Model/Model.h"

class IRenderingEngine;

class PostEffectRendering{
public:
	PostEffectRendering();
	~PostEffectRendering();
	void Initialize();

	void Rendering(IRenderingEngine* render, const std::function<void(void)>& func);
	void Flip(IRenderingEngine* render);

private:
	Model mModelTexture;
	Material mMaterial;
};