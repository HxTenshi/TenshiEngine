#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"

namespace physx{
	class PxRigidActor;
	class PxRevoluteJoint;
	class PxDistanceJoint;
}
enum class RigitActorID{
	Actor_0 = 0,
	Actor_1 = 1,
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

	void SetRigidActor(RigitActorID id, Actor* actor);

private:

	//physx::PxRevoluteJoint* mJoint;
	physx::PxDistanceJoint* mJoint;
	physx::PxRigidActor* mRigidActor[2];
	int mActorUID[2];
};

