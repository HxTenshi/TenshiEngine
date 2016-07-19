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
#include "IPhysXEngine.h"

class PhysX3Main : public PhysXEngine
	, public PxUserControllerHitReport, public PxControllerBehaviorCallback, public PxSceneQueryFilterCallback
{
public:

	PhysX3Main();
	~PhysX3Main();

	void InitializePhysX();

	void createPlane();
	PxRigidActor* createSphere();
	PxRigidActor* createBody();
	PxRigidActor* createBodyEngine();
	PxRigidStatic* createRigidStatic();

	void EngineDisplay();
	void Display();

	void ShutdownPhysX();

	void RemoveActor(PxActor* act);
	void RemoveActorEngine(PxActor* act);

	Actor* Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance) override;
	bool RaycastHit(const XMVECTOR& pos, const XMVECTOR& dir, float distance, ::RaycastHit* result) override;
	Actor* EngineSceneRaycast(const XMVECTOR& pos, const XMVECTOR& dir);

	PxShape* CreateShape();
	PxShape* CreateShapeSphere();
	PxShape* CreateTriangleMesh(const IPolygonsData* poly);
	PxController* CreateController();

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
	PxRigidStatic* mRigidStatic;

	PxScene* gScene;
	PxScene* mEngineScene;

	PxMaterial* mMaterial;

	PxReal myTimestep;
};


#pragma pop_macro("free")

#pragma pop_macro("realloc")