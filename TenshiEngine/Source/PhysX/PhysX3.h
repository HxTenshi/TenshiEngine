#pragma once

#pragma push_macro("free")
#undef free
#pragma push_macro("realloc")
#undef realloc

//class PxScene;
//class PxPhysics;
//class PxFoundation;
//class PxShape;
//class PxRigidActor;
//class PxMaterial;
//class PxParticleSystem;
//class PxMat33;
//class PxVec3;

#include <iostream>
#include <PxPhysics.h>
#include <PxPhysicsAPI.h> 
#include <physxprofilesdk\PxProfileZoneManager.h>
#include <extensions\PxDefaultCpuDispatcher.h>
//#include <PxExtensionsAPI.h>
//#include < PxDefaultErrorCallback.h>
//#include < PxDefaultAllocator.h>
//#include < PxDefaultSimulationFilterShader.h>
//#include < PxDefaultCpuDispatcher.h>
//#include < PxShapeExt.h>
#include "PxMat33Legacy.h" 
//#include < PxSimpleFactory.h>
#include <list>

using namespace physx;
class Actor;
class IPolygonsData;
#include <functional>
#include "IPhysXEngine.h"


class PhysX3Main : public PhysXEngine
	, public PxUserControllerHitReport, public PxControllerBehaviorCallback, public PxSceneQueryFilterCallback
{
public:

	PhysX3Main();
	~PhysX3Main();

	void InitializePhysX();

	void createPlane();
	//PxRigidActor* createSphere();
	PxRigidActor* createBody();
	PxRigidActor* createBodyEngine();
	PxRigidStatic* createRigidStatic();

	void EngineDisplay();
	void DisplayInitialize();
	void Display();

	void ShutdownPhysX();

	void AddActor(PxActor* act);
	void AddActorEngine(PxActor* act);
	void RemoveActor(PxActor* act);
	void RemoveActorEngine(PxActor* act);

	GameObject Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance, Layer::Enum layer = Layer::ALL) override;
	GameObject RaycastTop(const XMVECTOR& pos, const XMVECTOR& dir, float distance, Layer::Enum layer = Layer::ALL) override;
	void Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance, const std::function<void(::RaycastHit*)>& collback, Layer::Enum layer = Layer::ALL);
	bool RaycastHit(const XMVECTOR& pos, const XMVECTOR& dir, float distance, ::RaycastHit* result, Layer::Enum layer = Layer::ALL) override;
	GameObject PhysX3Main::EngineSceneRaycast(const XMVECTOR& pos, const XMVECTOR& dir);
	void EngineSceneRaycast(const XMVECTOR& pos, const XMVECTOR& dir, const std::function<void(::RaycastHit*)>& collback);
	int OverlapHitMultiple(weak_ptr<PhysXColliderComponent> collder, const std::function<void(GameObject)>& collback, Layer::Enum layer = Layer::ALL) override;

	PxShape* CreateShape();
	PxShape* CreateShapeSphere();
	PxShape* CreateShapeCapsule();
	PxShape* CreateTriangleMesh(const IPolygonsData* poly);
	PxShape* CreateConvexMesh(const IPolygonsData* poly);
	PxController* CreateController();
	PxRevoluteJoint* CreateRevoluteJoint();
	PxDistanceJoint* CreateDistanceJoint();

	unsigned int GetLayerCollideFlag(Layer::Enum l1);
	bool GetLayerCollideFlag(Layer::Enum l1, Layer::Enum l2);
	void SetLayerCollideFlag(Layer::Enum l1, Layer::Enum l2, bool flag);

	void AddStaticShape(PxShape* shape);
	void RemoveStaticShape(PxShape* shape);

	PxPhysics* GetSDK(){
		return gPhysicsSDK;
	};

	// Implements PxUserControllerHitReport
	virtual void							onShapeHit(const PxControllerShapeHit& hit);
	virtual void							onControllerHit(const PxControllersHit& hit)		{ (void)hit; }
	virtual void							onObstacleHit(const PxControllerObstacleHit& hit)	{ (void)hit; }

	// Implements PxControllerBehaviorCallback
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape& shape, const PxActor& actor);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController& controller);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle& obstacle);

	// Implements PxSceneQueryFilterCallback
	virtual PxQueryHitType::Enum			preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxSceneQueryFlags& queryFlags);
	virtual	PxQueryHitType::Enum			postFilter(const PxFilterData& filterData, const PxSceneQueryHit& hit);

private:
	void getColumnMajor(PxMat33& m, PxVec3& t, float* mat);

	PxPhysics* gPhysicsSDK;
	PxFoundation* mFoundation;
	PxProfileZoneManager* mProfileZoneManager;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader;
	PxCooking*	mCooking;
	PxControllerManager* mControllerManager;
	PxRigidDynamic* mRigidDynamic;
	PxRigidStatic* mRigidStatic;

	PxScene* gScene;
	PxScene* mEngineScene;

	PxMaterial* mMaterial;

	PxReal myTimestep;
};


#pragma pop_macro("free")

#pragma pop_macro("realloc")