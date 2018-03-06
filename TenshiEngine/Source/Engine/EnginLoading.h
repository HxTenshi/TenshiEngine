#pragma once
#include "MySTL/ptr.h"
class Actor;
class Model;
class Material;
struct Texs;
class SamplerStateSetting;
class EnginLoading {
public:
	EnginLoading();
	~EnginLoading();
	void Update();
	void Draw();
private:
	Model* mModel;
	Material* mMaterial;
	Texs* imp;
	SamplerStateSetting* mSamplerStateSetting;
};