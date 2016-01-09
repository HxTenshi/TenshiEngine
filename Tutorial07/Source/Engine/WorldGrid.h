#pragma once

#include "Graphic/Material/Material.h"
#include "Graphic/Shader/Shader.h"

class WorldGrid{
public:

	WorldGrid();
	~WorldGrid();
	void Draw()const;
private:

	void create();
	void createVertex(void* Vertices, UINT TypeSize, UINT VertexNum);
	void createIndex(WORD* Indices, UINT IndexNum);

	WorldGrid& operator = (const WorldGrid&);


	static const int mXlineNum;
	static const int mZlineNum;

	ID3D11Buffer*	mpVertexBuffer;
	ID3D11Buffer*	mpIndexBuffer;

	Shader mShader;
	Material mMaterial;
	Material mMaterialXYZ[3];
	ConstantBuffer<CBChangesEveryFrame> mLineCBuffer;
};