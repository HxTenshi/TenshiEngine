#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <String>
#include <list>
#include "MySTL/File.h"
#include "PhysXComponent.h"
#include "IComponent.h"

class Actor;
class TransformComponent;

class ITransformComponent :public Component{
public:
	virtual ~ITransformComponent(){

	}
	//virtual const XMVECTOR& WorldScale() const = 0;
	//virtual const XMVECTOR& WorldRotate() const = 0;
	virtual const XMVECTOR& WorldPosition() const = 0;


	virtual const XMVECTOR& Scale() const = 0;
	virtual const XMVECTOR& Rotate() const = 0;
	virtual const XMVECTOR& Position() const = 0;

	virtual void Scale(const XMVECTOR& scale) = 0;
	virtual void Rotate(const XMVECTOR& rotate) = 0;
	virtual void Position(const XMVECTOR& position) = 0;

	virtual void AddForce(XMVECTOR& force) = 0;
	virtual void AddTorque(XMVECTOR& force) = 0;
	virtual const XMMATRIX& GetMatrix() const = 0;

	virtual std::list<Actor*>& Children() = 0;
	virtual Actor* GetParent() = 0;
	virtual void SetParent(Actor* parent) = 0;

	virtual void SetUndo(const XMVECTOR& pos) = 0;

private:
	friend TransformComponent;
	virtual void FlagSetChangeMatrix() = 0;

};

class TransformComponent :public ITransformComponent{
public:
	TransformComponent();
	~TransformComponent();

	//const XMVECTOR& WorldScale() const override;
	//const XMVECTOR& WorldRotate() const override;
	const XMVECTOR& WorldPosition() const override;

	const XMVECTOR& Scale() const override;
	const XMVECTOR& Rotate() const override;
	const XMVECTOR& Position() const override;
	void Scale(const XMVECTOR& scale) override;
	void Rotate(const XMVECTOR& rotate) override;
	void Position(const XMVECTOR& position) override;

	void Update() override;

	void AddForce(XMVECTOR& force) override;
	void AddTorque(XMVECTOR& force) override;

	const XMMATRIX& GetMatrix() const override;
	void CreateInspector() override;

	void ExportData(File& f) override;
	void ImportData(File& f) override;

	void UpdatePhysX(PhysXChangeTransformFlag flag);

	void FlagSetChangeMatrix();
	bool IsChangeMatrix(){
		return !mFixMatrixFlag;
	}

	void SetUndo(const XMVECTOR& pos) override;

	std::list<Actor*>& Children() override;
	Actor* GetParent() override;
	void SetParent(Actor* parent) override;

private:
	XMVECTOR mScale;
	XMVECTOR mRotate;
	XMVECTOR mPosition;
	mutable XMMATRIX mMatrix;
	std::list<Actor*> mChildren;
	Actor* mParent;
protected:
	mutable bool mFixMatrixFlag;
};