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
		XMFLOAT3 time;

	};

	ParticleComponent();
	~ParticleComponent();
	void Initialize() override;
	void EngineUpdate() override;
	void Update() override;
#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void ParticleCapacityChange(UINT num);


	bool mInitialize;
	ID3D11Buffer*	mpSOBuffer[2];
	int mParticleCapacity;

	weak_ptr<MaterialComponent> mMaterial;
	ConstantBuffer<CBChangesEveryFrame> mCBuffer;
	ConstantBuffer<CBChangesPaticleParam> mCParticleBuffer;
	CBChangesPaticleParam mParticleParam;
	bool mBlendAdd;
	bool mAutoDestroy;
	float mTimer;
	bool mFirstDraw;
	bool mEngineUpdate;
};
