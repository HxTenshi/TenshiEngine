#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"
#include <String>
#include "MySTL/File.h"
#include "IComponent.h"

namespace physx{
	class PxRigidActor;
	class PxShape;
}

enum class PhysXChangeTransformFlag{
	Position	=0x0001,
	Rotate		=0x0002,
};

class PhysXComponent :public Component{
public:
	PhysXComponent();
	~PhysXComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;

	void Update() override;

	void CreateInspector() override;
	void IO_Data(I_ioHelper* io) override;
	void SetChengeTransform(PhysXChangeTransformFlag flag){
		mChengeTransformFlag |= (char)flag;
	}

	void SetTransform();

	void AddShape(physx::PxShape& shape);
	void RemoveShape(physx::PxShape& shape);
	void SetKinematic(bool flag);

	void AddForce(const XMVECTOR& force);
	void AddTorque(const XMVECTOR& force);

	bool mIsEngineMode;
private:
	physx::PxRigidActor* mRigidActor;
	bool mIsKinematic;
	char mChengeTransformFlag;
};

