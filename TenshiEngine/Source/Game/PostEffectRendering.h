#pragma once

#include "Graphic/Model/Model.h"

class PostEffectRendering{
public:
	PostEffectRendering();
	~PostEffectRendering();
	void Initialize();

	void Rendering();

private:
	Model mModelTexture;
	Material mMaterial;
};