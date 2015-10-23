#include "PhysXMain.h"
#include <algorithm>
#include "NxActorDrawUtil.h"
#include "DxLib.h"

#include "InputState.h"



NxActorFactory PhysXMain::mActorFactory;
NxScene* PhysXMain::mScene = 0;

/*===========================================================================*/
/**
* コンストラクタ．
*/
/*===========================================================================*/
PhysXMain::PhysXMain() :
mPhysicsSDX(0),
mGravity(0, -981.0f*2.0f, 0)
{
}

void PhysXMain::Initialize()
{
	mPhysicsSDX = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = mGravity;
	sceneDesc.broadPhase = NX_BROADPHASE_COHERENT;
	sceneDesc.collisionDetection = true;
	//sceneDesc.userContactReport = this;
	mScene = mPhysicsSDX->createScene(sceneDesc);

	NxMaterial* defaultMaterial = mScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.3f);
	defaultMaterial->setStaticFriction(0.9f);
	defaultMaterial->setDynamicFriction(0.8f);

	mActorFactory.setScene(mScene);
	mActorFactory.registerPlan("Ground", "Resource/PhysX/Ground.txt");
	mActorFactory.registerPlan("Box", "Resource/PhysX/Box.txt");
	mActorFactory.registerPlan("Okiagari", "Resource/PhysX/Okiagari.txt");
	mActorFactory.registerPlan("Death", "Resource/PhysX/Death.txt");
	mActorFactory.registerPlan("BillBox", "Resource/PhysX/BillBox.txt");
	mActorFactory.registerPlan("Shpere", "Resource/PhysX/Shpere.txt");
	mActorFactory.registerPlan("Capsule", "Resource/PhysX/Capsule.txt");
	mActorFactory.registerPlan("PlayerCapsule", "Resource/PhysX/PlayerCapsule.txt");
	mActorFactory.registerPlan("Tree", "Resource/PhysX/Tree.txt");
	mActorFactory.registerPlan("Stage", "Resource/PhysX/Stage.txt");
	mActorFactory.registerPlan("Pyramid", "Resource/PhysX/Pyramid.txt");
	mActorFactory.registerPlan("Tuti", "Resource/PhysX/Tuti.txt");
	mActorFactory.create("Stage");

	//mActorFactory.create("Pyramid")->setGlobalPosition(NxVec3(5000,5000,5000));

	//mActorFactory.create("BillBox");

	NxActorDrawUtil::initialize();

	//mScene->setActorPairFlags(*mOkiagariActor, *mBoxActor,
	//	NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
}

/*===========================================================================*/
/**
* 更新．
*
* @param [in] frameTimer フレームタイマ．
*/
/*===========================================================================*/
bool input = false;
void PhysXMain::Update()
{
	input = GetInputTrigger(KEY_INPUT_Z) ? true : false;

	mScene->simulate((1.0f / 60.0f));
	mScene->flushStream();
	mScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

void PhysXMain::ModelSetup(){

	if (input){
		//mActorFactory.create("Box")->setGlobalPosition(NxVec3(0, 800, 0));
		//mActorFactory.create("Okiagari")->setGlobalPosition(NxVec3(0, 1200, 0));
		//mActorFactory.create("Shpere")->setGlobalPosition(NxVec3(0, 1400, 0));
		//mActorFactory.create("Capsule")->setGlobalPosition(NxVec3(0, 1600, 0));
	}
}
/*===========================================================================*/
/**
* 描画．
*/
/*===========================================================================*/
void PhysXMain::Draw()
{
	return;
	std::for_each(mScene->getActors(),
		mScene->getActors() + mScene->getNbActors(),
		NxActorDrawUtil::draw);
}

/*===========================================================================*/
/**
* 終了処理．
*/
/*===========================================================================*/
void PhysXMain::Finish()
{

	NxActorDrawUtil::Finish();

	mPhysicsSDX->releaseScene(*mScene);
	mPhysicsSDX->release();
	mScene = 0;
}

/*========= End of File =====================================================*/

void PhysXMain::ReleaseActor(NxActor& actor){
	if (mScene == 0)return;
		mScene->releaseActor(actor);
}

float g_scale;
NxActor* PhysXMain::Create(const std::string& id){
	return mActorFactory.create(id);
}

void PhysXMain::SetScaleNxActor(NxActor* actor, float scale){
	g_scale = scale;
	std::for_each(
		actor->getShapes(),
		actor->getShapes() + actor->getNbShapes(),
		SetScaleNxShape);

	NxVec3 pos = actor->getCMassLocalPosition();
	actor->setCMassOffsetLocalPosition(pos*g_scale);
}

void PhysXMain::SetScaleNxShape(NxShape* shape){

	//NxMat33 mat = shape->getLocalPose().M * g_scale;
	//NxVec3 t = shape->getLocalPose().t * g_scale;
	//NxMat34 mat2(mat, t);
	//shape->setLocalPose(mat2);

	//mActorDesc.density *= scale;

	if (shape->isCapsule() != 0){
		SetScaleNxCapsuleShape(shape->isCapsule());
	}
	else if (shape->isBox() != 0){
		SetScaleNxBoxShape(shape->isBox());
	}
	else if (shape->isSphere() != 0){
		SetScaleNxSphereShape(shape->isSphere());
	}
}
void PhysXMain::SetScaleNxCapsuleShape(NxCapsuleShape* capsule){
	float h = capsule->getHeight();
	float r = capsule->getRadius();
	capsule->setHeight(h*g_scale);
	capsule->setRadius(r*g_scale);

	NxVec3 pos = capsule->getLocalPosition();
	capsule->setLocalPosition(pos*g_scale);

}
void PhysXMain::SetScaleNxBoxShape(NxBoxShape* box){
	NxVec3 dim = box->getDimensions();
	box->setDimensions(dim * g_scale);

	NxVec3 pos = box->getLocalPosition();
	box->setLocalPosition(pos*g_scale);
}
void PhysXMain::SetScaleNxSphereShape(NxSphereShape* sphere){
	float r = sphere->getRadius();
	sphere->setRadius(r * g_scale);

	NxVec3 pos = sphere->getLocalPosition();
	sphere->setLocalPosition(pos*g_scale);
}
