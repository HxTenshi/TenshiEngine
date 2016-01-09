#include "ModelBufferPMD.h"

#include "Graphic/Material/Material.h"

#include "Model.h"
#include "ConstantBuffer/ConstantBufferDif.h"
#include <string>


HRESULT ModelBufferPMD::Create(const char* FileName, Model* mpModel){

	HRESULT hr = S_OK;
	//pmdクラスを生成する。その際にコンストラクタにファイル名を教える
	std::string sFileName = FileName;
	pmd modeldata(sFileName.c_str());
	if (!modeldata.mLoadResult)return S_FALSE;
	unsigned int loc = sFileName.find_last_of("/");
	if (loc != std::string::npos && (loc + 1) != std::string::npos){
		loc += 1;
		sFileName.erase(loc);
	}

	//頂点データを取得
	UINT TYOUTEN = modeldata.vert_count;

	PMDVertex *vertices;
	vertices = new PMDVertex[TYOUTEN];

	for (int i = 0; i < TYOUTEN; i++){
		vertices[i].Pos.x = modeldata.vertex[i].pos[0];
		vertices[i].Pos.y = modeldata.vertex[i].pos[1];
		vertices[i].Pos.z = modeldata.vertex[i].pos[2];
		vertices[i].Normal.x = modeldata.vertex[i].normal_vec[0];
		vertices[i].Normal.y = modeldata.vertex[i].normal_vec[1];
		vertices[i].Normal.z = modeldata.vertex[i].normal_vec[2];
		vertices[i].Tex.x = modeldata.vertex[i].uv[0];
		vertices[i].Tex.y = modeldata.vertex[i].uv[1];
		vertices[i].BoneIndex[0] = modeldata.vertex[i].bone_num[0];
		vertices[i].BoneIndex[1] = modeldata.vertex[i].bone_num[1];
		vertices[i].BoneIndex[2] = 0;
		vertices[i].BoneIndex[3] = 0;
		vertices[i].BoneWeight[0] = modeldata.vertex[i].bone_weight;
		vertices[i].BoneWeight[1] = 100 - modeldata.vertex[i].bone_weight;
		vertices[i].BoneWeight[2] = 0;
		vertices[i].BoneWeight[3] = 0;
	}
	hr = createVertex(vertices, sizeof(PMDVertex), TYOUTEN);
	delete[] vertices;
	if (FAILED(hr))
		return hr;


	//インデックスデータを取得
	UINT INDEXSU = modeldata.face_vert_count;
	WORD *indices;
	indices = new WORD[INDEXSU];

	for (UINT i = 0; i < INDEXSU; i++){
		indices[i] = modeldata.face_vert_index[i];
	}
	hr = createIndex(indices, INDEXSU);
	delete[] indices;
	if (FAILED(hr))
		return hr;

	hr = createMaterial(modeldata.material_count, modeldata.material, sFileName, mpModel);
	if (FAILED(hr))
		return hr;

	createBone(modeldata.bone_count, modeldata.bone);

	createIk(modeldata.ik_data_count, modeldata.ik_data);

	mStride = sizeof(PMDVertex);

	return S_OK;
}

#include "Game/Component/Component.h"
HRESULT ModelBufferPMD::createMaterial(unsigned long count, pmd::t_material* material, const std::string& sFileName, Model* mpModel){
	//Resultマテリアルを消したからメッシュのみで他意味なし
	//このクラス自体もう使われない？
		HRESULT hr = S_OK;

		auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);

		//mMaterialNum = count;
		//mMaterials = new Material[mMaterialNum];
		mpModel->mMeshs.resize(count);
		UINT vartcount = 0;
		for (DWORD i = 0; i < count; i++){
			Material mat;
			mat.mDiffuse = XMFLOAT4(material[i].diffuse_color);
			mat.mSpecular = XMFLOAT4(material[i].specular_color[0], material[i].specular_color[1], material[i].specular_color[2], material[i].specularity);
			mat.mAmbient = XMFLOAT4(material[i].mirror_color[0], material[i].mirror_color[1], material[i].mirror_color[2], 1.0f);
			mpModel->mMeshs[i].mpModelBuffer = this;
			mpModel->mMeshs[i].mFace_vert_start_count = vartcount;
			mpModel->mMeshs[i].mFace_vert_count = material[i].face_vert_count;
			vartcount += material[i].face_vert_count;
			DWORD ti = material[i].toon_index;
			hr = mat.Create(cbm, cbt);
			if (FAILED(hr))
				return hr;
			if (ti != 255){
				std::string toonFile = "toon";
				toonFile += std::to_string((int)ti + 1);
				toonFile += ".bmp";
				toonFile = sFileName + toonFile;

				mat.SetTexture(toonFile.c_str(), 2);
			}

			std::string file = sFileName + material[i].texture_file_name;

			unsigned int loc = file.find("*", 0);
			if (loc != std::string::npos && (loc + 1) != std::string::npos){
				std::string subtex = file.substr(loc + 1);
				subtex = sFileName + subtex;

				mat.SetTexture(subtex.c_str(), 1);

				file.erase(loc);
			}
			mat.SetTexture(file.c_str(), 0);
		}
		return S_OK;
	}

HRESULT ModelBufferPMD::createBone(unsigned long count, pmd::t_bone* bone){

		//mBoneNum = count;
		//mBone = new Bone[mBoneNum];
		//for (DWORD i = 0; i < mBoneNum; i++){
		//	mBone[i].mStrName = bone[i].bone_name;// +"%0";
		//	mBone[i].mHierarchy.mIdxSelf = i;
		//	mBone[i].mHierarchy.mIdxParent = bone[i].parent_bidx;
		//	if (bone[i].parent_bidx >= mBoneNum) mBone[i].mHierarchy.mIdxParent = UINT(-1);
		//
		//
		//	XMVECTOR head_pos = XMVectorSet(bone[i].bone_head_pos[0], bone[i].bone_head_pos[1], bone[i].bone_head_pos[2], 0.0f);
		//	XMVECTOR parent_pos = { 0, 0, 0, 1 };
		//	if (mBone[i].mHierarchy.mIdxParent < mBoneNum){
		//		UINT p = mBone[i].mHierarchy.mIdxParent;
		//		parent_pos = XMVectorSet(bone[p].bone_head_pos[0], bone[p].bone_head_pos[1], bone[p].bone_head_pos[2], 0.0f);
		//	}
		//
		//	XMVECTOR tail_pos = { 0, 0, 0, 1 };
		//	if (bone[i].tail_pos_bidx < mBoneNum){
		//		UINT p = bone[i].tail_pos_bidx;
		//		tail_pos = XMVectorSet(bone[p].bone_head_pos[0], bone[p].bone_head_pos[1], bone[p].bone_head_pos[2], 0.0f);
		//	}
		//	XMVECTOR local_pos = XMVectorSubtract(head_pos, parent_pos);
		//
		//	mBone[i].mPos = XMFLOAT3(XMVectorGetX(local_pos), XMVectorGetY(local_pos), XMVectorGetZ(local_pos));
		//	mBone[i].mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		//	XMVECTOR q = XMQuaternionIdentity();
		//	mBone[i].mRot = XMFLOAT4(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
		//
		//	//ワールド行列計算
		//	XMVECTOR scale = { 1, 1, 1, 1 };
		//	mBone[i].mMtxPose = SRTMatrix(scale, q, local_pos);
		//	if (mBone[i].mHierarchy.mIdxParent < mBoneNum){
		//		mBone[i].mMtxPose = XMMatrixMultiply(mBone[i].mMtxPose, mBone[mBone[i].mHierarchy.mIdxParent].mMtxPose);
		//	}
		//
		//	mBone[i].mIkBoneIdx = bone[i].ik_parent_bidx;
		//
		//
		//	mBone[i].mMtxPoseInit = mBone[i].mMtxPose;
		//
		//}
		return S_OK;
	}

HRESULT ModelBufferPMD::createIk(unsigned long count, pmd::t_ik_data* ik){

	//	mIkNum = count;
	//	mIk = new Ik[mIkNum];
	//	for (DWORD i = 0; i < mIkNum; i++){
	//		mIk[i].bone_index = ik[i].ik_bone_index;
	//		mIk[i].target_bone_index = ik[i].ik_target_bone_index;
	//		mIk[i].chain_length = ik[i].ik_chain_length;
	//		mIk[i].iterations = ik[i].iterations;
	//		mIk[i].control_weight = ik[i].control_weight;
	//		for (int j = 0; j < ik[i].ik_chain_length; j++){
	//			mIk[i].child_bone_index.push_back(ik[i].ik_child_bone_index[j]);
	//		}
	//
	//
	//	}
		return S_OK;
	}




HRESULT ModelBufferPMX::Create(const char* FileName, Model* mpModel){


	//HRESULT hr = S_OK;
	////pmdクラスを生成する。その際にコンストラクタにファイル名を教える
	//std::string sFileName = FileName;
	//pmx modeldata(sFileName.c_str());
	//if (!modeldata.mLoadResult)return S_FALSE;
	//unsigned int loc = sFileName.find_last_of("/");
	//if (loc != std::string::npos && (loc + 1) != std::string::npos){
	//	loc += 1;
	//	sFileName.erase(loc);
	//}
	//
	////頂点データを取得
	//UINT TYOUTEN = modeldata.vert_count;
	//
	//PMDVertex *vertices;
	//vertices = new PMDVertex[TYOUTEN];
	//for (int i = 0; i < TYOUTEN; i++){
	//
	//	if (modeldata.vertex_data.vertex[i].weight_type == pmx::t_vertex_data::BDEF1){
	//		modeldata.vertex_data.vertex[i].bone_weight[0] = 1.0f;
	//	}
	//	if (modeldata.vertex_data.vertex[i].weight_type == pmx::t_vertex_data::BDEF2){
	//		modeldata.vertex_data.vertex[i].bone_weight[1] = 1.0f - modeldata.vertex_data.vertex[i].bone_weight[0];
	//	}
	//	if (modeldata.vertex_data.vertex[i].weight_type == pmx::t_vertex_data::SDEF){
	//		modeldata.vertex_data.vertex[i].bone_weight[1] = 1.0f - modeldata.vertex_data.vertex[i].bone_weight[0];
	//	}
	//
	//
	//	vertices[i].Pos.x = modeldata.vertex_data.vertex[i].v.pos[0];
	//	vertices[i].Pos.y = modeldata.vertex_data.vertex[i].v.pos[1];
	//	vertices[i].Pos.z = modeldata.vertex_data.vertex[i].v.pos[2];
	//	vertices[i].Normal.x = modeldata.vertex_data.vertex[i].v.normal_vec[0];
	//	vertices[i].Normal.y = modeldata.vertex_data.vertex[i].v.normal_vec[1];
	//	vertices[i].Normal.z = modeldata.vertex_data.vertex[i].v.normal_vec[2];
	//	vertices[i].Tex.x = modeldata.vertex_data.vertex[i].v.uv[0];
	//	vertices[i].Tex.y = modeldata.vertex_data.vertex[i].v.uv[1];
	//	vertices[i].BoneIndex[0] = modeldata.vertex_data.vertex[i].bone_num[0];
	//	vertices[i].BoneIndex[1] = modeldata.vertex_data.vertex[i].bone_num[1];
	//	vertices[i].BoneIndex[2] = modeldata.vertex_data.vertex[i].bone_num[2];
	//	vertices[i].BoneIndex[3] = modeldata.vertex_data.vertex[i].bone_num[3];
	//	vertices[i].BoneWeight[0] = (UINT)(modeldata.vertex_data.vertex[i].bone_weight[0] * 100.0f);
	//	vertices[i].BoneWeight[1] = (UINT)(modeldata.vertex_data.vertex[i].bone_weight[1] * 100.0f);
	//	vertices[i].BoneWeight[2] = (UINT)(modeldata.vertex_data.vertex[i].bone_weight[2] * 100.0f);
	//	vertices[i].BoneWeight[3] = (UINT)(modeldata.vertex_data.vertex[i].bone_weight[3] * 100.0f);
	//}
	//hr = createVertex(vertices, sizeof(PMDVertex), TYOUTEN);
	//delete[] vertices;
	//if (FAILED(hr))
	//	return hr;
	//
	//
	////インデックスデータを取得
	//UINT INDEXSU = modeldata.face_vert_count;
	//WORD *indices;
	//indices = new WORD[INDEXSU];
	//
	//int size = modeldata.header.config[pmx::t_header::VIDX];
	//for (UINT i = 0; i < INDEXSU; i++){
	//	if (size == 1){
	//		indices[i] = (WORD)((byte*)modeldata.face_vert_index)[i];
	//	}
	//	if (size == 2){
	//		indices[i] = (WORD)((unsigned short*)modeldata.face_vert_index)[i];
	//	}
	//	if (size == 4){
	//		indices[i] = (WORD)((unsigned int*)modeldata.face_vert_index)[i];
	//	}
	//}
	//hr = createIndex(indices, INDEXSU);
	//delete[] indices;
	//if (FAILED(hr))
	//	return hr;
	//
	//hr = createMaterial(modeldata.material_count, modeldata.material,modeldata.textureName, sFileName, mpModel);
	//if (FAILED(hr))
	//	return hr;
	//
	//
	//mIk = new Ik[modeldata.ik_count];
	//createBone(modeldata.bone_count, modeldata.bone);
	//
	//mStride = sizeof(PMDVertex);
	//
	return S_OK;
}

//  文字列を置換する
//Replace(file, "\\", "/");
//void Replace(std::string& String1, std::string String2, std::string String3)
//{
//	std::string::size_type  Pos(String1.find(String2));
//
//	while (Pos != std::string::npos)
//	{
//		String1.replace(Pos, String2.length(), String3);
//		Pos = String1.find(String2, Pos + String3.length());
//	}
//
//}
HRESULT ModelBufferPMX::createMaterial(unsigned long count, pmx::t_material* material, std::string* textures, const std::string& sFileName, Model* mpModel){
	//Resultマテリアルを消したからメッシュのみで他意味なし
	//このクラス自体もう使われない？
	HRESULT hr = S_OK;

	auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
	auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);

	mpModel->mMeshs.resize(count);
	UINT vertcount = 0;
	for (DWORD i = 0; i < count; i++){
		Material mat;
		mat.mDiffuse = XMFLOAT4(material[i].material.Diffuse);
		mat.mSpecular = XMFLOAT4(material[i].material.Specular[0], material[i].material.Specular[1], material[i].material.Specular[2], material[i].material.Specularity);
		mat.mAmbient = XMFLOAT4(material[i].material.Ambient[0], material[i].material.Ambient[1], material[i].material.Ambient[2], 1.0f);
		mpModel->mMeshs[i].mFace_vert_count = material[i].face_vert_count;
		mpModel->mMeshs[i].mpModelBuffer = this;
		mpModel->mMeshs[i].mFace_vert_start_count = vertcount;
		vertcount += material[i].face_vert_count;
		hr = mat.Create(cbm, cbt);
		if (FAILED(hr))
			return hr;
		if (material[i].ToonMode == 1){
			std::string toonFile = "toon";
			toonFile += std::to_string((int)material[i].ToonIdx + 1);
			toonFile += ".bmp";
			toonFile = sFileName + toonFile;

			mat.SetTexture(toonFile.c_str(), 2);
		}
		else{

			if (material[i].ToonIdx != -1){
				std::string file = sFileName + textures[material[i].ToonIdx];
				mat.SetTexture(file.c_str(), 2);
			}
		}

		if (material[i].SphereMode != 0){
			if (material[i].SphereIdx != -1){
				std::string file = sFileName + textures[material[i].SphereIdx];
				mat.SetTexture(file.c_str(), 1);
			}
		}

		if (material[i].TextureIdx != -1){

			std::string file = sFileName + textures[material[i].TextureIdx];
			mat.SetTexture(file.c_str(), 0);
		}
	}
	return S_OK;
}

HRESULT ModelBufferPMX::createBone(unsigned long count, pmx::t_bone* bone){

	//DWORD ikCount = 0;
	//mBoneNum = count;
	//mBone = new Bone[mBoneNum];
	//for (DWORD i = 0; i < mBoneNum; i++){
	//	mBone[i].mStrName = bone[i].BoneName;// +"%0";
	//	mBone[i].mHierarchy.mIdxSelf = i;
	//	mBone[i].mHierarchy.mIdxParent = bone[i].parent_bidx;
	//	if (bone[i].parent_bidx >= (int)mBoneNum) mBone[i].mHierarchy.mIdxParent = UINT(-1);
	//
	//
	//	XMVECTOR head_pos = XMVectorSet(bone[i].bone_head_pos[0], bone[i].bone_head_pos[1], bone[i].bone_head_pos[2], 0.0f);
	//	XMVECTOR parent_pos = { 0, 0, 0, 1 };
	//	if (mBone[i].mHierarchy.mIdxParent < (int)mBoneNum){
	//		UINT p = mBone[i].mHierarchy.mIdxParent;
	//		parent_pos = XMVectorSet(bone[p].bone_head_pos[0], bone[p].bone_head_pos[1], bone[p].bone_head_pos[2], 0.0f);
	//	}
	//
	//	XMVECTOR local_pos = XMVectorSubtract(head_pos, parent_pos);
	//
	//	mBone[i].mPos = XMFLOAT3(XMVectorGetX(local_pos), XMVectorGetY(local_pos), XMVectorGetZ(local_pos));
	//	mBone[i].mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	//	XMVECTOR q = XMQuaternionIdentity();
	//	mBone[i].mRot = XMFLOAT4(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
	//
	//	//ワールド行列計算
	//	XMVECTOR scale = { 1, 1, 1, 1 };
	//	mBone[i].mMtxPose = SRTMatrix(scale, q, local_pos);
	//	if (mBone[i].mHierarchy.mIdxParent < (int)mBoneNum){
	//		mBone[i].mMtxPose = XMMatrixMultiply(mBone[i].mMtxPose, mBone[mBone[i].mHierarchy.mIdxParent].mMtxPose);
	//	}
	//	if (bone[i].bone_flag & pmx::t_bone::BIT_IK){
	//		mBone[i].mIkBoneIdx = (WORD)bone[i].Ik.idx;
	//
	//		createIk(ikCount,i, bone[i].Ik);
	//		ikCount++;
	//	}
	//	else{
	//		mBone[i].mIkBoneIdx = 0;
	//	}
	//
	//
	//	mBone[i].mMtxPoseInit = mBone[i].mMtxPose;
	//
	//}
	return S_OK;
}

HRESULT ModelBufferPMX::createIk(DWORD ikCount, UINT bidx, pmx::t_ik_data& ik){



	//mIk[ikCount].bone_index = bidx;
	//mIk[ikCount].target_bone_index = ik.idx;
	//mIk[ikCount].chain_length = ik.IKLink.size();
	//mIk[ikCount].iterations = ik.Loop;
	//mIk[ikCount].control_weight = ik.LimitAng;
	//for (auto& ikc : ik.IKLink){
	//	mIk[ikCount].child_bone_index.push_back(ikc.idxBone);
	//}
	return S_OK;
}