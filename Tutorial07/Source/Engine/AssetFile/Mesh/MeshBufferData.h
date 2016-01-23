#pragma once

#include <vector>

#include "MySTL/Release_self.h"

class MeshFileData;


class MeshBufferData{

public:
	MeshBufferData();
	~MeshBufferData();

	void Create(const MeshFileData* meshfile);

private:
	//コピー禁止
	MeshBufferData(const MeshBufferData&) = delete;
	MeshBufferData& operator=(const MeshBufferData&) = delete;

	//解放は現在モデルバッファーがやってる
	//今後直す
	Release_self<ID3D11Buffer> m_VertexBuffer;	// バッファー
	Release_self<ID3D11Buffer> m_IndexBuffer;	// バッファー

	struct Mesh{
		UINT m_IndexNum;	// 頂点Indexのデータ数
		UINT m_StartIndex;	// 頂点Indexのデータ開始位置
	};

	std::vector<Mesh> m_Meshs;	// メッシュ単位の頂点Indexデータ
	UINT m_Stride;	// 頂点データのサイズ

public:

	auto GetVertexBuffer()const->ID3D11Buffer*;//decltype(m_VertexBuffer.Get());
	auto GetIndexBuffer()const->ID3D11Buffer*;
	auto GetMesh()const->std::add_const<decltype(m_Meshs)>::type&;
	auto GetStride()const->std::add_const<decltype(m_Stride)>::type&;

};