

#include <string>

#include "Model.h"


#include "Graphic/Material/Material.h"
#include "Game/Component/MaterialComponent.h"


#include "BoneModel.h"


//強制的にこのマテリアルを使用
//static
Material* Model::mForcedMaterial = NULL;
Material* Model::mForcedMaterialUseTexture = NULL;

Model::Model()
	: mBoneModel(NULL)
{
	mWorld = XMMatrixIdentity();
}
Model::~Model()
{

}

HRESULT Model::Create(const char* FileName){

	AssetDataBase::Instance(FileName, m_MeshAssetDataPtr);


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
	if (mCBuffer.mBuffer)mCBuffer.PSSetConstantBuffers();
}

void Model::Release(){

	if (mBoneModel){
		delete mBoneModel;
		mBoneModel = NULL;
	}
}


void Model::Update(){

	mCBuffer.mParam.mBeforeWorld = mCBuffer.mParam.mWorld;
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
void Model::Draw(const Material& material) const{
	if (!m_MeshAssetDataPtr)return;

	if (mForcedMaterial){
		mForcedMaterial->SetShader((bool)mBoneModel != NULL);
		mForcedMaterial->PSSetShaderResources();
	}
	else if (mForcedMaterialUseTexture){
		mForcedMaterialUseTexture->SetShader((bool)mBoneModel != NULL);
		material.VSSetShaderResources();
		material.PSSetShaderResources();
	}
	else{
		material.SetShader((bool)mBoneModel != NULL);
		material.VSSetShaderResources();
		material.PSSetShaderResources();
	}

	IASet();
	SetConstantBuffer();
	if (mBoneModel){
		mBoneModel->SetConstantBuffer();
	}

	auto& buf = m_MeshAssetDataPtr->GetBufferData();
	auto& mesh = buf.GetMesh();
	for (auto& m : mesh){
		Device::mpImmediateContext->DrawIndexed(m.m_IndexNum, m.m_StartIndex, 0);
	}
}

void Model::Draw(const shared_ptr<MaterialComponent> material) const{
	if (!m_MeshAssetDataPtr)return;
	if (mBoneModel){
		mBoneModel->SetConstantBuffer();
	}

	if (mForcedMaterial){
		mForcedMaterial->SetShader((bool)mBoneModel != NULL);
		mForcedMaterial->VSSetShaderResources();
		mForcedMaterial->PSSetShaderResources();
	}
	else if (mForcedMaterialUseTexture){
		mForcedMaterialUseTexture->SetShader((bool)mBoneModel != NULL);
	}

	IASet();
	SetConstantBuffer();
	auto& buf = m_MeshAssetDataPtr->GetBufferData();
	auto& mesh = buf.GetMesh();
	UINT i = 0;
	for (auto& m : mesh){
		if (mForcedMaterialUseTexture){
			material->GetMaterial(i).VSSetShaderResources();
			material->GetMaterial(i).PSSetShaderResources();
		}
		else if (!mForcedMaterial){
			material->GetMaterial(i).SetShader((bool)mBoneModel != NULL);
			material->GetMaterial(i).VSSetShaderResources();
			material->GetMaterial(i).PSSetShaderResources();
		}
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