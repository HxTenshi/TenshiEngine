#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"
#include <String>
#include "MySTL/File.h"
#include "IComponent.h"
#include "Type/ForceMode.h"
#include "MySTL/ptr.h"

namespace physx{
	class PxRigidActor;
	class PxShape;
}


enum class PhysXChangeTransformFlag{
	Position	=0x0001,
	Rotate		=0x0002,
};

class IPhysXComponent :public Component{
public:
	IPhysXComponent(){}
	virtual ~IPhysXComponent(){}

	virtual bool GetKinematic() = 0;
	virtual void SetKinematic(bool flag) = 0;
	virtual bool GetGravity() = 0;
	virtual void SetGravity(bool flag) = 0;
	virtual XMVECTOR GetForceVelocity() = 0;
	virtual void SetForceVelocity(const XMVECTOR& v) = 0;
	virtual void AddForce(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) = 0;
	virtual void AddTorque(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) = 0;

private:
};

class PhysXComponent :public enable_shared_from_this<PhysXComponent>, public IPhysXComponent{
public:
	PhysXComponent();
	~PhysXComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;

	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif
	void IO_Data(I_ioHelper* io) override;
	void SetChengeTransform(PhysXChangeTransformFlag flag){
		mChengeTransformFlag |= (char)flag;
	}


	physx::PxRigidActor* GetRigidActor(){
		return  mRigidActor;
	}
	void SetTransform(bool RebirthSet);

	bool AddShape(physx::PxShape& shape);
	void RemoveShape(physx::PxShape& shape);

	bool GetKinematic() override{ return mIsKinematic; };
	void SetKinematic(bool flag) override;

	bool GetGravity() override{ return mIsGravity; };
	void SetGravity(bool flag) override;
	XMVECTOR GetForceVelocity() override;
	void SetForceVelocity(const XMVECTOR& v) override;
	void AddForce(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) override;
	void AddTorque(const XMVECTOR& force, ForceMode::Enum forceMode = ForceMode::eFORCE) override;
	

	bool mIsEngineMode;
private:

	void ChildrenAttachShape(Actor* actor);

	physx::PxRigidActor* mRigidActor;
	bool mIsKinematic;
	char mChengeTransformFlag;
	bool mIsGravity;
};

