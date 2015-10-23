#ifndef _NXACTORDRAWUTIL_H_
#define _NXACTORDRAWUTIL_H_


class NxActor;
class NxShape;
class NxCapsuleShape;
class NxBoxShape;
class NxSphereShape;
class NxConvexShape;

class NxActorDrawUtil{
public:
	static void initialize();
	static void Finish();
	static void draw(NxActor* actor);

private:
	static void drawNxShape(NxShape* shape);
	static void drawNxCapsuleShape(NxCapsuleShape* capsule);
	static void drawNxBoxShape(NxBoxShape* box);
	static void drawNxSphereShape(NxSphereShape* sphere);
	static void drawNxConvexMesh(NxConvexShape* convexMesh);

};

#endif