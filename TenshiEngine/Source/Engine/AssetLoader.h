#pragma once

#include <string>
#include "XNAMath/xnamath.h"
#include "Graphic/Material/Material.h"
#include <allocators>

#include "Graphic/Loader/Model/PMX.h"

#include "Game/Component/MaterialComponent.h"

#include "Graphic/model/ModelStructs.h"

class ModelData;
using ModelDataPtr = shared_ptr < ModelData > ;

class BoneData;
using BoneDataPtr = shared_ptr < BoneData >;

class IPolygonsData{
public:
	virtual ~IPolygonsData(){}

	unsigned int VertexSize;
	unsigned int IndexSize;
	unsigned int MeshSize;

	virtual const void* GetVertexPtr() const = 0;
	virtual const void* GetIndexPtr() const = 0;
	virtual const void* GetMeshPtr() const = 0;
	virtual int GetVertexNum() const = 0;
	virtual int GetIndexNum() const = 0;
	virtual int GetMeshNum() const = 0;

	virtual XMFLOAT3 GetVertexPos(int i) const = 0;
	virtual unsigned int GetIndex(int i) const = 0;
	virtual int GetMesh(int i) const = 0;
};

template <class Vertex, class Index, class Mesh>
class PolygonsData : public IPolygonsData{
public:
	using VertexType = Vertex;
	//using IndexType = unsigned int;
	using IndexType = Index;
	using MeshType = Mesh;

	PolygonsData(){
		VertexSize = sizeof(VertexType);
		IndexSize = sizeof(IndexType);
		MeshSize = sizeof(MeshType);
	}

	const void* GetVertexPtr() const override{
		return Vertexs.data();
	}
	const void* GetIndexPtr() const override{
		return Indices.data();
	}
	const void* GetMeshPtr() const override{
		return Meshs.data();
	}
	int GetVertexNum() const override{
		return Vertexs.size();
	}
	int GetIndexNum() const override{
		return Indices.size();
	}
	int GetMeshNum() const override{
		return Meshs.size();
	}
	XMFLOAT3 GetVertexPos(int i) const override{
		return Vertexs[i].Pos;
	}
	unsigned int GetIndex(int i) const override{
		return (unsigned int)Indices[i];
	}
	int GetMesh(int i) const override{
		return (int)Meshs[i];
	}


	std::vector<VertexType> Vertexs;
	std::vector<IndexType> Indices;
	std::vector<MeshType> Meshs;
};

class ModelData{
public:
	ModelData(){
		m_Polygons = NULL;
		m_pVertexBuffer = NULL;
		m_pIndexBuffer = NULL;
	}


	~ModelData(){
		if (m_Polygons)delete m_Polygons;
	}

	static
	ModelDataPtr Create(IPolygonsData* m_Polygons){
		auto temp = make_shared<ModelData>();
		temp->m_Polygons = m_Polygons;
		temp->CreateBuffer();
		return temp;
	}

private:

	void CreateBuffer(){
		//共有初期データの準備
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));

		//バーテックスの作成
		{
			bd.ByteWidth = m_Polygons->VertexSize * m_Polygons->GetVertexNum();
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			InitData.pSysMem = m_Polygons->GetVertexPtr();
			Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
		}
		//インデックスの作成
		{
			bd.ByteWidth = m_Polygons->IndexSize * m_Polygons->GetIndexNum();
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

			InitData.pSysMem = m_Polygons->GetIndexPtr();
			Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
		}
	}

public:
	IPolygonsData* m_Polygons;

	//解放は現在モデルバッファーがやってる
	//今後直す
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
};

#include <vector>
class BoneDataIK{
public:
	std::vector<OutputIKBone> mBoneBuffer;
	std::vector<std::vector<IkLink>> mIK_Links;
	std::vector<std::string> mBoneName;
};
class BoneData{
public:
	std::vector<OutputSimpleBone> mBoneBuffer;
	std::vector<std::string> mBoneName;
};
//class AssetData{
//public:
//	enum class AssetType{
//		Model,
//		Bone,
//		Error,
//	};
//
//private:
//	AssetType mAssetType;
//};

class AssetLoader{
public:
	void LoadFile(const std::string& path){

		ExportModel(path);
	}
	ModelDataPtr LoadAsset(const std::string& path){

		return LoadModel(path);
	}
	BoneDataPtr LoadAssetBone(const std::string& path){

		return LoadBone(path);
	}


private:
	ModelDataPtr LoadModel(const std::string& path){
		if (path.find(".tesmesh\0") != std::string::npos){
			return LoadTEStaticMesh(path);
		}
		if (path.find(".tedmesh\0") != std::string::npos){
			return LoadTEDynamicMesh(path);
		}
		return ModelDataPtr();
	}
	BoneDataPtr LoadBone(const std::string& path){
		if (path.find(".tebone\0") != std::string::npos){
			return LoadTEBone(path);
		}
		return BoneDataPtr();
	}
	void ExportModel(const std::string& path){
		if (path.find(".pmd\0") != std::string::npos){
		}
		else if (path.find(".pmx\0") != std::string::npos){
			PMX_to_TEStaticMesh(path);
		}
	}
	//template<class T>
	//void write(T& write, FILE *hFP){
	//	fwrite(&write, sizeof(T), 1, hFP);
	//}
	template<class T>
	void vecfwrite_array(const std::vector<T>& write, FILE *hFP){

		int num = write.size();
		fwrite(&num, sizeof(int), 1, hFP);

		fwrite(write.data(), sizeof(T), write.size(), hFP);
	}
	template<class T>
	void vecfwrite(T& write, FILE *hFP){
		fwrite(&write, sizeof(T), 1, hFP);
	}
	template<>
	void vecfwrite(std::string& write, FILE *hFP){

		int num = write.length();
		fwrite(&num, sizeof(int), 1, hFP);

		fwrite(write.data(), sizeof(const char), num, hFP);
	}

	template<class IndexType>
	void createMesh(pmx& data,std::string& name){

		//auto itype = data.header.config[pmx::t_header::VIDX];

		PolygonsData<SimpleVertex, unsigned short, int> buffer;
		//頂点データを取得
		buffer.Vertexs.resize(data.vert_count);

		int i = 0;

		for (auto& v : buffer.Vertexs){
			v.Pos.x = data.vertex_data.vertex[i].v.pos[0];
			v.Pos.y = data.vertex_data.vertex[i].v.pos[1];
			v.Pos.z = data.vertex_data.vertex[i].v.pos[2];
			v.Normal.x = data.vertex_data.vertex[i].v.normal_vec[0];
			v.Normal.y = data.vertex_data.vertex[i].v.normal_vec[1];
			v.Normal.z = data.vertex_data.vertex[i].v.normal_vec[2];
			v.Tex.x = data.vertex_data.vertex[i].v.uv[0];
			v.Tex.y = data.vertex_data.vertex[i].v.uv[1];
			i++;
		}

		//インデックスデータを取得
		buffer.Indices.resize(data.face_vert_count);

		i = 0;
		for (auto& idx : buffer.Indices){
			idx = (unsigned short)((IndexType*)data.face_vert_index)[i];
			i++;
		}

		//メッシュ取得
		buffer.Meshs.resize(data.material_count);

		i = 0;
		for (auto& m : buffer.Meshs){
			m = data.material[i].face_vert_count;
			i++;
		}


		std::string savePath = "Assets/" + name + ".tesmesh";

		FILE *hFP;
		fopen_s(&hFP, savePath.c_str(), "wb");
		if (hFP != 0){
			vecfwrite_array(buffer.Vertexs, hFP);
			vecfwrite_array(buffer.Indices, hFP);
			vecfwrite_array(buffer.Meshs, hFP);
			fclose(hFP);
		}
	}

	template<class IndexType>
	void createMesh2(pmx& data, std::string& name){

		//auto itype = data.header.config[pmx::t_header::VIDX];

		PolygonsData<SimpleBoneVertex, unsigned short, int> buffer;
		//頂点データを取得
		buffer.Vertexs.resize(data.vert_count);

		int i = 0;

		for (auto& v : buffer.Vertexs){
			v.Pos.x = data.vertex_data.vertex[i].v.pos[0];
			v.Pos.y = data.vertex_data.vertex[i].v.pos[1];
			v.Pos.z = data.vertex_data.vertex[i].v.pos[2];
			v.Normal.x = data.vertex_data.vertex[i].v.normal_vec[0];
			v.Normal.y = data.vertex_data.vertex[i].v.normal_vec[1];
			v.Normal.z = data.vertex_data.vertex[i].v.normal_vec[2];
			v.Tex.x = data.vertex_data.vertex[i].v.uv[0];
			v.Tex.y = data.vertex_data.vertex[i].v.uv[1];

			v.BoneIndex[0] = data.vertex_data.vertex[i].bone_num[0];
			v.BoneIndex[1] = data.vertex_data.vertex[i].bone_num[1];
			v.BoneIndex[2] = data.vertex_data.vertex[i].bone_num[2];
			v.BoneIndex[3] = data.vertex_data.vertex[i].bone_num[3];


			if (data.vertex_data.vertex[i].weight_type == pmx::t_vertex_data::BDEF1){
				data.vertex_data.vertex[i].bone_weight[0] = 1.0f;
			}
			if (data.vertex_data.vertex[i].weight_type == pmx::t_vertex_data::BDEF2){
				data.vertex_data.vertex[i].bone_weight[1] = 1.0f - data.vertex_data.vertex[i].bone_weight[0];
			}
			if (data.vertex_data.vertex[i].weight_type == pmx::t_vertex_data::SDEF){
				data.vertex_data.vertex[i].bone_weight[1] = 1.0f - data.vertex_data.vertex[i].bone_weight[0];
			}
			if (data.vertex_data.vertex[i].weight_type == pmx::t_vertex_data::QDEF){
				//data.vertex_data.vertex[i].bone_weight[1] = 1.0f - data.vertex_data.vertex[i].bone_weight[0];
			}

			v.BoneWeight[0] = (UINT)(data.vertex_data.vertex[i].bone_weight[0] * 100.0f);
			v.BoneWeight[1] = (UINT)(data.vertex_data.vertex[i].bone_weight[1] * 100.0f);
			v.BoneWeight[2] = (UINT)(data.vertex_data.vertex[i].bone_weight[2] * 100.0f);
			v.BoneWeight[3] = (UINT)(data.vertex_data.vertex[i].bone_weight[3] * 100.0f);
			i++;
		}

		//インデックスデータを取得
		buffer.Indices.resize(data.face_vert_count);

		i = 0;
		for (auto& idx : buffer.Indices){
			idx = (unsigned short)((IndexType*)data.face_vert_index)[i];
			i++;
		}

		//メッシュ取得
		buffer.Meshs.resize(data.material_count);

		i = 0;
		for (auto& m : buffer.Meshs){
			m = data.material[i].face_vert_count;
			i++;
		}


		std::string savePath = "Assets/" + name + ".tedmesh";

		FILE *hFP;
		fopen_s(&hFP, savePath.c_str(), "wb");
		if (hFP != 0){
			vecfwrite_array(buffer.Vertexs, hFP);
			vecfwrite_array(buffer.Indices, hFP);
			vecfwrite_array(buffer.Meshs, hFP);
			fclose(hFP);
		}
	}

	void createBone(pmx& data, std::string& name){

		//auto itype = data.header.config[pmx::t_header::VIDX];

		std::vector<OutputSimpleBone> buffer;
		std::vector<std::string> bonename;
		//頂点データを取得
		buffer.resize(data.bone_count);
		bonename.resize(data.bone_count);

		int i = 0;

		for (auto& b : buffer){


			auto& bone = data.bone[i];

			bonename[i] = bone.BoneName;

			b.bone_head_pos[0] = bone.bone_head_pos[0];
			b.bone_head_pos[1] = bone.bone_head_pos[1];
			b.bone_head_pos[2] = bone.bone_head_pos[2];
			b.parent_bidx = bone.parent_bidx;

			i++;
		}




		std::string savePath = "Assets/" + name + ".tebone";

		FILE *hFP;
		fopen_s(&hFP, savePath.c_str(), "wb");
		if (hFP != 0){
			vecfwrite_array(buffer, hFP);
			for (auto& name : bonename){
				vecfwrite(name, hFP);
			}

			fclose(hFP);
		}
	}

	void createBoneIK(pmx& data, std::string& name){

		//auto itype = data.header.config[pmx::t_header::VIDX];

		std::vector<OutputIKBone> buffer;
		std::vector<std::string> bonename;
		std::vector<std::vector<IkLink>> IK_Links;
		//頂点データを取得
		buffer.resize(data.bone_count);
		bonename.resize(data.bone_count);
		IK_Links.resize(data.bone_count);

		int i = 0;

		for (auto& b : buffer){


			auto& bone = data.bone[i];

			bonename[i] = bone.BoneName;

			b.bone_head_pos[0] = bone.bone_head_pos[0];
			b.bone_head_pos[1] = bone.bone_head_pos[1];
			b.bone_head_pos[2] = bone.bone_head_pos[2];
			b.parent_bidx = bone.parent_bidx;
			b.Hierarchy = bone.Hierarchy;
			b.bone_flag = bone.bone_flag;

			b.JointPos[0] = bone.JointPos[0];
			b.JointPos[1] = bone.JointPos[1];
			b.JointPos[2] = bone.JointPos[2];
			b.idxJoint = bone.idxJoint;
			b.idxGive = bone.idxGive;
			b.fGive = bone.fGive;

			b.FixAxis[0] = bone.FixAxis[0];
			b.FixAxis[1] = bone.FixAxis[1];
			b.FixAxis[2] = bone.FixAxis[2];
			b.AxisX[0] = bone.AxisX[0];
			b.AxisX[1] = bone.AxisX[1];
			b.AxisX[2] = bone.AxisX[2];
			b.AxisZ[0] = bone.AxisZ[0];
			b.AxisZ[1] = bone.AxisZ[1];
			b.AxisZ[2] = bone.AxisZ[2];

			b.iExtParent = bone.iExtParent;

			b.t_ik_data_idx = bone.Ik.idx;
			b.t_ik_data_Loop = bone.Ik.Loop;
			b.t_ik_data_LimitAng = bone.Ik.LimitAng;

			int j = 0;
			IK_Links[i].resize(bone.Ik.IKLink.size());
			for (auto& ik : IK_Links[i]){
				auto& t = bone.Ik.IKLink[j];
				ik.bLimit = t.bLimit;
				ik.idxBone = t.idxBone;
				ik.Max[0] = t.Max[0];
				ik.Max[1] = t.Max[1];
				ik.Max[2] = t.Max[2];
				ik.Min[0] = t.Min[0];
				ik.Min[1] = t.Min[1];
				ik.Min[2] = t.Min[2];
			}

			i++;
		}




		std::string savePath = "Assets/" + name + ".tebone";

		FILE *hFP;
		fopen_s(&hFP, savePath.c_str(), "wb");
		if (hFP != 0){
			vecfwrite_array(buffer, hFP);
			for (auto& ik : IK_Links){
				vecfwrite_array(ik, hFP);
			}
			for (auto& name : bonename){
				vecfwrite(name, hFP);
			}

			fclose(hFP);
		}
	}

	HRESULT PMX_to_TEStaticMesh(const std::string& path){
		pmx data(path.c_str());

		HRESULT hr = S_OK;
		if (!data.mLoadResult)return E_FAIL;
		std::string name = path;
		unsigned int loc = name.find_last_of("\\");
		if (loc != std::string::npos && (loc + 1) != std::string::npos){
			loc += 1;
			name.erase(0,loc);
		}

		//メッシュの作成
		{
			auto itype = data.header.config[pmx::t_header::VIDX];
			if (itype == 1){
				createMesh<unsigned char>(data, name);
				createMesh2<unsigned char>(data, name);
			}
			if (itype == 2){
				createMesh<unsigned short>(data, name);
				createMesh2<unsigned short>(data, name);
			}
			if (itype == 4){
				createMesh<unsigned int>(data, name);
				createMesh2<unsigned int>(data, name);
			}
		}

		//ボーン作成
		createBone(data, name);

		//マテリアルアセットの作成
		{
			shared_ptr<MaterialComponent> mc = make_shared<MaterialComponent>();
			CreateMaterial(data.material_count, data.material, data.textureName, "Assets/Texture/", mc);
			mc->SaveAssetResource("Assets/" + name + ".txt");
		}

		return hr;
	}

	template<class T>
	void vecfread_array(std::vector<T>& read, FILE *hFP){

		int num;
		fread(&num, sizeof(int), 1, hFP);
		read.resize(num);
		fread(read.data(), sizeof(T), read.size(), hFP);
	}

	template<class T>
	void vecfread(T& read, FILE *hFP){
		fread(&read, sizeof(T), 1, hFP);
	}
	template<>
	void vecfread(std::string& read, FILE *hFP){

		int num;
		fread(&num, sizeof(int), 1, hFP);

		read.resize(num);

		fread((void*)read.data(), sizeof(const char), num, hFP);
		read += "\0";
	}
	ModelDataPtr LoadTEStaticMesh(const std::string& path){
		FILE *hFP;
		fopen_s(&hFP,path.c_str(), "rb");

		if (hFP == 0)return ModelDataPtr();

		PolygonsData<SimpleVertex, unsigned short, int>* poly = new PolygonsData<SimpleVertex, unsigned short, int>();
		vecfread_array(poly->Vertexs, hFP);
		vecfread_array(poly->Indices, hFP);
		vecfread_array(poly->Meshs, hFP);

		fclose(hFP);
		return ModelData::Create(poly);
		
	}
	ModelDataPtr LoadTEDynamicMesh(const std::string& path){
		FILE *hFP;
		fopen_s(&hFP, path.c_str(), "rb");

		if (hFP == 0)return ModelDataPtr();

		PolygonsData<SimpleBoneVertex, unsigned short, int>* poly = new PolygonsData<SimpleBoneVertex, unsigned short, int>();
		vecfread_array(poly->Vertexs, hFP);
		vecfread_array(poly->Indices, hFP);
		vecfread_array(poly->Meshs, hFP);

		fclose(hFP);
		return ModelData::Create(poly);

	}

	BoneDataPtr LoadTEBone(const std::string& path){
		FILE *hFP;
		fopen_s(&hFP, path.c_str(), "rb");

		if (hFP == 0)return BoneDataPtr();

		BoneDataPtr data = make_shared<BoneData>();
		vecfread_array(data->mBoneBuffer, hFP);
		//data->mIK_Links.resize(data->mBoneBuffer.size());
		//for (auto& ik : data->mIK_Links){
		//	vecfread_array(ik, hFP);
		//}
		data->mBoneName.resize(data->mBoneBuffer.size());
		for (auto& name : data->mBoneName){
			vecfread(name, hFP);
		}

		fclose(hFP);
		return data;

	}

	HRESULT CreateMaterial(unsigned long count, pmx::t_material* material, std::string* textures, const std::string& sFileName, shared_ptr<MaterialComponent> resultMaterial){
		HRESULT hr = S_OK;

		//mpModel->mMeshs.resize(count);
		//UINT vertcount = 0;
		for (DWORD i = 0; i < count; i++){
			Material mat;
			mat.mDiffuse = XMFLOAT4(material[i].material.Diffuse);
			mat.mSpecular = XMFLOAT4(material[i].material.Specular[0], material[i].material.Specular[1], material[i].material.Specular[2], material[i].material.Specularity);
			mat.mAmbient = XMFLOAT4(material[i].material.Ambient[0], material[i].material.Ambient[1], material[i].material.Ambient[2], 1.0f);
			//mpModel->mMeshs[i].mFace_vert_count = material[i].face_vert_count;
			//mpModel->mMeshs[i].mpModelBuffer = this;
			//mpModel->mMeshs[i].mFace_vert_start_count = vertcount;
			//vertcount += material[i].face_vert_count;
			hr = mat.Create();
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

			if (resultMaterial)
				resultMaterial->SetMaterial(i, mat);
		}
		return S_OK;
	}

};