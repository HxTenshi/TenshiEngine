
#include <NxCooking.h>
#pragma comment( lib, "NxCooking.lib" )

#include "NxConvexMeshFolder.h"
#include "NxMemoryStream.h"

// コンストラクタ
NxConvexMeshFolder::NxConvexMeshFolder(NxPhysicsSDK* physicsSDK, NxU32 numVertices, NxVec3* vertices,
	NxU32 numTriangles, NxU32* indices) :
	mPhysicsSDK(physicsSDK)
{
	// 凸形状メッシュデータのパラメータを設定
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices = numVertices; // 頂点データ数
	convexDesc.pointStrideBytes = sizeof(NxVec3); // 頂点データのサイズ
	convexDesc.points = vertices; // 頂点データ
	convexDesc.numTriangles = numTriangles; // 三角形データ数
	convexDesc.triangles = indices; // 三角形の頂点インデックス
	convexDesc.triangleStrideBytes = 3 * sizeof(NxU32); // 三角形の頂点インデックスのサイズ
	convexDesc.flags = NX_CF_COMPUTE_CONVEX; // 凸形状の計算を行う
	// 凸形状メッシュデータ作成初期化
	NxInitCooking();
	// メモリバッファを作成
	NxMemoryStream buffer;
	// 凸形状メッシュデータの作成
	NxCookConvexMesh(convexDesc, buffer);
	// 凸形状メッシュを作成する
	mNxConvexMesh = mPhysicsSDK->createConvexMesh(buffer);
}
// デストラクタ
NxConvexMeshFolder::~NxConvexMeshFolder()
{
	//mPhysicsSDK->releaseConvexMesh(*mNxConvexMesh);
}
// ポインタの取得
NxConvexMesh* NxConvexMeshFolder::get()
{
	return mNxConvexMesh;
}