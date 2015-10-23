#include "NxActorCreator.h"
#include "NxConvexMeshFolder.h"
#include <string>
#include <DxLib.h>
#include "TenkoLib.h"

NxActorCreator::NxActorCreator(NxScene* scene) :
mScene(scene){

}
void NxActorCreator::initialize(){
	mActorDesc.setToDefault();
	mActorBodyDesc = NxBodyDescPtr();
	mActorShapesDescContainer.clear();
	mConvexMeshContainer.clear();
}

void NxActorCreator::setPlan(std::istream& is){
	initialize();

	NxShapeDescPtr shapeDesc;
	std::string command;
	while (command != "end"){
		is >> command;
		if (command == "//"){
			continue;
		}
		else if (command == "shape"){
			if (shapeDesc != 0){
				mActorShapesDescContainer.push_back(shapeDesc);
			}
			std::string shapeType;
			is >> shapeType;
			if (shapeType == "sphere"){
				shapeDesc = NxShapeDescPtr(new NxSphereShapeDesc());
			}
			else if (shapeType == "box"){
				shapeDesc = NxShapeDescPtr(new NxBoxShapeDesc());
			}
			else if (shapeType == "capsule"){
				shapeDesc = NxShapeDescPtr(new NxCapsuleShapeDesc());
			}
			else if (shapeType == "plane"){
				shapeDesc = NxShapeDescPtr(new NxPlaneShapeDesc());
			}
			else if (shapeType == "convex_mesh") {
				shapeDesc = NxShapeDescPtr(new NxConvexShapeDesc());
			}

		}
		else if (command == "box_dimensions"){
			is >> static_cast<NxBoxShapeDesc*>(shapeDesc.get())->dimensions.x 
				>> static_cast<NxBoxShapeDesc*>(shapeDesc.get())->dimensions.y
				>> static_cast<NxBoxShapeDesc*>(shapeDesc.get())->dimensions.z;

		}
		else if (command == "sphere_radius"){
			is >> static_cast<NxSphereShapeDesc*>(shapeDesc.get())->radius;

		}
		else if (command == "capsule_height"){
			is >> static_cast<NxCapsuleShapeDesc*>(shapeDesc.get())->height;

		}
		else if (command == "capsule_radius"){
			is >> static_cast<NxCapsuleShapeDesc*>(shapeDesc.get())->radius;

		}
		else if (command == "convex_mesh_vertices") {
			// 頂点データ数の取得
			NxU32 numVertices;
			is >> numVertices;
			// 頂点座標の取得
			std::vector<NxVec3> vertices(numVertices);
			for (NxU32 i = 0; i < numVertices; ++i) {
				is >> vertices[i].x >> vertices[i].y >> vertices[i].z;
			}
			// 凸形状メッシュの作成
			NxConvexMeshPtr convexMesh(new NxConvexMeshFolder(&mScene->getPhysicsSDK(), numVertices, &vertices[0]));
			// 凸形状メッシュコンテナに追加する
			mConvexMeshContainer.push_back(convexMesh);
			// 凸形状メッシュを設定
			static_cast<NxConvexShapeDesc*>(shapeDesc.get())->meshData = convexMesh->get();
		}
		else if (command == "convex_mesh_triangles") {
			// 三角形データ数の取得
			NxU32 numTriangles;
			is >> numTriangles;
			// 頂点インデックスの取得
			std::vector<NxU32> indices(numTriangles * 3);
			for (NxU32 i = 0; i < numTriangles; ++i) {
				is >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
			}
			// 頂点座標数の読み込み
			NxU32 numVertices;
			is >> numVertices;
			// 頂点座標の読み込み
			std::vector<NxVec3> vertices(numVertices);
			for (NxU32 i = 0; i < numVertices; ++i) {
				is >> vertices[i].x >> vertices[i].y >> vertices[i].z;
			}
			// 凸形状メッシュの作成
			NxConvexMeshPtr convexMesh(new NxConvexMeshFolder(
				&mScene->getPhysicsSDK(), numVertices, &vertices[0], numTriangles, &indices[0]));
			// 凸形状メッシュコンテナに追加する
			mConvexMeshContainer.push_back(convexMesh);
			// 凸形状メッシュを設定
			static_cast<NxConvexShapeDesc*>(shapeDesc.get())->meshData = convexMesh->get();
		}
		else if (command == "convex_mesh_loadmodel"){
			std::string name;
			is >> name;
			float s = 1;
			if (name == "scale"){
				is >> s;
			}
			is >> name;
			int ModelHandle = MV1LoadModel(name.c_str());
			if (ModelHandle == -1){
				assert(0);
				break;
			}
			MV1_REF_POLYGONLIST RefMesh;

			MV1SetScale(ModelHandle, VGet(s, s, s));
			//MV1SetPosition(ModelHandle, VGet(0, 0, 0));


			int MeshNum = MV1GetFrameNum(ModelHandle);
			for (int j = 0; j < MeshNum; j++){
				if (j != 0){
					mActorShapesDescContainer.push_back(shapeDesc);
					shapeDesc = NxShapeDescPtr(new NxConvexShapeDesc());
				}

				// 参照用メッシュの作成
				MV1SetupReferenceMesh(ModelHandle, j, TRUE);
				// 参照用メッシュの取得
				RefMesh = MV1GetReferenceMesh(ModelHandle, j, TRUE);
				{
					// 三角形データ数の取得
					NxU32 numTriangles = RefMesh.PolygonNum;
					// 頂点インデックスの取得
					std::vector<NxU32> indices(numTriangles * 3);
					for (NxU32 i = 0; i < numTriangles; ++i) {
						indices[i * 3 + 0] = RefMesh.Polygons[i].VIndex[0];
						indices[i * 3 + 1] = RefMesh.Polygons[i].VIndex[1];
						indices[i * 3 + 2] = RefMesh.Polygons[i].VIndex[2];
					}
					// 頂点座標数の読み込み
					NxU32 numVertices = RefMesh.VertexNum;
					// 頂点座標の読み込み
					std::vector<NxVec3> vertices(numVertices);
					for (NxU32 i = 0; i < numVertices; ++i) {
						vertices[i].x = RefMesh.Vertexs[i].Position.x;
						vertices[i].y = RefMesh.Vertexs[i].Position.y;
						vertices[i].z = RefMesh.Vertexs[i].Position.z;
					}
					// 凸形状メッシュの作成
					NxConvexMeshPtr convexMesh(new NxConvexMeshFolder(&mScene->getPhysicsSDK(), numVertices, &vertices[0], numTriangles, &indices[0]));

					//NxU32 numTriangles = RefMesh.PolygonNum;
					//NxU32 numVertices = numTriangles * 3;
					//std::vector<NxVec3> vertices(numVertices);
					//for (NxU32 i = 0; i < numTriangles; ++i) {
					//	vertices[i * 3 + 0] = NxVGet(RefMesh.Vertexs[RefMesh.Polygons[i].VIndex[0]].Position);
					//	vertices[i * 3 + 1] = NxVGet(RefMesh.Vertexs[RefMesh.Polygons[i].VIndex[1]].Position);
					//	vertices[i * 3 + 2] = NxVGet(RefMesh.Vertexs[RefMesh.Polygons[i].VIndex[2]].Position);
					//}
					//NxConvexMeshPtr convexMesh(new NxConvexMeshFolder(&mScene->getPhysicsSDK(), numVertices, &vertices[0]));

					// 凸形状メッシュコンテナに追加する
					mConvexMeshContainer.push_back(convexMesh);
					// 凸形状メッシュを設定
					static_cast<NxConvexShapeDesc*>(shapeDesc.get())->meshData = convexMesh->get();
				}

			}

			//MV1TerminateReferenceMesh(ModelHandle, j, TRUE);
			MV1DeleteModel(ModelHandle);
		}
		else if (command == "shape_translate"){
			is >> shapeDesc->localPose.t.x
				>> shapeDesc->localPose.t.y
				>> shapeDesc->localPose.t.z;

		}
		else if (command == "shape_rotate"){
			float x, y, z;
			is >> x >> y >> z;
			NxMat33 mx, my, mz;
			mx.rotX(NxMath::degToRad(x));
			mx.rotY(NxMath::degToRad(y));
			mx.rotZ(NxMath::degToRad(z));
			shapeDesc->localPose.M = mz * mx * my;

		}
		else if (command == "shape_material"){
			is >> shapeDesc->materialIndex;

		}
		else if (command == "body"){
			mActorBodyDesc = NxBodyDescPtr(new NxBodyDesc());

		}
		else if (command == "body_mass_rotate"){
			float x, y, z;
			is >> x >> y >> z;
			NxMat33 mx, my, mz;
			mx.rotX(NxMath::degToRad(x));
			mx.rotY(NxMath::degToRad(y));
			mx.rotZ(NxMath::degToRad(z));
			mActorBodyDesc->massLocalPose.M = mz * mx * my;

		}
		else if (command == "body_mass_translate"){
			is >> mActorBodyDesc->massLocalPose.t.x
				>> mActorBodyDesc->massLocalPose.t.y
				>> mActorBodyDesc->massLocalPose.t.z;

		}
		else if (command == "actor_density"){
			is >> mActorDesc.density;

		}
		else if (command == "actor_rotate"){
			float x, y, z;
			is >> x >> y >> z;
			NxMat33 mx, my, mz;
			mx.rotX(NxMath::degToRad(x));
			mx.rotY(NxMath::degToRad(y));
			mx.rotZ(NxMath::degToRad(z));
			mActorDesc.globalPose.M = mz * mx * my;

		}
		else if (command == "actor_translate"){
			is >> mActorDesc.globalPose.t.x
				>> mActorDesc.globalPose.t.y
				>> mActorDesc.globalPose.t.z;

		}
		else if (command != "end"){
			assert(0);
			break;
		}
	}

	if (shapeDesc != 0){
		mActorShapesDescContainer.push_back(shapeDesc);
	}
	mActorDesc.body = mActorBodyDesc.get();

	for (NxShapeDescContainer::size_type i = 0; i < mActorShapesDescContainer.size(); ++i){
		mActorDesc.shapes.pushBack(mActorShapesDescContainer[i].get());
	}
}

NxActor* NxActorCreator::operator() () const{
	NxActor* actor = mScene->createActor(mActorDesc);
	if (mActorBodyDesc != 0){
		if (mActorBodyDesc->massLocalPose.isIdentity() == false){
			actor->setCMassOffsetLocalPose(mActorBodyDesc->massLocalPose);
		}
	}
	return actor;
}