

#include <string>

#include "Model.h"


#include "Graphic/Material/Material.h"
#include "Game/Component/MaterialComponent.h"


#include "BoneModel.h"


Model::Model()
	: mBoneModel(NULL)
{
	mWorld = XMMatrixIdentity();
}
Model::~Model()
{

}

HRESULT Model::Create(const char* FileName){
	HRESULT hr = S_OK;

	m_MeshAssetDataPtr = AssetDataBase::Instance(FileName);

	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	if (!mCBuffer.mBuffer)
		return E_FAIL;


	return S_OK;
}
HRESULT Model::CreateBoneModel(const char* FileName){
	HRESULT hr = S_OK;


	if (mBoneModel){
		delete mBoneModel;
		mBoneModel = NULL;
	}
	mBoneModel = new BoneModel();
	if(FAILED(mBoneModel->Create(FileName))){
		if (mBoneModel){
			delete mBoneModel;
			mBoneModel = NULL;
		}
	}
	return hr;
}


void Model::SetConstantBuffer() const
{
	if (mCBuffer.mBuffer)mCBuffer.VSSetConstantBuffers();
}

void Model::Release(){

	if (mBoneModel){
		delete mBoneModel;
		mBoneModel = NULL;
	}
}


void Model::Update(){
	//
	// Update variables that change once per frame
	//
	mCBuffer.mParam.mWorld = XMMatrixTranspose(mWorld);

	mCBuffer.UpdateSubresource();

}
void Model::IASet() const{

	auto& buf = m_MeshAssetDataPtr->GetBufferData();

	auto v = buf.GetVertexBuffer();
	auto i = buf.GetIndexBuffer();
	auto& s = buf.GetStride();
	// Set vertex buffer
	UINT offset = 0;
	Device::mpImmediateContext->IASetVertexBuffers(0, 1, &v, &s, &offset);
	
	// Set index buffer
	Device::mpImmediateContext->IASetIndexBuffer(i, DXGI_FORMAT_R16_UINT, 0);
}
void Model::Draw(Material material) const{
	if (!m_MeshAssetDataPtr)return;
	material.SetShader((bool)mBoneModel);
	IASet();
	SetConstantBuffer();
	if (mBoneModel){
		mBoneModel->SetConstantBuffer();
	}

	material.PSSetShaderResources();

	auto& buf = m_MeshAssetDataPtr->GetBufferData();
	auto& mesh = buf.GetMesh();

	for (auto& m : mesh){
		Device::mpImmediateContext->DrawIndexed(m.m_IndexNum, m.m_StartIndex, 0);
	}
}

void Model::Draw(shared_ptr<MaterialComponent> material) const{
	if (!m_MeshAssetDataPtr)return;
	if (mBoneModel){
		mBoneModel->SetConstantBuffer();
	}

	IASet();

	auto& buf = m_MeshAssetDataPtr->GetBufferData();
	auto& mesh = buf.GetMesh();
	UINT i = 0;
	for (auto& m : mesh){
		material->GetMaterial(i).SetShader((bool)mBoneModel);
		SetConstantBuffer();
		material->GetMaterial(i).PSSetShaderResources();
		Device::mpImmediateContext->DrawIndexed(m.m_IndexNum, m.m_StartIndex, 0);
		i++;
	}
}












//#include "Game/Component/Component.h"
//class ModelBufferTexture : public ModelBuffer{
//
//	HRESULT Create(const char* FileName, Model* mpModel) override{
//		HRESULT hr = S_OK;
//		// Create vertex buffer
//		PMDVertex vertices[] =
//		{
//			{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
//			{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
//			{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
//			{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
//		};
//
//		hr = createVertex(vertices, sizeof(PMDVertex), 4);
//		if (FAILED(hr))
//			return hr;
//
//		// Create index buffer
//		WORD indices[] =
//		{
//			3, 1, 0,
//			2, 1, 3,
//		};
//		hr = createIndex(indices, 6);
//		if (FAILED(hr))
//			return hr;
//
//		//mMaterialNum = 1;
//		//mMaterials = new Material[mMaterialNum];
//		mpModel->mMeshs.resize(1);
//		mpModel->mMeshs[0].mpModelBuffer = this;
//		mpModel->mMeshs[0].mFace_vert_start_count = 0;
//		mpModel->mMeshs[0].mFace_vert_count = 6;
//
//		Material mat;
//		mat.SetTexture(FileName, 0);
//		hr = mat.Create();
//		if (FAILED(hr))
//			return hr;
//
//		mStride = sizeof(PMDVertex);
//
//		return S_OK;
//	}
//};
//
//
//ModelTexture::ModelTexture(){
//
//}
//ModelTexture::~ModelTexture(){
//
//}
//HRESULT ModelTexture::Create(const char* FileName){
//	HRESULT hr = S_OK;
//
//	mModelBuffer = new ModelBufferTexture();
//
//	hr = mModelBuffer->Create(FileName, this);
//	if (FAILED(hr))
//		return hr;
//
//	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
//	if (!mCBuffer.mBuffer)
//		return S_FALSE;
//
//	return S_OK;
//}