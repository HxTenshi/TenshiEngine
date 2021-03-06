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
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;
#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif
	void IO_Data(I_ioHelper* io) override;

	//�e���͈͔��a
	float m_Radius;
	//�����J�n
	float m_AttenuationStart;
	//�����Ȑ��p�����[�^
	float m_AttenuationParam;
	//�J���[
	XMFLOAT4 m_Color;
	float m_HDR;

	Material mMaterial;

	Model mModel;

	ConstantBuffer<cbChangesPointLight> mPointLightBuffer;

};
