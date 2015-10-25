#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <String>
#include "MySTL/File.h"
#include "IComponent.h"

namespace physx{
	class PxRigidActor;
}
class PhysXComponent :public Component{
public:
	PhysXComponent();
	~PhysXComponent();
	void Initialize() override;

	void SetPosition(XMVECTOR& position);

	void Update() override;

	void CreateInspector() override;

	void ExportData(File& f) override;
	void ImportData(File& f) override;

	void SetChengeTransformFlag(){
		mChengeTransform = true;
	}

	void SetKinematic(bool flag);

	void AddForce(XMVECTOR& force);

private:
	physx::PxRigidActor* mRigidActor;
	bool mChengeTransform;
	bool mIsKinematic;
};