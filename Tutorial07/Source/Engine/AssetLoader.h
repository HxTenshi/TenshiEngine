#pragma once

#include <string>
#include <xnamath.h>
#include "Graphic/model/ModelBuffer.h"
#include "Graphic/Material/Material.h"
#include "Graphic/model/ModelBufferPMD.h"
#include <allocators>

#include "Graphic/Loader/Model/PMX.h"

#include "Game/Component.h"

enum class PMXFileVer{
	v0x01 = 0x0001//pos normal tex １つづつ
};

class ModelData;
using ModelDataPtr = shared_ptr < ModelData > ;

struct PolygonsData{
	using VertexType = SimpleVertex;
	//using IndexType = unsigned int;
	using IndexType = unsigned short;

	unsigned long VertexNum;
	VertexType *pVertexs;
	unsigned long IndexNum;
	IndexType *pIindices;
};

class ModelData{
public:
	~ModelData(){
		delete[] m_Polygons.pVertexs;
		delete[] m_Polygons.pIindices;
	}

	static
	ModelDataPtr Create(const PolygonsData& m_Polygons){

		ModelData *temp = new ModelData();
		temp->m_Polygons = m_Polygons;
		temp->CreateBuffer();
		return ModelDataPtr(temp);
	}

private:
	ModelData(){}

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
			bd.ByteWidth = sizeof(PolygonsData::VertexType) * m_Polygons.VertexNum;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			InitData.pSysMem = m_Polygons.pVertexs;
			Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
		}
		//インデックスの作成
		{
			bd.ByteWidth = sizeof(PolygonsData::IndexType) * m_Polygons.IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

			InitData.pSysMem = m_Polygons.pIindices;
			Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
		}
	}

public:
	PolygonsData m_Polygons;

	//解放は現在モデルバッファーがやってる
	//今後直す
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pIndexBuffer;
};

class AssetLoader{
public:
	void LoadFile(const std::string& path){

		ExportModel(path);
	}
	ModelDataPtr LoadAsset(const std::string& path){

		return LoadModel(path);
	}


private:
	ModelDataPtr LoadModel(const std::string& path){
		if (path.find(".tesmesh\0") != std::string::npos){
			return LoadTEStaticMesh(path);
		}
		return ModelDataPtr();
	}
	void ExportModel(const std::string& path){
		if (path.find(".pmd\0") != std::string::npos){
		}
		else if (path.find(".pmx\0") != std::string::npos){
			PMX_to_TEStaticMesh(path);
		}
	}

	HRESULT PMX_to_TEStaticMesh(const std::string& path){
		pmx data(path.c_str());

		HRESULT hr = S_OK;
		if (!data.mLoadResult)return S_FALSE;
		std::string name = path;
		unsigned int loc = name.find_last_of("\\");
		if (loc != std::string::npos && (loc + 1) != std::string::npos){
			loc += 1;
			name.erase(0,loc);
		}

		//頂点データを取得
		unsigned long TYOUTEN = data.vert_count;

		PolygonsData::VertexType *vertices;
		vertices = new PolygonsData::VertexType[TYOUTEN];

		for (unsigned long i = 0; i < TYOUTEN; i++){
			vertices[i].Pos.x = data.vertex_data.vertex[i].v.pos[0];
			vertices[i].Pos.y = data.vertex_data.vertex[i].v.pos[1];
			vertices[i].Pos.z = data.vertex_data.vertex[i].v.pos[2];
			vertices[i].Normal.x = data.vertex_data.vertex[i].v.normal_vec[0];
			vertices[i].Normal.y = data.vertex_data.vertex[i].v.normal_vec[1];
			vertices[i].Normal.z = data.vertex_data.vertex[i].v.normal_vec[2];
			vertices[i].Tex.x = data.vertex_data.vertex[i].v.uv[0];
			vertices[i].Tex.y = data.vertex_data.vertex[i].v.uv[1];
		}

		//インデックスデータを取得
		//unsigned long
		int INDEXSU = data.face_vert_count;
		PolygonsData::IndexType *indices;
		indices = new PolygonsData::IndexType[INDEXSU];

		for (int i = 0; i < INDEXSU; i++){
			indices[i] = ((PolygonsData::IndexType*)data.face_vert_index)[i];
		}


		std::string savePath = "Assets/" + name + ".tesmesh";

		FILE *hFP;
		hFP = fopen(savePath.c_str(), "wb");
		if (hFP != 0){
			fwrite(&TYOUTEN, sizeof(unsigned long), 1, hFP);
			fwrite(vertices, sizeof(PolygonsData::VertexType), TYOUTEN, hFP);
			fwrite(&INDEXSU, sizeof(unsigned long), 1, hFP);
			fwrite(indices, sizeof(PolygonsData::IndexType), INDEXSU, hFP);
			fclose(hFP);
		}
		delete[] vertices;
		delete[] indices;



		shared_ptr<MaterialComponent> mc = shared_ptr<MaterialComponent>(new MaterialComponent());
		CreateMaterial(data.material_count, data.material, data.textureName, "Assets/Texture/", NULL, mc);
		mc->SaveAssetResource("Assets/"+name+".txt");

		return hr;
	}

	ModelDataPtr LoadTEStaticMesh(const std::string& path){
		FILE *hFP;
		hFP = fopen(path.c_str(), "rb");

		if (hFP == 0)return ModelDataPtr();

		PolygonsData poly;
		fread(&poly.VertexNum, sizeof(unsigned long), 1, hFP);
		poly.pVertexs = new PolygonsData::VertexType[poly.VertexNum];
		fread(poly.pVertexs, sizeof(PolygonsData::VertexType), poly.VertexNum, hFP);
		fread(&poly.IndexNum, sizeof(unsigned long), 1, hFP);
		poly.pIindices = new PolygonsData::IndexType[poly.IndexNum];
		fread(poly.pIindices, sizeof(PolygonsData::IndexType), poly.IndexNum, hFP);

		fclose(hFP);
		return ModelData::Create(poly);
		
	}

	HRESULT CreateMaterial(unsigned long count, pmx::t_material* material, std::string* textures, const std::string& sFileName, Model* mpModel, shared_ptr<MaterialComponent> resultMaterial){
		HRESULT hr = S_OK;

		auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);

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

			if (resultMaterial)
				resultMaterial->SetMaterial(i, mat);
		}
		return S_OK;
	}

};