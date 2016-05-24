#pragma once

#include "IComponent.h"

#include "Graphic/Shader/GeometryShader.h"
#include "Graphic/Shader/VertexShader.h"
#include "Graphic/Shader/PixelShader.h"

#include "MySTL/ptr.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

#include <array>

class MaterialComponent;

class ParticleComponent :public Component{
public:
	struct ParticleVertex{
		XMFLOAT3 pos;
		XMFLOAT3 v0;
		XMFLOAT3 v;
		FLOAT time;

	};

	ParticleComponent();
	~ParticleComponent();
	void EngineUpdate() override;
	void Update() override;
	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;

	void ParticleCapacityChange(UINT num);


	bool mInitialize;
	GeometryStreamOutputShader mGeometryShader0;
	GeometryStreamOutputShader mGeometryShader1;
	VertexShader	mVertexShader;
	PixelShader		mPixelShader;
	ID3D11Buffer*	mpSOBuffer[2];
	int mParticleCapacity;

	weak_ptr<MaterialComponent> mMaterial;
	ConstantBuffer<CBChangesEveryFrame> mCBuffer;
	ConstantBuffer<CBChangesPaticleParam> mCParticleBuffer;
	CBChangesPaticleParam mParticleParam;
	bool mBlendAdd;
	bool mFirstDraw;

};
