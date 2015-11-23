

#include "PhysX3.h"
#include "../../../PhysX SDK/PhysXSDK/Include/PxActor.h"
#include <algorithm>
#include <physxprofilesdk\PxProfileZoneManager.h>
#include <extensions\PxDefaultCpuDispatcher.h>

class TestOn : public physx::PxSimulationEventCallback{
	void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) override{
		int a=0;
	}
	void onWake(PxActor **actors, PxU32 count) override{
		int a=0;
	}
	void onSleep(PxActor **actors, PxU32 count) override{
		int a=0;
	}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
	{
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];
			if (cp.events & PxPairFlag::eTRIGGER_DEFAULT)
			{
				Actor* act0 = (Actor*)pairHeader.actors[0]->userData;
				Actor* act1 = (Actor*)pairHeader.actors[1]->userData;
				if (!(act0&&act1))continue;
				auto script0 = act0->GetComponent<ScriptComponent>();
				auto script1 = act1->GetComponent<ScriptComponent>();
				if (script0)
					script0->OnCollide(act1);
				if (script1)
					script1->OnCollide(act0);

				break;
			}
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				Actor* act0 = (Actor*)pairHeader.actors[0]->userData;
				Actor* act1 = (Actor*)pairHeader.actors[1]->userData;
				if (!(act0&&act1))continue;
				auto script0 = act0->GetComponent<ScriptComponent>();
				auto script1 = act1->GetComponent<ScriptComponent>();
				if (script0)
					script0->OnCollide(act1);
				if (script1)
					script1->OnCollide(act0);

				break;

				//if ((pairHeader.actors[0] == mSubmarineActor) || (pairHeader.actors[1] == mSubmarineActor))
				//{
				//	PxActor* otherActor = (mSubmarineActor == pairHeader.actors[0]) ?
				//		pairHeader.actors[1] : pairHeader.actors[0];
				//	Seamine* mine = reinterpret_cast<Seamine*>(otherActor->userData);
				//	// insert only once
				//	if (std::find(mMinesToExplode.begin(), mMinesToExplode.end(), mine) == mMinesToExplode.end())
				//		mMinesToExplode.push_back(mine);
				//
				//	break;
				//}
			}
		}
	}
	void onTrigger(PxTriggerPair *pairs, PxU32 count) override{
		int a=0;
	}
};

PxFilterFlags SampleSubmarineFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}

struct  FilterGroup
{
	enum  Enum
	{
		eSUBMARINE = (1 << 0),
		eMINE_HEAD = (1 << 1),
		eMINE_LINK = (1 << 2),
		eCRAB = (1 << 3),
		eHEIGHTFIELD = (1 << 4),
	};
};

PhysX3Main::PhysX3Main():
gPhysicsSDK(NULL),
mFoundation(NULL),
gDefaultFilterShader(SampleSubmarineFilterShader),
gScene(NULL),
myTimestep(1.0f/60.0f),
mMaterial(NULL)
{
}

PhysX3Main::~PhysX3Main(){
	ShutdownPhysX();
}

TestOn* mTestOn;
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

	PxCookingParams cookingParam(scale);
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, cookingParam);


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

	sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_PAIRS | PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS | PxSceneFlag::eDISABLE_CONTACT_CACHE | PxSceneFlag::eDISABLE_CONTACT_REPORT_BUFFER_RESIZE | PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	mTestOn = new TestOn();
	sceneDesc.simulationEventCallback = mTestOn;

	gScene = gPhysicsSDK->createScene(sceneDesc);
	if (!gScene)
		std::cerr << "createScene failed!" << std::endl;
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	mMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);

	createPlane();

	//gScene->setSimulationEventCallback(new TestOn());

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
	auto flag = act->getActorFlags();
	flag |= PxActorFlag::eSEND_SLEEP_NOTIFIES;
	act->setActorFlags(flag);
}

PxRigidActor* PhysX3Main::createBox(){
  
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxRigidDynamic *actor = gPhysicsSDK->createRigidDynamic(transform);
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	if (!actor)
		std::cerr << "create actor failed!" << std::endl;
	gScene->addActor(*actor);

	PxRigidActor* act = actor;
	auto flag = act->getActorFlags();
	flag |= PxActorFlag::eSEND_SLEEP_NOTIFIES;
	act->setActorFlags(flag);
	return act;
}

#include <cooking\PxTriangleMeshDesc.h>
#include <cooking\PxCooking.h>
PxShape* PhysX3Main::CreateShape(){
	//PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxVec3 dimensions(0.5, 0.5, 0.5);
	PxBoxGeometry geometry(dimensions);
	auto box = gPhysicsSDK->createShape(geometry, *mMaterial, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE);

	PxFilterData  filterData;
	filterData.word0 = FilterGroup::eSUBMARINE;  //ワード0 =自分のID 
	filterData.word1 = FilterGroup::eSUBMARINE;	//ワード1 = IDマスクcallback; 
	box->setSimulationFilterData(filterData);

	return box;
}

PxShape* PhysX3Main::CreateShapeSphere(){
	//PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxSphereGeometry geometry(0.5);
	auto shape = gPhysicsSDK->createShape(geometry, *mMaterial, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE);
	PxFilterData  filterData;
	filterData.word0 = FilterGroup::eSUBMARINE;  //ワード0 =自分のID 
	filterData.word1 = FilterGroup::eSUBMARINE;	//ワード1 = IDマスクcallback; 
	shape->setSimulationFilterData(filterData);
	return shape;

}

PxShape* PhysX3Main::CreateTriangleMesh(const std::vector<PolygonsData::VertexType>& vertex, const std::vector<PolygonsData::IndexType>&  index){

	//std::vector<PxVec3> vertexVec3(VertexNum);
	std::vector<PxVec3> vertexVec3;
	vertexVec3.resize(vertex.size());
	for (int i = 0; i < vertex.size(); i++){
		vertexVec3[i] = PxVec3(vertex[i].Pos.x, vertex[i].Pos.y, vertex[i].Pos.z);
	}

	//PxBoundedData vertexdata;
	//vertexdata.count = vertex.size();
	//vertexdata.stride = sizeof(PxVec3);
	//vertexdata.data = vertexVec3.data();


	//std::vector<PxU32> indexU32;
	std::vector<PxU32> indexU32(index.size());
	indexU32.resize(index.size());
	for (int i = 0; i < index.size(); i++){
		indexU32[i] = (PxU32)index[i];
	}

	//PxBoundedData indexdata;
	//indexdata.count = index.size() / 3;
	//indexdata.stride = sizeof(PolygonsData::IndexType) * 3;
	//indexdata.data = indexU32.data();


	// メッシュデータのパラメータを設定
	PxTriangleMeshDesc triangleDesc;
	// 頂点データ
	triangleDesc.points.count = vertexVec3.size();
	triangleDesc.points.stride = sizeof(PxVec3);
	triangleDesc.points.data = vertexVec3.data();
	// 三角形の頂点インデックス
	triangleDesc.triangles.count = indexU32.size() / 3;
	triangleDesc.triangles.stride = sizeof(PxU32) * 3;
	triangleDesc.triangles.data = indexU32.data();

	//triangleDesc.flags = PxMeshFlag;

	PxDefaultMemoryOutputStream writeBuffer;
	bool status = mCooking->cookTriangleMesh(triangleDesc, writeBuffer);
	if (!status){
		return NULL;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	// メッシュを作成する
	auto mesh =  gPhysicsSDK->createTriangleMesh(readBuffer);

	PxTriangleMeshGeometry meshGeo(mesh);
	auto shape = gPhysicsSDK->createShape(meshGeo, *mMaterial, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE);

	PxFilterData  filterData;
	filterData.word0 = FilterGroup::eSUBMARINE;  //ワード0 =自分のID 
	filterData.word1 = FilterGroup::eSUBMARINE;	//ワード1 = IDマスクcallback; 
	shape->setSimulationFilterData(filterData);

	return shape;
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
	gScene->checkResults();
	
	while (!gScene->fetchResults())
	{
		//break;
		// do something useful        
	}
	//while ()
	{
		//break;    
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
	delete mTestOn;
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