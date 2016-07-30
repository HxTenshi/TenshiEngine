#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"

namespace physx{
	class PxRigidActor;
	class PxRevoluteJoint;
}
enum class RigitActorID{
	Actor_1,
	Actor_2,
};

class IJointComponent :public Component{
public:
	IJointComponent(){}
	virtual ~IJointComponent(){}

private:
};

class JointComponent :public IJointComponent{
public:
	JointComponent();
	~JointComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;

	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif
	void IO_Data(I_ioHelper* io) override;

	void SetRigidActor(RigitActorID id, physx::PxRigidActor* actor);

private:

	physx::PxRevoluteJoint* mJoint;
	physx::PxRigidActor* mRigidActor[2];
};

