
#include "NxActorDrawUtil.h"
#include <NxPhysics.h>
#include <algorithm>
#include "TenkoLib.h"

int mhbox, mhsphere, mhcylinder;
MATRIX matrix;
MATRIX matrix2;

void NxActorDrawUtil::initialize(){

	mhbox = MV1LoadModel("Resource/PhysX/box.mqo");
	mhsphere = MV1LoadModel("Resource/PhysX/sphere.mqo");
	mhcylinder = MV1LoadModel("Resource/PhysX/cylinder.mqo");
}
void NxActorDrawUtil::Finish(){

	MV1DeleteModel(mhbox);
	MV1DeleteModel(mhsphere);
	MV1DeleteModel(mhcylinder);
}

void NxActorDrawUtil::draw(NxActor* actor){

	float mat[16];
	actor->getGlobalPose().getColumnMajor44(mat);
	matrix = MGet(mat);
	std::for_each(
		actor->getShapes(),
		actor->getShapes() + actor->getNbShapes(),
		drawNxShape);

	
}

void NxActorDrawUtil::drawNxShape(NxShape* shape){

	float mat[16];
	shape->getLocalPose().getColumnMajor44(mat);
	matrix2 = MGet(mat);
	matrix2 = MMult(matrix2, matrix);

	VECTOR pos = MGetPos(matrix2);
	//if (CheckCameraViewClip(pos) == TRUE)return;

	if (shape->isCapsule() != 0){
		drawNxCapsuleShape(shape->isCapsule());
	}
	else if (shape->isBox() != 0){
		drawNxBoxShape(shape->isBox());
	}
	else if (shape->isSphere() != 0){
		drawNxSphereShape(shape->isSphere());
	}
	else if (shape->isConvexMesh() != 0) {
		drawNxConvexMesh(shape->isConvexMesh());
	}
}

void NxActorDrawUtil::drawNxCapsuleShape(NxCapsuleShape* capsule){
	{
		VECTOR tr = VGet(0, capsule->getHeight() / 2.0f, 0);
		float s = capsule->getRadius();
		VECTOR scale = VGet(s, s, s);
		MATRIX mat = MMult(MGetScale(scale), MGetTranslate(tr));
		mat = MMult(mat, matrix2);
		MV1SetMatrix(mhsphere, mat);
		MV1DrawModel(mhsphere);
	}
	{	
		float w = capsule->getRadius();
		float h = capsule->getHeight();
		VECTOR scale = VGet(w, h, w);
		MATRIX mat = MMult(MGetScale(scale), matrix2);
		MV1SetMatrix(mhcylinder, mat);
		MV1DrawModel(mhcylinder);
	}
	{
		VECTOR tr = VGet(0, -capsule->getHeight() / 2.0f, 0);
		float s = capsule->getRadius();
		VECTOR scale = VGet(s, s, s);
		MATRIX mat = MMult(MGetScale(scale), MGetTranslate(tr));
		mat = MMult(mat, matrix2);
		MV1SetMatrix(mhsphere, mat);
		MV1DrawModel(mhsphere);
	}
}
void NxActorDrawUtil::drawNxBoxShape(NxBoxShape* box){
	VECTOR scale = VGet(box->getDimensions().x * 2.0f, box->getDimensions().y * 2.0f, box->getDimensions().z * 2.0f);
	MATRIX mat = MMult(MGetScale(scale), matrix2);
	MV1SetMatrix(mhbox, mat);
	MV1DrawModel(mhbox);
}
void NxActorDrawUtil::drawNxSphereShape(NxSphereShape* sphere){
	float s = sphere->getRadius();
	VECTOR scale = VGet(s, s, s);
	MATRIX mat = MMult(MGetScale(scale), matrix2);
	MV1SetMatrix(mhsphere, mat);
	MV1DrawModel(mhsphere);
}

void NxActorDrawUtil::drawNxConvexMesh(NxConvexShape* convexMesh)
{
	// 凸形状メッシュ情報の取得
	NxConvexMeshDesc meshDesc;
	convexMesh->getConvexMesh().saveToDesc(meshDesc);
	// 頂点データを取得
	const NxVec3* points = static_cast<const NxVec3*>(meshDesc.points);
	// 頂点インデックスを取得
	const NxU32(*triangles)[3] = static_cast<const NxU32(*)[3]>(meshDesc.triangles);

	for (NxU32 i = 0; i < meshDesc.numTriangles; ++i) {
		VECTOR p1 = VTransform(VGet(points[triangles[i][0]]), matrix2);
		VECTOR p2 = VTransform(VGet(points[triangles[i][1]]), matrix2);
		VECTOR p3 = VTransform(VGet(points[triangles[i][2]]), matrix2);
		DrawTriangle3D(p1, p2, p3, GetColor(255, 255, 255), FALSE);
	}
}