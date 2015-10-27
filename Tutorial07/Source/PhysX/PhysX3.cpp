

#include "PhysX3.h"
#include "../../../PhysX SDK/PhysXSDK/Include/PxActor.h"
#include <algorithm>
#include <physxprofilesdk\PxProfileZoneManager.h>
#include <extensions\PxDefaultCpuDispatcher.h>

PhysX3Main::PhysX3Main():
gPhysicsSDK(NULL),
mFoundation(NULL),
gDefaultFilterShader(PxDefaultSimulationFilterShader),
gScene(NULL),
myTimestep(1.0f/60.0f),
mMaterial(NULL)
{
}

PhysX3Main::~PhysX3Main(){
	ShutdownPhysX();
}

PxDefaultCpuDispatcher* mCpuDispatcher;
PxVec3 gravity(0, -9.81f, 0);
void PhysX3Main::InitializePhysX() {

	//mhbox = MV1LoadModel("res/PhysX/box.mqo");

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	if (!mFoundation){
		std::cerr << "PxCreateFoundation failed!" << std::endl;
		exit(1);
	}

	bool recordMemoryAllocations = true;
	mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
	if (!mProfileZoneManager){
		std::cerr << "PxProfileZoneManager::createProfileZoneManager failed!" << std::endl;
		exit(1);
	}

	PxTolerancesScale scale;
	scale.mass = 1000;
	scale.length = 1;        // typical length of an object
	//物理を停止するスピード
	scale.speed = 0.1f;         // typical speed of an object, gravity*1s is a reasonable choice

	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		scale, recordMemoryAllocations, mProfileZoneManager);
	if (gPhysicsSDK == NULL) {
		std::cerr << "Error creating PhysX device." << std::endl;
		std::cerr << "Exiting..." << std::endl;
		exit(1);
	}
	if (!PxInitExtensions(*gPhysicsSDK))
		std::cerr << "PxInitExtensions failed!" << std::endl;


	//Create the scene
	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = gravity;
	if (!sceneDesc.cpuDispatcher) {
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!mCpuDispatcher)
			std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = gDefaultFilterShader;

	gScene = gPhysicsSDK->createScene(sceneDesc);
	if (!gScene)
		std::cerr << "createScene failed!" << std::endl;
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	mMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);

	createPlane();

	//gScene->simulate(myTimestep);
}
PxRigidStatic* p;
void PhysX3Main::createPlane(){
	//Create actors 
	//1) Create ground plane
	PxReal d = 0.0f;
	PxTransform pose = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysicsSDK->createRigidStatic(pose);
	if (!plane)
		std::cerr << "create plane failed!" << std::endl;
	PxShape* shape = plane->createShape(PxPlaneGeometry(), *mMaterial);
	if (!shape)
		std::cerr << "create shape failed!" << std::endl;
	gScene->addActor(*plane);

	p = plane;
	PxRigidActor* act = plane;
}

PxRigidActor* PhysX3Main::createSphere(){

	PxReal density = 1.0f;
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxReal r = 0.5f;
	PxSphereGeometry geometry(r);

	PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	if (!actor)
		std::cerr << "create actor failed!" << std::endl;
	gScene->addActor(*actor);

	PxRigidActor* act = actor;
	return act;
}

PxRigidActor* PhysX3Main::createBox(){

	//2) Create cube  
	PxReal density = 1.0f;
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	//PxVec3 dimensions(0.5, 0.5, 0.5);
	//PxBoxGeometry geometry(dimensions);
	PxRigidDynamic *actor = gPhysicsSDK->createRigidDynamic(transform);
	//PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	if (!actor)
		std::cerr << "create actor failed!" << std::endl;
	gScene->addActor(*actor);

	PxRigidActor* act = actor;
	return act;
}


PxShape* PhysX3Main::CreateShape(){
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxVec3 dimensions(0.5, 0.5, 0.5);
	PxBoxGeometry geometry(dimensions);
	return gPhysicsSDK->createShape(geometry, *mMaterial,PxShapeFlag::eSIMULATION_SHAPE);

}

PxShape* PhysX3Main::CreateShapeSphere(){
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxSphereGeometry geometry(0.5);
	return gPhysicsSDK->createShape(geometry, *mMaterial, PxShapeFlag::eSIMULATION_SHAPE);

}

static int mCount=0;
static int mNum = 0;
static int mMax = 1;

void PhysX3Main::StepPhysX()
{
	//mCount++;
	//if (mCount % 6 == 0 && mNum != mMax){
	//	mNum++;
	//	createBox();
	//}

	//シミュレーションするものがないと出力がでる
	gScene->simulate(myTimestep);

	//シュミレーション中に？作成Shapeを？作成できない
	//...perform useful work here using previous frame's state data        
	while (!gScene->fetchResults())
	{
		//break;
		// do something useful        
	}
}

void PhysX3Main::getColumnMajor(PxMat33& m, PxVec3& t, float* mat) {
	mat[0] = m.column0[0];
	mat[1] = m.column0[1];
	mat[2] = m.column0[2];
	mat[3] = 0;

	mat[4] = m.column1[0];
	mat[5] = m.column1[1];
	mat[6] = m.column1[2];
	mat[7] = 0;

	mat[8] = m.column2[0];
	mat[9] = m.column2[1];
	mat[10] = m.column2[2];
	mat[11] = 0;

	mat[12] = t[0];
	mat[13] = t[1];
	mat[14] = t[2];
	mat[15] = 1;
}

void PhysX3Main::DrawBox(PxShape* pShape, PxRigidActor* actor){
	PxTransform pT = PxShapeExt::getGlobalPose(*pShape, *actor);

	//PxBoxGeometry bg;
	//pShape->getBoxGeometry(bg);

	//PxMat33 m = PxMat33Legacy(pT.q);
	//
	//float matf[16];
	//getColumnMajor(m, pT.p, matf);

	//MATRIX mat = MGet(matf);
	//VECTOR scale = VGet(bg.halfExtents.x * 2.0f, bg.halfExtents.y * 2.0f, bg.halfExtents.z * 2.0f);
	//mat = MMult(MGetScale(scale), mat);
	//MV1SetMatrix(mhbox, mat);
	//MV1DrawModel(mhbox);

}

void PhysX3Main::DrawShape(PxShape* shape, PxRigidActor* actor)
{
	PxGeometryType::Enum type = shape->getGeometryType();
	switch (type)
	{
	case PxGeometryType::eBOX:
		DrawBox(shape, actor);
		break;
	}
}

void PhysX3Main::DrawActor(PxRigidActor* actor)
{
	static PxShape* shapes[256];
	PxU32 nShapes = actor->getNbShapes();
	//PxShape** shapes = new PxShape*[nShapes];

	actor->getShapes(shapes, nShapes);
	while (nShapes--)
	{
		DrawShape(shapes[nShapes], actor);
	}
	//delete[] shapes;
}

void PhysX3Main::RenderActors()
{

	//return;
	// Render all the actors in the scene 

	//std::for_each(boxs.begin(), boxs.end(), [&](PxRigidActor* box)
	//{
	//	DrawActor(box);
	//}
	//);
}


void PhysX3Main::Display() {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	//setup the view transformation using 
	//gluLookAt(...);

	//Update PhysX 
	if (gScene)
	{
		StepPhysX();
	}

	//RenderActors();

	//glutSwapBuffers();
}

void PhysX3Main::ShutdownPhysX() {
	p->release();
	mMaterial->release();
	mCpuDispatcher->release();
	gScene->release();
	mFoundation->release();
	gPhysicsSDK->release();//メモリーリーク
	mProfileZoneManager->release();
}


void PhysX3Main::RemoveActor(PxActor* act){
	gScene->removeActor(*act);
	act->release();
}