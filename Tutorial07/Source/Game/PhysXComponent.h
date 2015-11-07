#pragma once

#include <d3d11.h>
#include <xnamath.h>
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

	void Update() override;

	void CopyData(Component* post, Component* base) override;
	void CreateInspector() override;
	void ExportData(File& f) override;
	void ImportData(File& f) override;

	void SetChengeTransform(PhysXChangeTransformFlag flag){
		mChengeTransformFlag |= (char)flag;
	}

	void SetTransform();

	void AddShape(physx::PxShape& shape);
	void RemoveShape(physx::PxShape& shape);
	void SetKinematic(bool flag);

	void AddForce(XMVECTOR& force);
	void AddTorque(XMVECTOR& force);

private:
	physx::PxRigidActor* mRigidActor;
	bool mIsKinematic;
	char mChengeTransformFlag;
};