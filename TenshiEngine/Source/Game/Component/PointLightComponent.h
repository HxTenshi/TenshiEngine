#pragma once

#include "IComponent.h"

#include "Graphic/Model/Model.h"
#include "Graphic/Material/Material.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

class PointLightComponent :public Component{
public:

	PointLightComponent();
	~PointLightComponent();

	void Initialize() override;
	void Finish() override;
	void EngineUpdate() override;
	void Update() override;
	void CreateInspector() override;
	void IO_Data(I_ioHelper* io) override;

	//影響範囲半径
	float m_Radius;
	//減衰開始
	float m_AttenuationStart;
	//減衰曲線パラメータ
	float m_AttenuationParam;
	//カラー
	XMFLOAT3 m_Color;

	Material mMaterial;

	Model mModel;

	ConstantBuffer<cbChangesPointLight> mPointLightBuffer;

};
