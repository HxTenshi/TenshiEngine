#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <String>
#include "MySTL/File.h"
#include "PhysXComponent.h"
#include "IComponent.h"

class ITransformComponent :public Component{
public:
	virtual ~ITransformComponent(){

	}

	virtual const XMVECTOR& Scale() const = 0;
	virtual const XMVECTOR& Rotate() const = 0;
	virtual const XMVECTOR& Position() const = 0;

	virtual void Scale(const XMVECTOR& scale) = 0;
	virtual void Rotate(const XMVECTOR& rotate) = 0;
	virtual void Position(const XMVECTOR& position) = 0;

	virtual void AddForce(XMVECTOR& force) = 0;
	virtual const XMMATRIX& GetMatrix() const = 0;
};

class TransformComponent :public ITransformComponent{
public:
	TransformComponent();

	const XMVECTOR& Scale() const override;
	const XMVECTOR& Rotate() const override;
	const XMVECTOR& Position() const override{
		return mPosition;
	}

	void Scale(const XMVECTOR& scale) override;
	void Rotate(const XMVECTOR& rotate) override;
	void Position(const XMVECTOR& position) override{
		mPosition = position;
		mFixMatrixFlag = false;
		UpdatePhysX();
	}

	void AddForce(XMVECTOR& force) override;

	const XMMATRIX& GetMatrix() const override;
	void CreateInspector() override;

	void ExportData(File& f) override;
	void ImportData(File& f) override;

	void UpdatePhysX();

	bool isChangeMatrix(){
		return !mFixMatrixFlag;
	}

private:
	XMVECTOR mScale;
	XMVECTOR mRotate;
	XMVECTOR mPosition;
	mutable bool mFixMatrixFlag;
	mutable XMMATRIX mMatrix;
};