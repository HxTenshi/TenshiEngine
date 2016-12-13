#pragma once

#include "IComponent.h"
#include "XNAMath/xnamath.h"

#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"
#include "Game/Component/MaterialComponent.h"
#include "Graphic/Material/Material.h"
#include "Graphic/Model/Model.h"

class EditorCamera;

class ICameraComponent :public Component {
public:
	virtual ~ICameraComponent(){}
	virtual float GetNear() = 0;
	virtual float GetFar() = 0;
	virtual const XMMATRIX& GetViewMatrix() = 0;
	virtual const XMMATRIX& GetProjectionMatrix() = 0;
	virtual const XMMATRIX GetBillboardMatrix() = 0;
	virtual XMVECTOR Project(const XMVECTOR& position) = 0;
	virtual XMVECTOR UnProject(const XMVECTOR& position) = 0;
	virtual void SetPerspective() = 0;
	virtual void SetOrthographic() = 0;
	virtual void SetSkyTexture(TextureAsset asset) = 0;
	virtual TextureAsset GetSkyTexture() = 0;
};

class CameraComponent :public ICameraComponent {
public:
	CameraComponent();
	~CameraComponent();

	void Initialize() override;
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void VSSetConstantBuffers(ID3D11DeviceContext* context) const;
	void PSSetConstantBuffers(ID3D11DeviceContext* context) const;
	void GSSetConstantBuffers(ID3D11DeviceContext* context) const;

	float GetNear() override;
	float GetFar() override;

	const XMMATRIX& GetViewMatrix() override;
	const XMMATRIX& GetProjectionMatrix() override;
	const XMMATRIX GetBillboardMatrix() override;

	XMVECTOR Project(const XMVECTOR& position) override;
	XMVECTOR UnProject(const XMVECTOR& position) override;

	enum class ScreenClearType{
		Color,
		Sky,
		None,
		Count
	};

	void ScreenClear();

	void SetPerspective() override;
	void SetOrthographic() override;
	void SetSkyTexture(TextureAsset asset) override;
	TextureAsset GetSkyTexture() override {
		return mSkyTexture;
	}
private:
	void UpdateView();

	ConstantBuffer<CBNeverChanges>		mCBNeverChanges;
	ConstantBuffer<CBChangeOnResize>	mCBChangeOnResize;
	ConstantBuffer<CBBillboard>			mCBBillboard;
	ConstantBuffer<cbNearFar>			mCBNearFar;

	XMMATRIX                            mView;
	XMMATRIX                            mProjection;
	XMVECTOR							Eye;
	XMVECTOR							At;
	XMVECTOR							Up;

	ScreenClearType mScreenClearType;
	//MaterialComponent mSkyMaterial;
	//Material mSkyMaterial;
	//Model mSkyModel;

	TextureAsset mSkyTexture;

	bool m_IsPerspective;
	float m_Fov;
	float m_Near;
	float m_Far;

	friend EditorCamera;
};