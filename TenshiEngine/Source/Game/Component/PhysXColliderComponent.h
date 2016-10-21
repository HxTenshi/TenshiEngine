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

struct GeometryType {
	enum Enum {
		Box,
		Sphere,
		Mesh,
	};
};

class IPhysXColliderComponent{
public:
	virtual ~IPhysXColliderComponent(){}
	virtual void SetTransform(const XMVECTOR& pos) = 0;
	virtual const XMVECTOR& GetTransform() const = 0;
	virtual void SetScale(const XMVECTOR& scale) = 0;
	virtual const XMVECTOR& GetScale() const = 0;

	virtual void ChangeMaterial(const PhysxMaterialAsset& material) = 0;
	virtual PhysxMaterialAsset GetMaterial() = 0;

	virtual void ChangeShapeBox() = 0;
	virtual void ChangeShapeSphere() = 0;
	virtual void ChangeShapeMesh(const MeshAsset& mesh) = 0;

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
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;
	void ChangeParentCallback() override;
#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

	void DrawMesh(ID3D11DeviceContext* context, const Material& material);

	void SetTransform(const XMVECTOR& pos) override;
	const XMVECTOR& GetTransform() const override;

	void SetScale(const XMVECTOR& scale) override;
	const XMVECTOR& GetScale() const override;

	void ChangeMaterial(const PhysxMaterialAsset& material) override;
	PhysxMaterialAsset GetMaterial() override;
 
	void ChangeShapeBox();
	void ChangeShapeSphere();
	void ChangeShapeMesh(const MeshAsset& mesh) override;

	void SetIsTrigger(bool flag) override;
	bool GetIsTrigger() override;

	void AttachPhysxComponent(weak_ptr<PhysXComponent> com);

	physx::PxShape* GetShape();

private:
	void SetPhysxLayer(int layer);
	bool SearchAttachPhysXComponent();
	void ShapeAttach(physx::PxShape* shape);

	//シェイプをリジッドスタティックにアタッチするか削除する
	void AttachRigidStatic(bool attach);
	//シェイプをリジッドダイナミックにアタッチするか削除する
	void AttachRigidDynamic(bool attach);
	//シェイプをリリース
	void ReleaseShape();
	//アタッチの解放
	void ReleaseAttach();

	void OnEnabled()override;
	void OnDisabled()override;

	void ShapeReSettings();

	physx::PxShape* mShape;
	XMMATRIX mShapeMatrix;
	//-1 = static, 0 = none, 1~ = dynamic
	int mAttachTarget;
	weak_ptr<PhysXComponent> mAttachPhysXComponent;

	bool mIsParentPhysX;

	int mGeometryType;
	MeshAsset mMeshAsset;
	PhysxMaterialAsset mPhysicsMaterialAsset;

	XMVECTOR mPosition;
	XMVECTOR mScale;

	XMVECTOR mGameObjectPosition;
	XMVECTOR mGameObjectRotate;
	XMVECTOR mGameObjectScale;

	bool mIsTrigger;
#ifdef _ENGINE_MODE
	std::string mDebugStr;
	Model mDebugDraw;
#endif

	friend Actor;
};