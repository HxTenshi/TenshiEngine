
#include "Device/DirectX11Device.h"
#include "MeshBufferData.h"
#include "MeshFileData.h"
#include "Engine/AssetLoader.h"


MeshBufferData::MeshBufferData()
	: m_Stride(0){
}

MeshBufferData::~MeshBufferData(){
}

void MeshBufferData::Create(const MeshFileData* meshfile){
	m_Meshs.clear();
	//共有初期データの準備
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	auto poly = meshfile->GetPolygonsData();

	//バーテックスの作成
	{
		bd.ByteWidth = poly->VertexSize * poly->GetVertexNum();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		InitData.pSysMem = poly->GetVertexPtr();
		ID3D11Buffer* temp;
		Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &temp);
		m_VertexBuffer = temp;
	}
	//インデックスの作成
	{
		bd.ByteWidth = poly->IndexSize * poly->GetIndexNum();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
		InitData.pSysMem = poly->GetIndexPtr();
		ID3D11Buffer* temp;
		Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &temp);
		m_IndexBuffer = temp;
	}
	//メッシュの作成
	{
		m_Meshs.resize(poly->GetMeshNum());
		int i = 0;
		int total = 0;
		for (auto& mesh : m_Meshs){
			auto count = poly->GetMesh(i);

			mesh.m_StartIndex = total;
			mesh.m_IndexNum = count;

			total += count;
			i++;
		}
	}
	// 頂点のサイズ
	{
		m_Stride = poly->VertexSize;
	}

}

auto MeshBufferData::GetVertexBuffer()const->ID3D11Buffer*{
	auto v = m_VertexBuffer.Get();
	return v;
}
auto MeshBufferData::GetIndexBuffer()const->ID3D11Buffer*{
	return m_IndexBuffer.Get();
}
auto MeshBufferData::GetMesh()const->std::add_const<decltype(m_Meshs)>::type&{
	return m_Meshs;
}
auto MeshBufferData::GetStride()const->std::add_const<decltype(m_Stride)>::type&{
	return m_Stride;
}