#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"
#include <String>
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
	void Start() override;
	void Finish() override;
	void EngineUpdate() override;
	void Update() override;
	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;

	void ChangeShape();
	void ChangeMaterial();
	void CreateMesh();
	void SetIsTrigger(bool flag);

	void SetMesh(const char* file){
		mMeshFile = file;
	}

	physx::PxShape* GetShape();
private:
	bool SearchAttachPhysXComponent();
	void ShapeAttach(physx::PxShape* shape);
	void UpdatePose();
	physx::PxShape* mShape;
	bool mIsShapeAttach;
	weak_ptr<PhysXComponent> mAttachPhysXComponent;

	bool mIsParentPhysX;

	bool mIsSphere;
	std::string mMeshFile;
	std::string mPhysicsMaterialFile;

	//åªç›ÇÃÉXÉPÅ[Éã
	XMVECTOR mScale;

	bool mIsTrigger;
};