#pragma once

#include "IComponent.h"
#include "XNAMath/xnamath.h"

#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"
#include "Game/Component/MaterialComponent.h"
#include "Graphic/Material/Material.h"
#include "Graphic/Model/Model.h"

class EditorCamera;

class CameraComponent :public Component{
public:
	CameraComponent();
	~CameraComponent();

	void Initialize() override;

	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void VSSetConstantBuffers(ID3D11DeviceContext* context) const;
	void PSSetConstantBuffers(ID3D11DeviceContext* context) const;
	void GSSetConstantBuffers(ID3D11DeviceContext* context) const;

	float GetNear();
	float GetFar();

	const XMMATRIX& GetViewMatrix();

	enum class ScreenClearType{
		Color,
		Sky,
		None,
		Count
	};

	void ScreenClear();

	void SetPerspective();
	void SetOrthographic();
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
	Material mSkyMaterial;
	Model mSkyModel;

	friend EditorCamera;
};