#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"
#include <String>
#include <list>
#include "PhysXComponent.h"
#include "IComponent.h"
#include "Type/ForceMode.h"
#include "Game/Types.h"

class Actor;
class Game;
class TransformComponent;


class ITransformComponent :public Component{
public:
	virtual ~ITransformComponent(){

	}
	virtual const XMVECTOR WorldScale() const = 0;
	virtual const XMVECTOR LossyScale() const = 0;
	//virtual const XMVECTOR& WorldRotate() const = 0;
	virtual const XMVECTOR& WorldPosition() const = 0;
	virtual const XMVECTOR WorldQuaternion() const = 0;
	virtual void WorldPosition(const XMVECTOR& position) = 0;
	virtual void WorldQuaternion(const XMVECTOR& quaternion) = 0;
	virtual void WorldScale(const XMVECTOR& scale) = 0;
	virtual void WorldMatrix(const XMMATRIX& matrix) = 0;


	virtual const XMVECTOR& Scale() const = 0;
	virtual const XMVECTOR& Rotate() const = 0;
	virtual const XMVECTOR& Position() const = 0;
	virtual const XMVECTOR DegreeRotate() const = 0;
	virtual const XMVECTOR Quaternion() const = 0;

	virtual void Scale(const XMVECTOR& scale) = 0;
	virtual void Rotate(const XMVECTOR& rotate) = 0;
	virtual void DegreeRotate(const XMVECTOR& rotate) = 0;
	virtual void Position(const XMVECTOR& position) = 0;
	virtual void Quaternion(const XMVECTOR& Quaternion) = 0;

	virtual const XMVECTOR Forward() const = 0;
	virtual const XMVECTOR Left() const = 0;
	virtual const XMVECTOR Up() const = 0;

	virtual void AddForce(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) = 0;
	virtual void AddTorque(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) = 0;
	virtual const XMMATRIX& GetMatrix() const = 0;

	virtual std::list<GameObject> Children() = 0;
	virtual GameObject GetParent() = 0;
	virtual void SetParentUniqueID(UniqueID id) = 0;
	virtual void SetParent(GameObject parent) = 0;
	virtual void SetParentWorld(GameObject parent) = 0;

	virtual void AllChildrenDestroy() = 0;
protected:
	ITransformComponent(){};
private:

	virtual std::list<GameObject>* ChildrenRef() = 0;
	friend TransformComponent;
	virtual void FlagSetChangeMatrix(PhysXChangeTransformFlag flag) = 0;

};

class TransformComponent :public ITransformComponent{
public:
	TransformComponent();
	~TransformComponent();

	const XMVECTOR WorldScale() const override;
	const XMVECTOR LossyScale() const override;
	//const XMVECTOR& WorldRotate() const override;
	const XMVECTOR& WorldPosition() const override;
	const XMVECTOR WorldQuaternion() const override;
	void WorldPosition(const XMVECTOR& position) override;
	void WorldQuaternion(const XMVECTOR& quaternion) override;
	void WorldScale(const XMVECTOR& scale) override;
	void WorldMatrix(const XMMATRIX & matrix) override;

	const XMVECTOR& Scale() const override;
	const XMVECTOR& Rotate() const override;
	const XMVECTOR& Position() const override;
	const XMVECTOR DegreeRotate() const override;
	const XMVECTOR Quaternion() const override;
	void Scale(const XMVECTOR& scale) override;
	void Rotate(const XMVECTOR& rotate) override;
	void Position(const XMVECTOR& position) override;
	void DegreeRotate(const XMVECTOR& rotate) override;
	void Quaternion(const XMVECTOR& Quaternion) override;

	const XMVECTOR Forward() const override;
	const XMVECTOR Left() const override;
	const XMVECTOR Up() const override;

	void Initialize() override;
	void Start() override;
	void Finish() override;
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;

	void AddForce(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) override;
	void AddTorque(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) override;

	const XMMATRIX& GetMatrix() const override;
#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void UpdatePhysX(PhysXChangeTransformFlag flag);

	//このゲームオブジェクトより子のオブジェクトを全てデストロイする
	void AllChildrenDestroy() override;
	void FlagSetChangeMatrix(PhysXChangeTransformFlag flag);
	bool IsChangeMatrix(){
		return !mFixMatrixFlag;
	}

	std::list<GameObject> Children() override;
	GameObject GetParent() override;
	void SetParentUniqueID(UniqueID id) override;
	void SetParent(GameObject parent) override;
	void SetParentWorld(GameObject parent) override;

private:

	std::list<GameObject>* ChildrenRef() override {
		return &mChildren;
	}
	XMVECTOR mScale;
	XMVECTOR mRotate;
	XMVECTOR mInspectorRotateDegree;
	XMVECTOR mPosition;
	mutable XMMATRIX mMatrix;
	std::list<GameObject> mChildren;
	GameObject mParent;
	UniqueID mParentUniqueHashID;

	friend Actor;
	friend Game;
	bool mIsEndInitialize;
protected:
	mutable bool mFixMatrixFlag;
};