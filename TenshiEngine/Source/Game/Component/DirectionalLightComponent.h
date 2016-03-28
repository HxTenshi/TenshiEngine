#pragma once

#include "IComponent.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Material/Material.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

class DirectionalLightComponent :public Component{
public:

	DirectionalLightComponent();
	~DirectionalLightComponent();

	void Initialize() override;
	void Finish() override;
	void EngineUpdate() override;
	void Update() override;
	void CreateInspector() override;
	void IO_Data(I_ioHelper* io) override;

	void SetColor(XMFLOAT3 color);

private:
	//ÉJÉâÅ[
	XMFLOAT3 m_Color;

	ConstantBuffer<cbChangesLight>		mCBChangeLgiht;
	//ConstantBuffer<cbChangesMaterial>	mCBChangeLgihtMaterial;

	Material mMaterial;
	Model mModelTexture;

};
