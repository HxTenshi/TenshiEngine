
#include <string>

#include "Model.h"


#include "Graphic/Material/Material.h"
#include "Game/Component/MaterialComponent.h"


#include "BoneModel.h"

#include "Engine/AssetFile/Mesh/MeshFileData.h"

#include "Game/RenderingSystem.h"


//強制的にこのマテリアルを使用
//static
Material* Model::mForcedMaterial = NULL;
//Material* Model::mForcedMaterialUseTexture = NULL;
ForcedMaterialFilter::Enum Model::mForcedMaterialFilter = ForcedMaterialFilter::None;


Model::Model()
	: mBoneModel(NULL)
	, mIsChangeMatrix(false)
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
	mCBuffer.mParam.mWorld = XMMatrixTranspose(mWorld);
	mCBuffer.mParam.mBeforeWorld = mCBuffer.mParam.mWorld;


	return S_OK;
}
HRESULT Model::CreateBoneModel(const char* FileName){
	HRESULT hr = S_OK;


	if (mBoneModel){
		delete mBoneModel;
		mBoneModel = NULL;
	}
	mBoneModel = new BoneModel();
	if (FAILED(mBoneModel->Create(FileName))){
		if (mBoneModel){
			delete mBoneModel;
			mBoneModel = NULL;
		}
	}
	return hr;
}
HRESULT Model::Create(MeshAsset& asset){

	m_MeshAssetDataPtr = asset.m_Ptr;

	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	if (!mCBuffer.mBuffer)
		return E_FAIL;
	mCBuffer.mParam.mWorld = XMMatrixTranspose(mWorld);
	mCBuffer.mParam.mBeforeWorld = mCBuffer.mParam.mWorld;


	return S_OK;
}
HRESULT Model::CreateBoneModel(BoneAsset& asset){
	HRESULT hr = S_OK;


	if (mBoneModel){
		delete mBoneModel;
		mBoneModel = NULL;
	}
	mBoneModel = new BoneModel();
	if (FAILED(mBoneModel->Create(asset))){
		if (mBoneModel){
			delete mBoneModel;
			mBoneModel = NULL;
		}
	}
	return hr;
}

int Model::GetMeshNum(){
	auto& buf = m_MeshAssetDataPtr->GetFileData()->GetBufferData();
	return (int)buf.GetMesh().size();
}

void Model::SetConstantBuffer(ID3D11DeviceContext* context) const
{
	if (mCBuffer.mBuffer)mCBuffer.VSSetConstantBuffers(context);
	if (mCBuffer.mBuffer)mCBuffer.PSSetConstantBuffers(context);
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

	mIsChangeMatrix = true;


}
void Model::IASet(ID3D11DeviceContext* context) const{

	auto& buf = m_MeshAssetDataPtr->GetFileData()->GetBufferData();

	auto v = buf.GetVertexBuffer();
	auto i = buf.GetIndexBuffer();
	auto& s = buf.GetStride();
	// Set vertex buffer
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &v, &s, &offset);

	// Set index buffer
	context->IASetIndexBuffer(i, DXGI_FORMAT_R16_UINT, 0);
}
void Model::Draw(ID3D11DeviceContext* context, const Material& material) const{
	if (!m_MeshAssetDataPtr)return;
	if (mBoneModel){
		mBoneModel->SetConstantBuffer(context);
	}

	//if (mForcedMaterial){
	//	mForcedMaterial->SetShader((bool)(mBoneModel != NULL), context);
	//	mForcedMaterial->PSSetShaderResources(context);
	//}
	//else if (mForcedMaterialUseTexture){
	//	mForcedMaterialUseTexture->SetShader((bool)(mBoneModel != NULL), context);
	//	material.VSSetShaderResources(context);
	//	material.PSSetShaderResources(context);
	//}
	//else{
	//	material.SetShader((bool)(mBoneModel != NULL), context);
	//	material.VSSetShaderResources(context,ForcedMaterialFilter::ALL);
	//	material.PSSetShaderResources(context, ForcedMaterialFilter::ALL);
	//}

	material.SetShader((bool)(mBoneModel != NULL), context);
	material.VSSetShaderResources(context, ForcedMaterialFilter::ALL);
	material.PSSetShaderResources(context, ForcedMaterialFilter::ALL);

	if (mForcedMaterial){
		if (mForcedMaterialFilter & ForcedMaterialFilter::Shader_PS){
			mForcedMaterial->SetShader_PS(context);
		}
		if (mForcedMaterialFilter & ForcedMaterialFilter::Shader_VS){
			mForcedMaterial->SetShader_VS((bool)(mBoneModel != NULL), context);
		}

		mForcedMaterial->VSSetShaderResources(context, mForcedMaterialFilter);
		mForcedMaterial->PSSetShaderResources(context, mForcedMaterialFilter);
	}

	if (mIsChangeMatrix){
		mIsChangeMatrix = false;
		mCBuffer.UpdateSubresource(context);
	}
	//mCBuffer.UpdateSubresource(context);

	IASet(context);
	SetConstantBuffer(context);

	auto& buf = m_MeshAssetDataPtr->GetFileData()->GetBufferData();
	auto& mesh = buf.GetMesh();

	for (auto& m : mesh){
		context->DrawIndexed(m.m_IndexNum, m.m_StartIndex, 0);
	}
}

void Model::Draw(ID3D11DeviceContext* context, const shared_ptr<MaterialComponent> material) const{
	if (!m_MeshAssetDataPtr)return;
	if (mBoneModel){
		mBoneModel->SetConstantBuffer(context);
	}

	if (mForcedMaterial){
		if (mForcedMaterialFilter & ForcedMaterialFilter::Shader_PS){
			mForcedMaterial->SetShader_PS(context);
		}
		if (mForcedMaterialFilter & ForcedMaterialFilter::Shader_VS){
			mForcedMaterial->SetShader_VS((bool)(mBoneModel != NULL), context);
		}

		mForcedMaterial->VSSetShaderResources(context, mForcedMaterialFilter);
		mForcedMaterial->PSSetShaderResources(context, mForcedMaterialFilter);
	}

	if (mIsChangeMatrix){
		mIsChangeMatrix = false;
		mCBuffer.UpdateSubresource(context);
	}
	//mCBuffer.UpdateSubresource(context);

	IASet(context);
	SetConstantBuffer(context);
	auto& buf = m_MeshAssetDataPtr->GetFileData()->GetBufferData();
	auto& mesh = buf.GetMesh();
	UINT i = 0;

	auto filter = (ForcedMaterialFilter::Enum)~mForcedMaterialFilter;

	for (const auto& m : mesh){
		{
			if (filter & ForcedMaterialFilter::Shader_PS){
				material->GetMaterial(0).SetShader_PS(context);
			}
			if (filter & ForcedMaterialFilter::Shader_VS){
				material->GetMaterial(0).SetShader_VS((bool)(mBoneModel != NULL), context);
			}
			material->GetMaterial(0).VSSetShaderResources(context,filter);
			material->GetMaterial(0).PSSetShaderResources(context,filter);
		}
		context->DrawIndexed(m.m_IndexNum, m.m_StartIndex, 0);
		i++;
	}
}

#include "Game/Component/MeshComponent.h"
void Model::Draw(ID3D11DeviceContext* context, const std::vector<weak_ptr<MeshComponent>>& mesh) const{
	if (!m_MeshAssetDataPtr)return;
	if (mBoneModel){
		mBoneModel->SetConstantBuffer(context);
	}

	if (mForcedMaterial){
		if (mForcedMaterialFilter & ForcedMaterialFilter::Shader_PS){
			mForcedMaterial->SetShader_PS(context);
		}
		if (mForcedMaterialFilter & ForcedMaterialFilter::Shader_VS){
			mForcedMaterial->SetShader_VS((bool)(mBoneModel != NULL), context);
		}

		mForcedMaterial->VSSetShaderResources(context, mForcedMaterialFilter);
		mForcedMaterial->PSSetShaderResources(context, mForcedMaterialFilter);
	}

	if (mIsChangeMatrix){
		mIsChangeMatrix = false;
		mCBuffer.UpdateSubresource(context);
	}

	IASet(context);
	SetConstantBuffer(context);

	for (const auto& m : mesh){
		if (!m || !m->IsEnabled())continue;
		auto modelmesh = m->GetModelMesh();
		modelmesh->Draw(context, (bool)(mBoneModel != NULL));
	}
}

BoundingSphere Model::GetBoundingSphere()
{
	if (!m_MeshAssetDataPtr)return BoundingSphere();
	return m_MeshAssetDataPtr->GetFileData()->GetBoundingSphere();
}


void ModelMesh::Create(MeshAsset& asset, int id){
	m_MeshAssetDataPtr = asset.m_Ptr;
	m_ID = id;
}

void ModelMesh::Draw(ID3D11DeviceContext* context,bool UseAnimetion) const{
	if (!m_MeshAssetDataPtr)return;
	if (!m_MaterialComponent)return;


	auto& buf = m_MeshAssetDataPtr->GetFileData()->GetBufferData();
	auto& mesh = buf.GetMesh();
	auto& m = mesh.at(m_ID);

	auto filter = (ForcedMaterialFilter::Enum)~Model::mForcedMaterialFilter;
	if (filter & ForcedMaterialFilter::Shader_PS){
		m_MaterialComponent->GetMaterial(0).SetShader_PS(context);
	}
	if (filter & ForcedMaterialFilter::Shader_VS){
		m_MaterialComponent->GetMaterial(0).SetShader_VS(UseAnimetion, context);
	}
	m_MaterialComponent->GetMaterial(0).VSSetShaderResources(context, filter);
	m_MaterialComponent->GetMaterial(0).PSSetShaderResources(context, filter);

	context->DrawIndexed(m.m_IndexNum, m.m_StartIndex, 0);
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