#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <String>
#include "MySTL/File.h"
#include "MySTL/Ptr.h"
#include "IComponent.h"

namespace physx{
	class PxShape;
}
class PhysXComponent;
class PhysXColliderComponent :public Component{
public:
	PhysXColliderComponent();
	~PhysXColliderComponent();
	void Initialize() override;

	void Update() override;
	void CreateInspector() override;

	void ExportData(File& f) override;
	void ImportData(File& f) override;

	physx::PxShape* GetShape();
private:
	void UpdatePose();
	physx::PxShape* mShape;
	weak_ptr<PhysXComponent> mAttachPhysXComponent;

	bool mIsParentPhysX;
};