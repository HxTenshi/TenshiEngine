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

#ifdef _DEBUG
//#pragma comment(lib,"PhysX3Gpu_x86.lib")
//#pragma comment(lib,"PhysX3GpuCHECKED_x86.lib")
#pragma comment(lib,"PhysX3DEBUG_x86.lib")
//#pragma comment(lib,"PhysX3GpuDEBUG_x86.lib")
//#pragma comment(lib,"PhysX3GpuPROFILE_x86.lib")
//#pragma comment(lib,"PhysX3DEBUG_x86.lib")
//#pragma comment(lib,"PhysX3CharacterKinematicDEBUG_x86.lib")
#pragma comment(lib,"PhysX3CommonDEBUG_x86.lib")
//#pragma comment(lib,"PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib,"PhysXProfileSDKDEBUG.lib")
//#pragma comment(lib,"PxTaskDEBUG.lib")
#pragma comment(lib,"PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib,"PhysX3CookingDEBUG_x86.lib")

#else
#pragma comment(lib,"PhysX3_x86.lib")
#pragma comment(lib,"PhysX3Common_x86.lib")
#pragma comment(lib,"PhysXProfileSDK.lib")
#pragma comment(lib,"PhysX3Extensions.lib")
#pragma comment(lib,"PhysX3Cooking_x86.lib")
#endif

using namespace physx;
class Actor;
class IPolygonsData;

class PhysX3Main{
public:

	PhysX3Main();
	~PhysX3Main();

	void InitializePhysX();

	void createPlane();
	PxRigidActor* createSphere();
	PxRigidActor* createBody();
	PxRigidActor* createBodyEngine();

	void DrawBox(PxShape* pShape, PxRigidActor* actor);
	void DrawShape(PxShape* shape, PxRigidActor* actor);

	void DrawActor(PxRigidActor* actor);

	void EngineDisplay();
	void Display();
	void StepPhysX();

	void ShutdownPhysX();

	void RemoveActor(PxActor* act);
	void RemoveActorEngine(PxActor* act);

	Actor* Raycast(const XMVECTOR& pos, const XMVECTOR& dir);
	Actor* EngineSceneRaycast(const XMVECTOR& pos, const XMVECTOR& dir);

	PxShape* CreateShape();
	PxShape* CreateShapeSphere();
	PxShape* CreateTriangleMesh(const IPolygonsData* poly);

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