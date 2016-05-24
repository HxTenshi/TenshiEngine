#pragma once

#include "IComponent.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Material/Material.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

class IDirectionalLightComponent{
public:
	virtual ~IDirectionalLightComponent(){}
	virtual void SetColor(XMFLOAT3 color) = 0;
};

class DirectionalLightComponent :public IDirectionalLightComponent,public Component{
public:

	DirectionalLightComponent();
	~DirectionalLightComponent();

	void Initialize() override;
	void Finish() override;
	void EngineUpdate() override;
	void Update() override;
	void CreateInspector() override;
	void IO_Data(I_ioHelper* io) override;

	XMFLOAT3 GetColor(){ return m_Color; }
	void SetColor(XMFLOAT3 color) override;

private:
	//ÉJÉâÅ[
	XMFLOAT3 m_Color;

	ConstantBuffer<cbChangesLight>		mCBChangeLgiht;
	//ConstantBuffer<cbChangesMaterial>	mCBChangeLgihtMaterial;

	Material mMaterial;
	Model mModelTexture;

};
