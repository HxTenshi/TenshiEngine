
#include "MeshFileData.h"

#include "Engine/AssetLoader.h"

#include "MySTL/Utility.h"


MeshFileData::MeshFileData(){
	m_Polygons = NULL;
}

MeshFileData::~MeshFileData(){
	if (m_Polygons){
		delete m_Polygons;
		m_Polygons = NULL;
	}
}

// 3頂点とUV値から指定座標でのU軸（Tangent）及びV軸（Binormal）を算出
//
// p0, p1, p2    : ローカル空間での頂点座標（ポリゴン描画順にすること）
// uv0, uv1, uv2 : 各頂点のUV座標
// outTangent    : U軸（Tangent）出力
// outBinormal   : V軸（Binormal）出力

void CalcTangentAndBinormal(
	SimpleVertexNormal& p0,
	SimpleVertexNormal& p1,
	SimpleVertexNormal& p2
	) {
	// 5次元→3次元頂点に
	XMVECTOR CP0[3] = {
		XMVectorSet(p0.Pos.x, p0.Tex.x, p0.Tex.y,1),
		XMVectorSet(p0.Pos.y, p0.Tex.x, p0.Tex.y,1),
		XMVectorSet(p0.Pos.z, p0.Tex.x, p0.Tex.y,1),
	};
	XMVECTOR CP1[3] = {
		XMVectorSet(p1.Pos.x, p1.Tex.x, p1.Tex.y, 1),
		XMVectorSet(p1.Pos.y, p1.Tex.x, p1.Tex.y, 1),
		XMVectorSet(p1.Pos.z, p1.Tex.x, p1.Tex.y, 1),
	};
	XMVECTOR CP2[3] = {
		XMVectorSet(p2.Pos.x, p2.Tex.x, p2.Tex.y, 1),
		XMVectorSet(p2.Pos.y, p2.Tex.x, p2.Tex.y, 1),
		XMVectorSet(p2.Pos.z, p2.Tex.x, p2.Tex.y, 1),
	};

	// 平面パラメータからUV軸座標算出
	float U[3], V[3];
	for (int i = 0; i < 3; ++i) {
		XMVECTOR V1 = CP1[i] - CP0[i];
		XMVECTOR V2 = CP2[i] - CP1[i];
		XMVECTOR ABC;
		ABC = XMVector3Cross(V1, V2);

		if (ABC.x == 0.0f) {
			// やばいす！
			// ポリゴンかUV上のポリゴンが縮退してます！
			//_ASSERT(0);
			//memset(outTangent, 0, sizeof(D3DXVECTOR3));
			//memset(outBinormal, 0, sizeof(D3DXVECTOR3));
			p0.Binormal = XMFLOAT3(1, 0, 0);
			p1.Binormal = XMFLOAT3(1, 0, 0);
			p2.Binormal = XMFLOAT3(1, 0, 0);
			p0.Tangent = XMFLOAT3(0,1,0);
			p1.Tangent = XMFLOAT3(0,1,0);
			p2.Tangent = XMFLOAT3(0,1,0);
			return;
		}
		U[i] = -ABC.y / ABC.x;
		V[i] = -ABC.z / ABC.x;
	}


	XMVECTOR tan = XMVectorSet(U[0], U[1], U[2], 1);
	XMVECTOR bin = -XMVectorSet(V[0], V[1], V[2], 1);

	// 正規化します
	tan = XMVector3Normalize(tan);
	bin = XMVector3Normalize(bin);

	p0.Binormal = XMFLOAT3(bin.x, bin.y, bin.z);
	p1.Binormal = XMFLOAT3(bin.x, bin.y, bin.z);
	p2.Binormal = XMFLOAT3(bin.x, bin.y, bin.z);
	p0.Tangent = XMFLOAT3(tan.x, tan.y, tan.z);
	p1.Tangent = XMFLOAT3(tan.x, tan.y, tan.z);
	p2.Tangent = XMFLOAT3(tan.x, tan.y, tan.z);
}
void CalcTangentAndBinormal(
	SimpleBoneVertexNormal& p0,
	SimpleBoneVertexNormal& p1,
	SimpleBoneVertexNormal& p2
	) {
	// 5次元→3次元頂点に
	XMVECTOR CP0[3] = {
		XMVectorSet(p0.Pos.x, p0.Tex.x, p0.Tex.y, 1),
		XMVectorSet(p0.Pos.y, p0.Tex.x, p0.Tex.y, 1),
		XMVectorSet(p0.Pos.z, p0.Tex.x, p0.Tex.y, 1),
	};
	XMVECTOR CP1[3] = {
		XMVectorSet(p1.Pos.x, p1.Tex.x, p1.Tex.y, 1),
		XMVectorSet(p1.Pos.y, p1.Tex.x, p1.Tex.y, 1),
		XMVectorSet(p1.Pos.z, p1.Tex.x, p1.Tex.y, 1),
	};
	XMVECTOR CP2[3] = {
		XMVectorSet(p2.Pos.x, p2.Tex.x, p2.Tex.y, 1),
		XMVectorSet(p2.Pos.y, p2.Tex.x, p2.Tex.y, 1),
		XMVectorSet(p2.Pos.z, p2.Tex.x, p2.Tex.y, 1),
	};

	// 平面パラメータからUV軸座標算出
	float U[3], V[3];
	for (int i = 0; i < 3; ++i) {
		XMVECTOR V1 = CP1[i] - CP0[i];
		XMVECTOR V2 = CP2[i] - CP1[i];
		XMVECTOR ABC;
		ABC = XMVector3Cross(V1, V2);

		if (ABC.x == 0.0f) {
			// やばいす！
			// ポリゴンかUV上のポリゴンが縮退してます！
			//_ASSERT(0);
			//memset(outTangent, 0, sizeof(D3DXVECTOR3));
			//memset(outBinormal, 0, sizeof(D3DXVECTOR3));
			p0.Binormal = XMFLOAT3(1, 0, 0);
			p1.Binormal = XMFLOAT3(1, 0, 0);
			p2.Binormal = XMFLOAT3(1, 0, 0);
			p0.Tangent = XMFLOAT3(0, 1, 0);
			p1.Tangent = XMFLOAT3(0, 1, 0);
			p2.Tangent = XMFLOAT3(0, 1, 0);
			return;
		}
		U[i] = -ABC.y / ABC.x;
		V[i] = -ABC.z / ABC.x;
	}


	XMVECTOR tan = XMVectorSet(U[0], U[1], U[2], 1);
	XMVECTOR bin = -XMVectorSet(V[0], V[1], V[2], 1);

	// 正規化します
	tan = XMVector3Normalize(tan);
	bin = XMVector3Normalize(bin);

	p0.Binormal = XMFLOAT3(bin.x, bin.y, bin.z);
	p1.Binormal = XMFLOAT3(bin.x, bin.y, bin.z);
	p2.Binormal = XMFLOAT3(bin.x, bin.y, bin.z);
	p0.Tangent = XMFLOAT3(tan.x, tan.y, tan.z);
	p1.Tangent = XMFLOAT3(tan.x, tan.y, tan.z);
	p2.Tangent = XMFLOAT3(tan.x, tan.y, tan.z);
}


bool MeshFileData::Create(const char* filename){
	m_FileName = filename;

	FILE *hFP;
	fopen_s(&hFP, filename, "rb");

	if (hFP == 0)return false;

	if (m_Polygons){
		delete m_Polygons;
		m_Polygons = NULL;
	}

	std::string str = filename;
	auto type = behind_than_find_last_of(str, ".");

	IPolygonsData* _data = NULL;
	if (type == "tesmesh"){
		auto buf = new PolygonsData<SimpleVertex, unsigned short, int>();
		fstd::r_vector(buf->Vertexs, hFP);
		fstd::r_vector(buf->Indices, hFP);
		fstd::r_vector(buf->Meshs, hFP);

		auto data = new PolygonsData<SimpleVertexNormal, unsigned short, int>();

		data->Indices = std::move(buf->Indices);
		data->Meshs = std::move(buf->Meshs);

		auto vnum = buf->Vertexs.size();
		//data->Vertexs.resize(vnum);


		auto num = data->Indices.size();
		data->Vertexs.resize(num);
		for (unsigned int i = 0; i < num; i+=3){

			int i0 = data->Indices[i + 0];
			int i1 = data->Indices[i + 1];
			int i2 = data->Indices[i + 2];

			int o0 = data->Indices[i + 0] = i + 0;
			int o1 = data->Indices[i + 1] = i + 1;
			int o2 = data->Indices[i + 2] = i + 2;

			data->Vertexs[o0].Pos = buf->Vertexs[i0].Pos;
			data->Vertexs[o1].Pos = buf->Vertexs[i1].Pos;
			data->Vertexs[o2].Pos = buf->Vertexs[i2].Pos;
			data->Vertexs[o0].Normal = buf->Vertexs[i0].Normal;
			data->Vertexs[o1].Normal = buf->Vertexs[i1].Normal;
			data->Vertexs[o2].Normal = buf->Vertexs[i2].Normal;
			data->Vertexs[o0].Tex = buf->Vertexs[i0].Tex;
			data->Vertexs[o1].Tex = buf->Vertexs[i1].Tex;
			data->Vertexs[o2].Tex = buf->Vertexs[i2].Tex;


			auto& p0 = data->Vertexs[o0];
			auto& p1 = data->Vertexs[o1];
			auto& p2 = data->Vertexs[o2];

			CalcTangentAndBinormal(p0, p1, p2);
		}

		delete buf;
		_data = data;
	}else if (type == "tedmesh"){
		auto buf = new PolygonsData<SimpleBoneVertex, unsigned short, int>();
		fstd::r_vector(buf->Vertexs, hFP);
		fstd::r_vector(buf->Indices, hFP);
		fstd::r_vector(buf->Meshs, hFP);


		auto data = new PolygonsData<SimpleBoneVertexNormal, unsigned short, int>();

		data->Indices = std::move(buf->Indices);
		data->Meshs = std::move(buf->Meshs);

		auto vnum = buf->Vertexs.size();
		//data->Vertexs.resize(vnum);


		auto num = data->Indices.size();
		data->Vertexs.resize(num);
		for (int i = 0; i < num; i += 3){

			int i0 = data->Indices[i + 0];
			int i1 = data->Indices[i + 1];
			int i2 = data->Indices[i + 2];

			int o0 = data->Indices[i + 0] = i + 0;
			int o1 = data->Indices[i + 1] = i + 1;
			int o2 = data->Indices[i + 2] = i + 2;

			data->Vertexs[o0].Pos = buf->Vertexs[i0].Pos;
			data->Vertexs[o1].Pos = buf->Vertexs[i1].Pos;
			data->Vertexs[o2].Pos = buf->Vertexs[i2].Pos;
			data->Vertexs[o0].Normal = buf->Vertexs[i0].Normal;
			data->Vertexs[o1].Normal = buf->Vertexs[i1].Normal;
			data->Vertexs[o2].Normal = buf->Vertexs[i2].Normal;
			data->Vertexs[o0].Tex = buf->Vertexs[i0].Tex;
			data->Vertexs[o1].Tex = buf->Vertexs[i1].Tex;
			data->Vertexs[o2].Tex = buf->Vertexs[i2].Tex;


			for (int j = 0; j < 4; j++){
				data->Vertexs[o0].BoneWeight[j] = buf->Vertexs[i0].BoneWeight[j];
				data->Vertexs[o1].BoneWeight[j] = buf->Vertexs[i1].BoneWeight[j];
				data->Vertexs[o2].BoneWeight[j] = buf->Vertexs[i2].BoneWeight[j];
				data->Vertexs[o0].BoneIndex[j] = buf->Vertexs[i0].BoneIndex[j];
				data->Vertexs[o1].BoneIndex[j] = buf->Vertexs[i1].BoneIndex[j];
				data->Vertexs[o2].BoneIndex[j] = buf->Vertexs[i2].BoneIndex[j];
			}


			auto& p0 = data->Vertexs[o0];
			auto& p1 = data->Vertexs[o1];
			auto& p2 = data->Vertexs[o2];

			CalcTangentAndBinormal(p0, p1, p2);
		}

		delete buf;
		_data = data;

	}
	else if (type == "tedmesh2"){
		auto buf = new PolygonsData<SimpleBoneVertexNormal, unsigned short, int>();
		fstd::r_vector(buf->Vertexs, hFP);
		fstd::r_vector(buf->Indices, hFP);
		fstd::r_vector(buf->Meshs, hFP);

		_data = buf;

	}

	m_Polygons = _data;

	fclose(hFP);

	m_MeshBufferData.Create(this);

	return true;
}

bool MeshFileData::FileUpdate(){
	return Create(m_FileName.c_str());
}

const IPolygonsData* MeshFileData::GetPolygonsData() const{
	return m_Polygons;
}


const MeshBufferData& MeshFileData::GetBufferData() const{
	return m_MeshBufferData;
}