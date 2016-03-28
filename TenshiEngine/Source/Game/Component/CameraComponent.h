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

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;

	void VSSetConstantBuffers() const;
	void PSSetConstantBuffers() const;
	void GSSetConstantBuffers() const;

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