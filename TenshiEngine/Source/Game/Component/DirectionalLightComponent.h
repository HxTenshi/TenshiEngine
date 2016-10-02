#pragma once

#include "IComponent.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Material/Material.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

class IDirectionalLightComponent{
public:
	virtual ~IDirectionalLightComponent(){}
	virtual void SetColor(XMFLOAT4 color) = 0;
};

class DirectionalLightComponent :public IDirectionalLightComponent,public Component{
public:

	DirectionalLightComponent();
	~DirectionalLightComponent();

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

	XMFLOAT4 GetColor(){ return m_Color; }
	void SetColor(XMFLOAT4 color) override;

private:
	//ÉJÉâÅ[
	XMFLOAT4 m_Color;
	float m_HDR;

	ConstantBuffer<cbChangesLight>		mCBChangeLgiht;
	//ConstantBuffer<cbChangesMaterial>	mCBChangeLgihtMaterial;

	Material mMaterial;
	Model mModelTexture;

};
