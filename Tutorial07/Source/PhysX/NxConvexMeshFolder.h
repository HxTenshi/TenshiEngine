#ifndef _NXCONVEXMESHFOLDER_H_
#define _NXCONVEXMESHFOLDER_H_
#include <NxPhysics.h>
// 凸形状メッシュフォルダ
class NxConvexMeshFolder
{
public:
	// コンストラクタ
	NxConvexMeshFolder(NxPhysicsSDK* physicsSDK, NxU32 numVertices, NxVec3* vertices,
		NxU32 numTriangles = 0, NxU32* indices = 0);
	// デストラクタ
	~NxConvexMeshFolder();
	// ポインタの取得
	NxConvexMesh* get();
private:
	// コピー禁止（未実装）
	NxConvexMeshFolder(const NxConvexMeshFolder& t);
	NxConvexMeshFolder& operator = (const NxConvexMeshFolder& t);
private:
	NxPhysicsSDK* mPhysicsSDK; // PhysicsSDK
	NxConvexMesh* mNxConvexMesh; // 凸形状メッシュ
};
#endif