

#include "PhysX3.h"
#include <algorithm>

#include "Game/Actor.h"
#include "Game/Component/ScriptComponent.h"

#include "Engine/AssetLoader.h"

#include "Game/Game.h"


#ifdef _DEBUG
//#pragma comment(lib,"PhysX3Gpu_x86.lib")
//#pragma comment(lib,"PhysX3GpuDEBUG_x86.lib")
//#pragma comment(lib,"PxTaskDEBUG.lib")
#pragma comment(lib,"PhysX_MDd.lib")
#pragma comment(lib,"PhysXCommon_MDd.lib")
#pragma comment(lib,"PhysXProfileSDK_MDd.lib")
#pragma comment(lib,"PhysXExtensions_MDd.lib")
#pragma comment(lib,"PhysXCooking_MDd.lib")
#pragma comment(lib,"PhysXCharacterKinematic_MDd.lib")

#else

#ifdef _ENGINE_MODE
#pragma comment(lib,"PhysX_MD.lib")
#pragma comment(lib,"PhysXCommon_MD.lib")
#pragma comment(lib,"PhysXProfileSDK_MD.lib")
#pragma comment(lib,"PhysXExtensions_MD.lib")
#pragma comment(lib,"PhysXCooking_MD.lib")
#pragma comment(lib,"PhysXCharacterKinematic_MD.lib")
#else
#pragma comment(lib,"PhysX_MT.lib")
#pragma comment(lib,"PhysXCommon_MT.lib")
#pragma comment(lib,"PhysXProfileSDK_MT.lib")
#pragma comment(lib,"PhysXExtensions_MT.lib")
#pragma comment(lib,"PhysXCooking_MT.lib")
#pragma comment(lib,"PhysXCharacterKinematic_MT.lib")
#endif

#endif

//bool g_SimulationInitialize = false;
class TestOn : public physx::PxSimulationEventCallback{
	void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) override{
		(void)constraints;
		(void)count;
	}
	void onWake(PxActor **actors, PxU32 count) override{
		(void)actors;
		(void)count;
	}
	void onSleep(PxActor **actors, PxU32 count) override{
		(void)actors;
		(void)count;
	}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
	{
		//if (g_SimulationInitialize)return;
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			//Actor* act0 = (Actor*)pairHeader.actors[0]->userData;
			//Actor* act1 = (Actor*)pairHeader.actors[1]->userData;

			const PxContactPair& cp = pairs[i];
			Actor* act0 = (Actor*)cp.shapes[0]->userData;
			Actor* act1 = (Actor*)cp.shapes[1]->userData;
			if (!(act0&&act1))continue;
			auto script0 = act0->GetComponent<ScriptComponent>();
			auto script1 = act1->GetComponent<ScriptComponent>();

			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if (script0)
					script0->OnCollide(act1);
				if (script1)
					script1->OnCollide(act0);

				//break;

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
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				if (script0)
					script0->LostCollide(act1);
				if (script1)
					script1->LostCollide(act0);

				//break;
			}
		}
	}
	void onTrigger(PxTriggerPair *pairs, PxU32 count) override{
		//if (g_SimulationInitialize)return;
		for (PxU32 i = 0; i < count; i++)
		{
			const PxTriggerPair& cp = pairs[i];

			Actor* act0 = (Actor*)cp.triggerShape->userData;
			Actor* act1 = (Actor*)cp.otherShape->userData;
			if (!(act0&&act1))continue;
			auto script0 = act0->GetComponent<ScriptComponent>();
			auto script1 = act1->GetComponent<ScriptComponent>();

			if (cp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if (script0)
					script0->OnCollide(act1);
				if (script1)
					script1->OnCollide(act0);

				//break;
			}
			if (cp.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				if (script0)
					script0->LostCollide(act1);
				if (script1)
					script1->LostCollide(act0);

				//break;

			}
		}
	}
};

std::unordered_map<unsigned int, unsigned int> mCollideFiler;

PxFilterFlags SampleSubmarineFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	(void)constantBlock;
	(void)constantBlockSize;
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;

	}
	else{
		pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eTRIGGER_DEFAULT;
	}

	bool kinematic0 = PxFilterObjectIsKinematic(attributes0);
	bool kinematic1 = PxFilterObjectIsKinematic(attributes1);

	if (kinematic0 && kinematic1)
	{
		return PxFilterFlag::eSUPPRESS;
	}

	//if (filterData0.word3 & filterData1.word3)
	//{
	//}

	pairFlags |= PxPairFlag::eRESOLVE_CONTACTS;
	pairFlags |= PxPairFlag::eCCD_LINEAR;

	// generate contacts for all that were not filtered above

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	//if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	if (mCollideFiler[filterData0.word0] & filterData1.word0){
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return PxFilterFlag::eDEFAULT;
	}
	return PxFilterFlag::eSUPPRESS;
}

PX_INLINE void addForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup)
{
	/*	if(mode == PxForceMode::eACCELERATION || mode == PxForceMode::eVELOCITY_CHANGE)
	{
	Ps::getFoundation().error(PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__,
	"PxRigidBodyExt::addForce methods do not support eACCELERATION or eVELOCITY_CHANGE modes");
	return;
	}*/

	const PxTransform globalPose = body.getGlobalPose();
	const PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

	const PxVec3 torque = (pos - centerOfMass).cross(force);
	body.addForce(force, mode, wakeup);
	body.addTorque(torque, mode, wakeup);
}

static void addForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup = true)
{
	//transform pos to world space
	const PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

	addForceAtPosInternal(body, force, globalForcePos, mode, wakeup);
}

void PhysX3Main::onShapeHit(const PxControllerShapeHit& hit){
	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
	if (actor)
	{
		if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			return;

		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
		// particular objects, if the gameplay requires it.
		const PxVec3 upVector = hit.controller->getUpDirection();
		const PxF32 dp = hit.dir.dot(upVector);
		//		shdfnd::printFormatted("%f\n", fabsf(dp));
		if (fabsf(dp)<1e-3f)
			//		if(hit.dir.y==0.0f)
		{
			const PxTransform globalPose = actor->getGlobalPose();
			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
			::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
		}
	}
}


PxControllerBehaviorFlags PhysX3Main::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	(void)shape;
	const PxRigidDynamic* body = actor.is<PxRigidDynamic>();
	if (body)
	{
		if (body->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			//�X���C�h�t���O�𗧂Ă�ƍ�Ŕ����Ɋ���
			//PxControllerBehaviorFlag::eCCT_SLIDE
			return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
	}
	else{
		//return PxControllerBehaviorFlag::eCCT_SLIDE;
		return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
	}
	
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags PhysX3Main::getBehaviorFlags(const PxController&)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags PhysX3Main::getBehaviorFlags(const PxObstacle&)
{
	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxSceneQueryHitType::Enum PhysX3Main::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxSceneQueryFlags& queryFlags)
{
	(void)filterData;
	(void)shape;
	(void)actor;
	(void)queryFlags;
	//if (actor->userData == shape->getActor()->userData){
	//
	//	return PxSceneQueryHitType::eBLOCK;
	//}

	return PxSceneQueryHitType::eBLOCK;
	//return PxSceneQueryHitType::eBLOCK;
}

PxSceneQueryHitType::Enum PhysX3Main::postFilter(const PxFilterData& filterData, const PxSceneQueryHit& hit)
{
	(void)filterData;
	(void)hit;
	//if (hit.actor->userData == hit.shape->getActor()->userData){
	//
	//	return PxSceneQueryHitType::eBLOCK;
	//}
	
	return PxSceneQueryHitType::eBLOCK;
	//return PxSceneQueryHitType::eBLOCK;
}


PhysX3Main::PhysX3Main()
:gPhysicsSDK(NULL)
,mFoundation(NULL)
,gDefaultFilterShader(SampleSubmarineFilterShader)
,gScene(NULL)
,mCooking(NULL)
,mEngineScene(NULL)
,myTimestep(1.0f/60.0f)
,mMaterial(NULL)
, mProfileZoneManager(NULL)
, mControllerManager(NULL)
, mRigidStatic(NULL)
{
}

PhysX3Main::~PhysX3Main(){
	ShutdownPhysX();
}

TestOn* mTestOn = NULL;
PxDefaultCpuDispatcher* mCpuDispatcher = NULL;
PxVec3 gravity(0, -9.81f, 0);
void PhysX3Main::InitializePhysX() {

	for (int I = 0; I < _LAYER_NUM; I++){
		int i = 1 << I;
		mCollideFiler[i] = 0xFFFFFFFF;
	}


	_SYSTEM_LOG_H("PhysX�̏�����");

	//mhbox = MV1LoadModel("res/PhysX/box.mqo");
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	if (!mFoundation){
		std::cerr << "PxCreateFoundation failed!" << std::endl;

		_SYSTEM_LOG_ERROR("Foundation�̍쐬");
		exit(1);
	}

	mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
	if (!mProfileZoneManager){
		std::cerr << "PxProfileZoneManager::createProfileZoneManager failed!" << std::endl;
		_SYSTEM_LOG_ERROR("ProfileZoneManager�̍쐬");
		exit(1);
	}

	PxTolerancesScale scale;
	scale.mass = 1000;
	scale.length = 1;        // typical length of an object
	//�������~����X�s�[�h
	scale.speed = 0.1f;         // typical speed of an object, gravity*1s is a reasonable choice

	bool recordMemoryAllocations = false;
	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		scale, recordMemoryAllocations, mProfileZoneManager);
	if (gPhysicsSDK == NULL) {
		std::cerr << "Error creating PhysX device." << std::endl;
		std::cerr << "Exiting..." << std::endl;
		_SYSTEM_LOG_ERROR("PhysiXDevice�̍쐬");
		exit(1);
	}
	if (!PxInitExtensions(*gPhysicsSDK)){
		std::cerr << "PxInitExtensions failed!" << std::endl;
		_SYSTEM_LOG_H_ERROR();
		_SYSTEM_LOG_ERROR("PhysiXDevice��Initialize");
	}

	PxCookingParams cookingParam(scale);
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, cookingParam);


	//Create the scene
	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = gravity;
	if (!sceneDesc.cpuDispatcher) {
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!mCpuDispatcher){
			std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;

			_SYSTEM_LOG_H_ERROR();
			_SYSTEM_LOG_ERROR("CpuDispatcher�̍쐬");
		}
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	else{
		_SYSTEM_LOG_H_ERROR();
	}
	if (!sceneDesc.filterShader){
		sceneDesc.filterShader = gDefaultFilterShader;
	}
	else{
		_SYSTEM_LOG_H_ERROR();
	}

	// | PxSceneFlag::eDISABLE_CONTACT_CACHE | PxSceneFlag::eDISABLE_CONTACT_REPORT_BUFFER_RESIZE
	sceneDesc.flags |= PxSceneFlag::eENABLE_KINEMATIC_PAIRS | PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS | PxSceneFlag::eENABLE_ACTIVETRANSFORMS | PxSceneFlag::eENABLE_CCD;
	mTestOn = new TestOn();
	sceneDesc.simulationEventCallback = mTestOn;

	gScene = gPhysicsSDK->createScene(sceneDesc);
	if (!gScene){
		std::cerr << "createScene failed!" << std::endl;
		_SYSTEM_LOG_H_ERROR();
		_SYSTEM_LOG_ERROR("Scene�̍쐬");
	}
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	mEngineScene = gPhysicsSDK->createScene(sceneDesc);
	if (!mEngineScene){
		std::cerr << "createScene failed!" << std::endl;
		_SYSTEM_LOG_H_ERROR();
		_SYSTEM_LOG_ERROR("Scene�̍쐬");
	}
	mEngineScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	mEngineScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	mMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);


	{
		mControllerManager = PxCreateControllerManager(*gScene);
	}


	mRigidDynamic = (PxRigidDynamic*)createBody();
	mRigidDynamic->setRigidDynamicFlag(PxRigidBodyFlag::eKINEMATIC, true);
	AddActor(mRigidDynamic);
	mRigidStatic = createRigidStatic();
	AddActor(mRigidStatic);

	//createPlane();


	//gScene->setSimulationEventCallback(new TestOn());

	//gScene->simulate(myTimestep);
}
void PhysX3Main::createPlane(){
	//Create actors 
	//1) Create ground plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysicsSDK->createRigidStatic(pose);
	if (!plane){
		std::cerr << "create plane failed!" << std::endl;

		_SYSTEM_LOG_ERROR("PhysX Shape Plane�̍쐬");
	}
	//PxShape* shape = plane->createShape(PxPlaneGeometry(), *mMaterial);
	//if (!shape)
	//	std::cerr << "create shape failed!" << std::endl;
	gScene->addActor(*plane);

	PxRigidActor* act = plane;
	auto flag = act->getActorFlags();
	flag |= PxActorFlag::eSEND_SLEEP_NOTIFIES;
	act->setActorFlags(flag);
}

PxRigidActor* PhysX3Main::createBody(){
  
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxRigidDynamic *actor = gPhysicsSDK->createRigidDynamic(transform);
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	if (!actor){

		_SYSTEM_LOG_ERROR("PhysX Actor�̍쐬");
		std::cerr << "create actor failed!" << std::endl;
	}

	PxRigidActor* act = actor;
	auto flag = act->getActorFlags();
	//flag |= PxActorFlag::eSEND_SLEEP_NOTIFIES;
	act->setActorFlags(flag);
	
	return act;
}


PxRigidStatic* PhysX3Main::createRigidStatic(){
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxRigidStatic *actor = gPhysicsSDK->createRigidStatic(transform);
	if (!actor){

		_SYSTEM_LOG_ERROR("PhysX Actor�̍쐬");
		std::cerr << "create actor failed!" << std::endl;
	}

	auto flag = actor->getActorFlags();
	//flag |= PxActorFlag::eSEND_SLEEP_NOTIFIES;
	actor->setActorFlags(flag);

	return actor;
}

PxRigidActor* PhysX3Main::createBodyEngine(){

	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxRigidDynamic *actor = gPhysicsSDK->createRigidDynamic(transform);
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	if (!actor){

		_SYSTEM_LOG_ERROR("PhysX Shape Plane�̍쐬");
		std::cerr << "create actor failed!" << std::endl;
	}

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
	auto shape = gPhysicsSDK->createShape(geometry, *mMaterial, true , PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE);

	PxFilterData  filterData;
	filterData.word0 = Layer::None;  //���[�h0 =������ID 
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	return shape;
}

PxShape* PhysX3Main::CreateShapeSphere(){
	//PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxSphereGeometry geometry(0.5);
	auto shape = gPhysicsSDK->createShape(geometry, *mMaterial, true, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE);
	PxFilterData  filterData;
	filterData.word0 = Layer::None;  //���[�h0 =������ID 
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	return shape;

}

PxShape * PhysX3Main::CreateShapeCapsule()
{
	PxCapsuleGeometry geometry(0.5,0.5);
	auto shape = gPhysicsSDK->createShape(geometry, *mMaterial, true, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE);
	PxFilterData  filterData;
	filterData.word0 = Layer::None;  //���[�h0 =������ID 
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	return shape;
}

PxShape* PhysX3Main::CreateTriangleMesh(const IPolygonsData* poly){
	if (!poly)return NULL;
	auto vnum = poly->GetVertexNum();
	//std::vector<PxVec3> vertexVec3(VertexNum);
	std::vector<PxVec3> vertexVec3(vnum);
	for (int i = 0; i < vnum; i++){
		auto Pos = poly->GetVertexPos(i);
		vertexVec3[i] = PxVec3(Pos.x, Pos.y, Pos.z);
	}

	//PxBoundedData vertexdata;
	//vertexdata.count = vertex.size();
	//vertexdata.stride = sizeof(PxVec3);
	//vertexdata.data = vertexVec3.data();


	auto inum = poly->GetIndexNum();
	//std::vector<PxU32> indexU32;
	std::vector<PxU32> indexU32(inum);
	for (int i = 0; i < inum; i++){
		indexU32[i] = (PxU32)poly->GetIndex(i);
	}

	//PxBoundedData indexdata;
	//indexdata.count = index.size() / 3;
	//indexdata.stride = sizeof(PolygonsData::IndexType) * 3;
	//indexdata.data = indexU32.data();


	// ���b�V���f�[�^�̃p�����[�^��ݒ�
	PxTriangleMeshDesc triangleDesc;
	// ���_�f�[�^
	triangleDesc.points.count = vertexVec3.size();
	triangleDesc.points.stride = sizeof(PxVec3);
	triangleDesc.points.data = vertexVec3.data();
	// �O�p�`�̒��_�C���f�b�N�X
	triangleDesc.triangles.count = indexU32.size() / 3;
	triangleDesc.triangles.stride = sizeof(PxU32) * 3;
	triangleDesc.triangles.data = indexU32.data();

	//triangleDesc.flags = PxMeshFlag;

	PxDefaultMemoryOutputStream writeBuffer;
	bool status = mCooking->cookTriangleMesh(triangleDesc, writeBuffer);
	if (!status){

		_SYSTEM_LOG_ERROR("PhysX Shape TriangleMesh�̍쐬");
		return NULL;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	// ���b�V�����쐬����
	auto mesh =  gPhysicsSDK->createTriangleMesh(readBuffer);

	PxTriangleMeshGeometry meshGeo(mesh);
	//PxShapeFlag::eSIMULATION_SHAPE
	auto shape = gPhysicsSDK->createShape(meshGeo, *mMaterial, true, PxShapeFlag::eSCENE_QUERY_SHAPE);

	PxFilterData  filterData;
	filterData.word0 = Layer::None;  //���[�h0 =������ID 
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	return shape;
}
PxShape* PhysX3Main::CreateConvexMesh(const IPolygonsData* poly) {
	if (!poly)return NULL;
	auto vnum = poly->GetVertexNum();
	//std::vector<PxVec3> vertexVec3(VertexNum);
	std::vector<PxVec3> vertexVec3(vnum);
	for (int i = 0; i < vnum; i++) {
		auto Pos = poly->GetVertexPos(i);
		vertexVec3[i] = PxVec3(Pos.x, Pos.y, Pos.z);
	}

	//PxBoundedData vertexdata;
	//vertexdata.count = vertex.size();
	//vertexdata.stride = sizeof(PxVec3);
	//vertexdata.data = vertexVec3.data();


	auto inum = poly->GetIndexNum();
	//std::vector<PxU32> indexU32;
	std::vector<PxU32> indexU32(inum);
	for (int i = 0; i < inum; i++) {
		indexU32[i] = (PxU32)poly->GetIndex(i);
	}

	//PxBoundedData indexdata;
	//indexdata.count = index.size() / 3;
	//indexdata.stride = sizeof(PolygonsData::IndexType) * 3;
	//indexdata.data = indexU32.data();


	// ���b�V���f�[�^�̃p�����[�^��ݒ�
	PxConvexMeshDesc triangleDesc;
	// ���_�f�[�^
	triangleDesc.points.count = vertexVec3.size();
	triangleDesc.points.stride = sizeof(PxVec3);
	triangleDesc.points.data = vertexVec3.data();
	// �O�p�`�̒��_�C���f�b�N�X
	triangleDesc.triangles.count = indexU32.size() / 3;
	triangleDesc.triangles.stride = sizeof(PxU32) * 3;
	triangleDesc.triangles.data = indexU32.data();

	//triangleDesc.flags = PxMeshFlag;

	PxDefaultMemoryOutputStream writeBuffer;
	bool status = mCooking->cookConvexMesh(triangleDesc, writeBuffer);
	if (!status) {

		_SYSTEM_LOG_ERROR("PhysX Shape ConvexMesh�̍쐬");
		return NULL;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	// ���b�V�����쐬����
	auto mesh = gPhysicsSDK->createConvexMesh(readBuffer);

	PxConvexMeshGeometry meshGeo(mesh);
	//PxShapeFlag::eSIMULATION_SHAPE
	auto shape = gPhysicsSDK->createShape(meshGeo, *mMaterial, true, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE);

	PxFilterData  filterData;
	filterData.word0 = Layer::None;  //���[�h0 =������ID 
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);

	return shape;
}

PxController* PhysX3Main::CreateController(){
	PxBoxControllerDesc desc;
	desc.halfHeight = 0.5f;

	//PxCapsuleControllerDesc desc;
	//desc.height = 0.1f;
	//desc.radius = 0.5f;

	desc.material = mMaterial;
	desc.callback = this;
	desc.reportCallback = this;
	desc.behaviorCallback = this;

	PxController* c = mControllerManager->createController(desc);
	return c;
}

PxRevoluteJoint* PhysX3Main::CreateRevoluteJoint(){
	auto joint = PxRevoluteJointCreate(*gPhysicsSDK, mRigidDynamic, PxTransform(PxVec3(0), PxQuat(1.0f)), NULL, PxTransform(PxVec3(0), PxQuat(1.0f)));
	return joint;

}
PxDistanceJoint* PhysX3Main::CreateDistanceJoint(){
	auto joint = PxDistanceJointCreate(*gPhysicsSDK, mRigidDynamic, PxTransform(PxVec3(0), PxQuat(1.0f)), NULL, PxTransform(PxVec3(0), PxQuat(1.0f)));
	return joint;
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

void PhysX3Main::EngineDisplay() {

	myTimestep = Game::GetDeltaTime()->GetDeltaTime();

	if (mEngineScene && myTimestep > 0){

		mEngineScene->simulate(myTimestep);
		//mEngineScene->checkResults(true);
		mEngineScene->fetchResults(true);

	}

}
void PhysX3Main::DisplayInitialize() {
	if (gScene)
	{
		//gScene->setFlag(PxSceneFlag::eENABLE_STABILIZATION, false);
		//gScene->setFlag(PxSceneFlag::eENABLE_STABILIZATION, true);
		//gScene->flushSimulation(false);
	}

}

void PhysX3Main::Display() {

	myTimestep = Game::GetDeltaTime()->GetDeltaTime();
	//Update PhysX 
	if (gScene && myTimestep > 0)
	{
		//�V�~�����[�V����������̂��Ȃ��Əo�͂��ł�
		gScene->simulate(myTimestep);

		//�V���~���[�V�������ɁH�쐬Shape���H�쐬�ł��Ȃ�
		//...perform useful work here using previous frame's state data
		//gScene->checkResults(true);
		gScene->fetchResults(true);
	}

}

void PhysX3Main::ShutdownPhysX() {



	if (mRigidDynamic)mRigidDynamic->release();
	if (mRigidStatic)mRigidStatic->release();

	{
		physx::PxActorTypeSelectionFlags t;
		t |= physx::PxActorTypeSelectionFlag::eRIGID_STATIC;
		t |= physx::PxActorTypeSelectionFlag::eRIGID_DYNAMIC;
		t |= physx::PxActorTypeSelectionFlag::ePARTICLE_FLUID;
		t |= physx::PxActorTypeSelectionFlag::ePARTICLE_SYSTEM;
		t |= physx::PxActorTypeSelectionFlag::eCLOTH;

		int n = gScene->getNbActors(t);
		std::vector<physx::PxActor*> buffer(n);
		gScene->getActors(t, buffer.data(), n);

		for (UINT i = 0; i < buffer.size(); i++)
		{
			gScene->removeActor(*buffer[i]);
buffer[i]->release();
		}
	}
	{
	physx::PxActorTypeSelectionFlags t;
	t |= physx::PxActorTypeSelectionFlag::eRIGID_STATIC;
	t |= physx::PxActorTypeSelectionFlag::eRIGID_DYNAMIC;
	t |= physx::PxActorTypeSelectionFlag::ePARTICLE_FLUID;
	t |= physx::PxActorTypeSelectionFlag::ePARTICLE_SYSTEM;
	t |= physx::PxActorTypeSelectionFlag::eCLOTH;

	int n = mEngineScene->getNbActors(t);
	std::vector<physx::PxActor*> buffer(n);
	mEngineScene->getActors(t, buffer.data(), n);

	for (UINT i = 0; i < buffer.size(); i++)
	{
		mEngineScene->removeActor(*buffer[i]);
		buffer[i]->release();
	}
	}
	if (mTestOn)delete mTestOn;
	if (gScene)gScene->release();
	if (mEngineScene)mEngineScene->release();
	if (mMaterial)mMaterial->release();
	if (mCpuDispatcher)mCpuDispatcher->release();
	if (mCooking)mCooking->release();
	if (mControllerManager)mControllerManager->purgeControllers();
	if (mControllerManager)mControllerManager->release();
	if (gPhysicsSDK)gPhysicsSDK->release();
	if (mProfileZoneManager)mProfileZoneManager->release();
	if (mFoundation)mFoundation->release();
}

unsigned int PhysX3Main::GetLayerCollideFlag(Layer::Enum l1)
{
	return mCollideFiler[l1];
}

bool PhysX3Main::GetLayerCollideFlag(Layer::Enum l1, Layer::Enum l2) {
	return (bool)(mCollideFiler[l1] & l2);
}
void PhysX3Main::SetLayerCollideFlag(Layer::Enum l1, Layer::Enum l2, bool flag) {
	if (flag) {
		bool f = mCollideFiler[l1] & l2;
		if (!f) {
			mCollideFiler[l1] ^= l2;
			mCollideFiler[l2] ^= l1;
		}
	}
	else {
		bool f = mCollideFiler[l1] & l2;
		if (f) {
			mCollideFiler[l1] ^= l2;
			mCollideFiler[l2] ^= l1;
		}
	}
}
void PhysX3Main::AddActor(PxActor* act) {
	gScene->addActor(*act);
}
void PhysX3Main::AddActorEngine(PxActor* act) {
	mEngineScene->addActor(*act);
}
void PhysX3Main::RemoveActor(PxActor* act) {
	gScene->removeActor(*act);
}
void PhysX3Main::RemoveActorEngine(PxActor* act) {
	mEngineScene->removeActor(*act);
}

void PhysX3Main::AddStaticShape(PxShape* shape) {
	mRigidStatic->attachShape(*shape);
}
void PhysX3Main::RemoveStaticShape(PxShape* shape) {
	mRigidStatic->detachShape(*shape);
}

GameObject PhysX3Main::Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance, Layer::Enum layer) {
	PxVec3 ori(pos.x, pos.y, pos.z);
	PxVec3 _dir(dir.x, dir.y, dir.z);
	PxReal dis = distance;
	PxHitFlags hitflag = PxHitFlag::eDEFAULT;
	PxRaycastHit hit;
	PxQueryFilterData filter;
	filter.data.word0 = (PxU32)layer;
	filter.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eRESERVED | PxQueryFlag::eNO_BLOCK | PxQueryFlag::ePOSTFILTER | PxQueryFlag::eANY_HIT;
	if (gScene->raycastSingle(ori, _dir, dis, hitflag, hit, filter)) {
		if (auto act = (Actor*)hit.shape->userData) {
			return act->shared_from_this();
		}
	}
	return NULL;
}

GameObject PhysX3Main::RaycastTop(const XMVECTOR & pos, const XMVECTOR & dir, float distance, Layer::Enum layer)
{
	std::list<::RaycastHit> hits;

	//�I�������������o
	Raycast(pos, dir, distance, [&](::RaycastHit* hit) {
		hits.push_back(*hit);
	}, layer);
	//���o�ɐ���
	if (hits.size() >= 1) {

		hits.sort([](auto &hit1, auto &hit2) {
			return hit1.distance < hit2.distance;
		});
		return hits.front().hit;
	}
	//���o�Ɏ��s
	return NULL;
}

void PhysX3Main::Raycast(const XMVECTOR & pos, const XMVECTOR & dir, float distance, const std::function<void(::RaycastHit*)>& collback, Layer::Enum layer)
{
	PxVec3 ori(pos.x, pos.y, pos.z);
	PxVec3 _dir(dir.x, dir.y, dir.z);
	PxReal dis = distance;
	PxHitFlags hitflag = PxHitFlag::eDEFAULT;
	const PxU32 arraySize = 64;
	PxRaycastHit hits[arraySize];
	bool blockingHit;

	if (int hitNum = gScene->raycastMultiple(ori, _dir, dis, hitflag, hits, arraySize, blockingHit)) {
		for (int i = 0; i < hitNum; i++) {
			::RaycastHit result;
			auto& hit = hits[i];

			result.hit = NULL;
			if (auto act = (Actor*)hit.shape->userData) {
				result.hit = (GameObject)act->shared_from_this();
			}
			result.normal = XMVectorSet(hit.normal.x, hit.normal.y, hit.normal.z, 1.0f);
			result.position = XMVectorSet(hit.position.x, hit.position.y, hit.position.z, 1.0f);
			result.distance = hit.distance;

			collback(&result);
		}
	}
}

bool PhysX3Main::RaycastHit(const XMVECTOR& pos, const XMVECTOR& dir, float distance, ::RaycastHit* result, Layer::Enum layer){
	if (!result)return false;
	PxVec3 ori(pos.x, pos.y, pos.z);
	PxVec3 _dir(dir.x, dir.y, dir.z);
	PxReal dis = distance;
	PxHitFlags hitflag = PxHitFlag::eDEFAULT | PxHitFlag::eNORMAL | PxHitFlag::ePOSITION;
	PxRaycastHit hit;
	PxQueryFilterData filter;
	filter.data.word0 = (PxU32)layer;
	filter.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eRESERVED | PxQueryFlag::eNO_BLOCK | PxQueryFlag::ePOSTFILTER | PxQueryFlag::eANY_HIT;
	if (gScene->raycastSingle(ori, _dir, dis, hitflag, hit, filter)){
		result->hit = NULL;
		if (auto act = (Actor*)hit.shape->userData) {
			auto obj = act->shared_from_this();
			result->hit = (GameObject)obj;
		}
		result->normal = XMVectorSet(hit.normal.x, hit.normal.y, hit.normal.z, 1.0f);
		result->position = XMVectorSet(hit.position.x, hit.position.y, hit.position.z,1.0f);
		result->distance = hit.distance;
		return true;
	}
	return false;
}

GameObject PhysX3Main::EngineSceneRaycast(const XMVECTOR& pos, const XMVECTOR& dir) {
	PxVec3 ori(pos.x, pos.y, pos.z);
	PxVec3 _dir(dir.x, dir.y, dir.z);
	PxReal dis = 100;
	PxHitFlags hitflag = PxHitFlag::eDEFAULT;
	PxRaycastHit hit;

	if (mEngineScene->raycastSingle(ori, _dir, dis, hitflag, hit)) {
		if (auto act = (Actor*)hit.shape->userData) {
			return act->shared_from_this();
		}
	}
	return NULL;
}

void PhysX3Main::EngineSceneRaycast(const XMVECTOR & pos, const XMVECTOR & dir, const std::function<void(::RaycastHit*)>& collback)
{

	PxVec3 ori(pos.x, pos.y, pos.z);
	PxVec3 _dir(dir.x, dir.y, dir.z);
	PxReal dis = 100;
	PxHitFlags hitflag = PxHitFlag::eDEFAULT;
	const PxU32 arraySize = 64;
	PxRaycastHit hits[arraySize];
	bool blockingHit;

	if (int hitNum = mEngineScene->raycastMultiple(ori, _dir, dis, hitflag, hits, arraySize, blockingHit)) {
		for (int i = 0; i < hitNum; i++) {
			::RaycastHit result;
			auto& hit = hits[i];

			result.hit = NULL;
			if (auto act = (Actor*)hit.shape->userData) {
				result.hit = (GameObject)act->shared_from_this();
			}
			result.normal = XMVectorSet(hit.normal.x, hit.normal.y, hit.normal.z, 1.0f);
			result.position = XMVectorSet(hit.position.x, hit.position.y, hit.position.z, 1.0f);
			result.distance = hit.distance;

			collback(&result);
		}
	}
}

#include "Game/Component/PhysXColliderComponent.h"
int PhysX3Main::OverlapHitMultiple(weak_ptr<PhysXColliderComponent> collder,const std::function<void(GameObject)>& collback, Layer::Enum layer) {
	if (!collder)return 0;
	auto shape = collder->GetShape();
	auto geo = shape->getGeometry().any();

	auto quat = collder->gameObject->mTransform->WorldQuaternion();
	auto pos = collder->gameObject->mTransform->WorldPosition();
	PxTransform parTrans(pos.x, pos.y, pos.z, PxQuat(quat.x, quat.y, quat.z, quat.w));
	parTrans = shape->getLocalPose() * parTrans;

	static PxOverlapHit hit[256];
	//PxQueryFilterData filter;
	//filter.data.word0 = layer;
	//filter.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eRESERVED | PxQueryFlag::eNO_BLOCK | PxQueryFlag::ePOSTFILTER;
	if (int hitNum = gScene->overlapMultiple(geo, parTrans, hit,256)) {
		for (int i = 0; i < hitNum; i++) {
			auto act = (Actor*)hit[i].shape->userData;
			collback(act->shared_from_this());
		}
		return hitNum;
	}
	return 0;
}