
#ifndef _PhsyXApplication_H_
#define _PhsyXApplication_H_

#include <NxPhysics.h>
// PhysX物理計算ライブラリの設定
#pragma comment( lib, "PhysX/PhysXLoader.lib" )

#include "NxActorFactory.h"

/**
* PhysXアプリケーションクラス．
*/
class PhysXMain //:public NxUserContactReport
{
public:

	PhysXMain();

	void Initialize();
	void Update();
	void ModelSetup();
	void Draw();
	void Finish();

	static void ReleaseActor(NxActor& actor);
	static NxActor* Create(const std::string& id);

	//void onContactNotify(NxContactPair& pair, NxU32 events);

	static void SetScaleNxActor(NxActor* actor, float scale);

private:
	static void SetScaleNxShape(NxShape* shape);
	static void SetScaleNxCapsuleShape(NxCapsuleShape* capsule);
	static void SetScaleNxBoxShape(NxBoxShape* box);
	static void SetScaleNxSphereShape(NxSphereShape* sphere);

private:
	NxPhysicsSDK* mPhysicsSDX;
	static NxScene* mScene;
	NxVec3 mGravity;

	static NxActorFactory mActorFactory;
};

#endif