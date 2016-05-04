
#include "WorldGrid.h"

#include "Device/DirectX11Device.h"

#include "Graphic/Material/Material.h"

#include "Game/RenderingSystem.h"

const int WorldGrid::mZlineNum = 100;
const int WorldGrid::mXlineNum = 100;

WorldGrid::WorldGrid(){

	create();

	mShader.Create("EngineResource/Line.fx");

	mMaterial.mDiffuse = XMFLOAT4{ 0.2f, 0.2f, 0.2f, 1 };
	mMaterial.Create();

	int xyz = 0;
	mMaterialXYZ[xyz].mDiffuse = XMFLOAT4{ 0.4f, 0.0f, 0.0f, 1 };
	mMaterialXYZ[xyz].Create();
	xyz = 1;
	mMaterialXYZ[xyz].mDiffuse = XMFLOAT4{ 0.0f, 0.0f, 0.4f, 1 };
	mMaterialXYZ[xyz].Create();
	xyz = 2;
	mMaterialXYZ[xyz].mDiffuse = XMFLOAT4{ 0.0f, 0.4f, 0.0f, 1 };
	mMaterialXYZ[xyz].Create();


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
	
	mLineCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	mLineCBuffer.mParam.mWorld = XMMatrixIdentity();
	mLineCBuffer.UpdateSubresource(render->m_Context);

}
WorldGrid::~WorldGrid(){
	mpVertexBuffer->Release();
	mpIndexBuffer->Release();

}


void WorldGrid::Draw(){

	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

	render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	mShader.SetShader(false, render->m_Context);
	mMaterial.PSSetShaderResources(render->m_Context);
	UINT mStride = sizeof(XMFLOAT4);
	UINT offset = 0;
	render->m_Context->IASetVertexBuffers(0, 1, &mpVertexBuffer, &mStride, &offset);
	render->m_Context->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


	mLineCBuffer.VSSetConstantBuffers(render->m_Context);
	mLineCBuffer.PSSetConstantBuffers(render->m_Context);
	render->m_Context->DrawIndexed(mXlineNum, 0, 0);

	render->m_Context->DrawIndexed(mZlineNum, mXlineNum, 0);

	mMaterialXYZ[0].PSSetShaderResources(render->m_Context);
	render->m_Context->DrawIndexed(2, mXlineNum + mZlineNum, 0);
	mMaterialXYZ[1].PSSetShaderResources(render->m_Context);
	render->m_Context->DrawIndexed(2, mXlineNum + mZlineNum + 2, 0);
	mMaterialXYZ[2].PSSetShaderResources(render->m_Context);
	render->m_Context->DrawIndexed(2, mXlineNum + mZlineNum + 4, 0);

	render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void WorldGrid::create(){
	const int _lineNum = mXlineNum + mZlineNum + 6;
	XMFLOAT4 v[_lineNum];
	WORD idx[_lineNum];

	const float GridSize = 1.0f;

	for (WORD i = 0; i < mXlineNum; i += 2){
		float x = (float)(i - mXlineNum / 2)*GridSize;

		float z = GridSize*mZlineNum / 2;
		v[i] = { x, 0.0f, -z, 0.0f };
		v[i + 1] = { x, 0.0f, z, 0.0f };
		if (x == 0)v[i + 1] = { x, 0.0f, 0.0f, 0.0f };

		idx[i] = i;
		idx[i + 1] = i + 1;
	}

	for (WORD i = 0; i < mZlineNum; i += 2){
		float z = (float)(i - mZlineNum / 2)*GridSize;
		float x = GridSize*mXlineNum / 2;
		v[mXlineNum + i] = { -x, 0.0f, z, 0.0f };
		v[mXlineNum + i + 1] = { x, 0.0f, z, 0.0f };
		if (z == 0)v[mXlineNum + i + 1] = { 0.0f, 0.0f, z, 0.0f };

		idx[mXlineNum + i] = (WORD)mXlineNum + i;
		idx[mXlineNum + i + 1] = (WORD)mXlineNum + i + 1;
	}
	float z = GridSize*mZlineNum / 2;
	float x = GridSize*mXlineNum / 2;
	int i = mXlineNum + mZlineNum;
	v[i] = { 0, 0.0f, 0.0f, 0.0f };
	v[i + 1] = { x, 0.0f, 0.0f, 0.0f };
	idx[i] = (WORD)i;
	idx[i + 1] = (WORD)i + 1;

	i += 2;
	v[i] = { 0.0f, 0.0f, 0, 0.0f };
	v[i + 1] = { 0.0f, 0.0f, z, 0.0f };
	idx[i] = (WORD)i;
	idx[i + 1] = (WORD)i + 1;

	i += 2;
	v[i] = { 0.0f, 0, 0.0f, 0.0f };
	v[i + 1] = { 0.0f, z, 0.0f, 0.0f };
	idx[i] = (WORD)i;
	idx[i + 1] = (WORD)i + 1;

	createVertex(v, sizeof(XMFLOAT4), _lineNum);
	createIndex(idx, _lineNum);
}

void WorldGrid::createVertex(void* Vertices, UINT TypeSize, UINT VertexNum){

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = TypeSize * VertexNum;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vertices;
	auto hr = Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &mpVertexBuffer);
	if (FAILED(hr))mpVertexBuffer = NULL;
}

void WorldGrid::createIndex(WORD* Indices, UINT IndexNum){

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * IndexNum;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Indices;
	auto hr = Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &mpIndexBuffer);
	if (FAILED(hr))mpIndexBuffer = NULL;
}
