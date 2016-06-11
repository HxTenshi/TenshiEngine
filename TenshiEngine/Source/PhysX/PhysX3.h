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

class PhysX3Main : public PhysXEngine{
public:

	PhysX3Main();
	~PhysX3Main();

	void InitializePhysX();

	void createPlane();
	PxRigidActor* createSphere();
	PxRigidActor* createBody();
	PxRigidActor* createBodyEngine();

	void EngineDisplay();
	void Display();
	void StepPhysX();

	void ShutdownPhysX();

	void RemoveActor(PxActor* act);
	void RemoveActorEngine(PxActor* act);

	Actor* Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance) override;
	Actor* EngineSceneRaycast(const XMVECTOR& pos, const XMVECTOR& dir);

	PxShape* CreateShape();
	PxShape* CreateShapeSphere();
	PxShape* CreateTriangleMesh(const IPolygonsData* poly);
	PxPhysics* GetSDK(){
		return gPhysicsSDK;
	};

private:
	void getColumnMajor(PxMat33& m, PxVec3& t, float* mat);

	PxPhysics* gPhysicsSDK;
	PxFoundation* mFoundation;
	PxProfileZoneManager* mProfileZoneManager;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader;
	PxCooking*	mCooking;

	PxScene* gScene;
	PxScene* mEngineScene;

	PxMaterial* mMaterial;

	PxReal myTimestep;
};


#pragma pop_macro("free")

#pragma pop_macro("realloc")