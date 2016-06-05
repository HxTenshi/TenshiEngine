#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"
#include <String>
#include "MySTL/Ptr.h"
#include "IComponent.h"
#include "Graphic/Model/Model.h"

namespace physx{
	class PxShape;
}
struct ID3D11DeviceContext;
class PhysXComponent;
class Material;

class IPhysXColliderComponent{
public:
	virtual ~IPhysXColliderComponent(){}
	virtual void SetTransform(const XMVECTOR& pos) = 0;
	virtual const XMVECTOR& GetTransform() const = 0;
	virtual void SetScale(const XMVECTOR& scale) = 0;
	virtual const XMVECTOR& GetScale() const = 0;

	virtual void ChangeMaterial(const PhysxMaterialAssetDataPtr& material) = 0;
	virtual void ChangeMaterial(const std::string& file) = 0;
	virtual PhysxMaterialAssetDataPtr GetMaterial() = 0;

	virtual void CreateMesh(const MeshAssetDataPtr& mesh) = 0;
	virtual void CreateMesh(const std::string& file) = 0;

	virtual void SetIsTrigger(bool flag) = 0;
	virtual bool GetIsTrigger() = 0;
};

class PhysXColliderComponent :public IPhysXColliderComponent, public Component{
public:
	PhysXColliderComponent();
	~PhysXColliderComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;
	void EngineUpdate() override;
	void Update() override;
#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void DrawMesh(ID3D11DeviceContext* context, const Material& material);


	void SetTransform(const XMVECTOR& pos) override;
	const XMVECTOR& GetTransform() const override;

	void SetScale(const XMVECTOR& scale) override;
	const XMVECTOR& GetScale() const override;

	void ChangeMaterial(const PhysxMaterialAssetDataPtr& material) override;
	void ChangeMaterial(const std::string& file) override;
	PhysxMaterialAssetDataPtr GetMaterial() override;

	// false=box, true=sphere 
	void ChangeShape(bool flag);

	void CreateMesh(const MeshAssetDataPtr& mesh) override;
	void CreateMesh(const std::string& file) override;

	void SetIsTrigger(bool flag) override;
	bool GetIsTrigger() override;


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
	PhysxMaterialAssetDataPtr mPhysicsMaterial;

	XMVECTOR mPosition;
	XMVECTOR mScale;

	bool mIsTrigger;

	std::string mDebugStr;
	Model mDebugDraw;
};