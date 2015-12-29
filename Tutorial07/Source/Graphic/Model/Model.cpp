

#include "Model.h"

#include "ModelBuffer.h"
#include "Graphic/Material/Material.h"
#include <string>
#include "ModelBufferPMD.h"

void Mesh::Draw()const{
	mpModelBuffer->Draw(mFace_vert_count, mFace_vert_start_count);
}


Model::Model()
	:mVMD("anim/好き雪本気マジック_モーション/好き雪本気マジック_Lat式.vmd")
	, mModelBuffer(NULL)
	//: mVMD("anim/test2.vmd")
{
	// Initialize the world matrices
	mWorld = XMMatrixIdentity();
}
Model::~Model()
{

}
#include "Game/Component.h"
HRESULT Model::Create(const char* FileName,shared_ptr<MaterialComponent> resultMaterial){
	HRESULT hr = S_OK;

	std::string f = FileName;
	if (f.find(".tesmesh\0") != std::string::npos){
		mModelBuffer = new AssetModelBuffer();
	}
	else if(f.find(".pmd\0") != std::string::npos){
		mModelBuffer = new ModelBufferPMD();
	}
	else if (f.find(".pmx\0") != std::string::npos){
		mModelBuffer = new ModelBufferPMX();
	}
	else{
		mModelBuffer = new ModelBuffer();
	}

	hr = mModelBuffer->Create(FileName, this, resultMaterial);
	if (FAILED(hr))
		return hr;

	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	if (!mCBuffer.mBuffer)
		return S_FALSE;

	if (mModelBuffer->mBoneNum){
		mCBBoneMatrix = ConstantBufferArray<cbBoneMatrix>::create(7, mModelBuffer->mBoneNum);
		if (!mCBBoneMatrix.mBuffer)
			return S_FALSE;

		mModelBuffer->VMDMotionCreate(mVMD);
	}


	return S_OK;
}


void Model::SetConstantBuffer() const
{
	if (mCBuffer.mBuffer)mCBuffer.VSSetConstantBuffers();
	if (mCBBoneMatrix.mBuffer)mCBBoneMatrix.VSSetConstantBuffers();
}

void Model::Release(){

	if (mModelBuffer){
		mModelBuffer->Release();
		delete mModelBuffer;
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
	mModelBuffer->IASet();
}
void Model::Draw(Material material) const{
	material.SetShader();
	IASet();
	SetConstantBuffer();
	material.PSSetShaderResources();
	for (UINT i = 0; i < mMeshs.size(); i++){
		mMeshs[i].Draw();
	}
}

void Model::Draw(shared_ptr<MaterialComponent> material) const{
	for (UINT i = 0; i < mMeshs.size(); i++){
		material->GetMaterial(i).SetShader();
		IASet();
		SetConstantBuffer();
		material->GetMaterial(i).PSSetShaderResources();
		mMeshs[i].Draw();
	}
}

//#include "../../Input/Input.h"

void Model::PlayVMD(float time){
	if (!mModelBuffer->mMotion)return;
	mModelBuffer->VMDAnimation(time);
		
	// ボーン差分行列の作成、定数バッファに転送
	for (UINT ib = 0; ib<mModelBuffer->mBoneNum; ++ib){
		XMVECTOR Determinant;
		XMMATRIX invmtx = XMMatrixInverse(&Determinant, mModelBuffer->mBone[ib].mMtxPoseInit);
		XMMATRIX mtx = XMMatrixMultiplyTranspose(invmtx, mModelBuffer->mBone[ib].mMtxPose);
		mCBBoneMatrix.mParam[ib].BoneMatrix[0] = XMFLOAT4(mtx.r[0].x, mtx.r[0].y, mtx.r[0].z, mtx.r[0].w);//mtx.r[0];
		mCBBoneMatrix.mParam[ib].BoneMatrix[1] = XMFLOAT4(mtx.r[1].x, mtx.r[1].y, mtx.r[1].z, mtx.r[1].w);//mtx.r[1];
		mCBBoneMatrix.mParam[ib].BoneMatrix[2] = XMFLOAT4(mtx.r[2].x, mtx.r[2].y, mtx.r[2].z, mtx.r[2].w);//mtx.r[2];
	}

	mCBBoneMatrix.UpdateSubresource();
}



bool Model::CheckHitPoint(const XMVECTOR& point){

	
	XMVECTOR _max = XMVector3Transform(mModelBuffer->mMaxVertex, mWorld);
	XMVECTOR _min = XMVector3Transform(mModelBuffer->mMinVertex, mWorld);

	XMVECTOR max;
	XMVECTOR min;
	max.x = max(_max.x, _min.x);
	max.y = max(_max.y, _min.y);
	max.z = max(_max.z, _min.z);
	min.x = min(_max.x, _min.x);
	min.y = min(_max.y, _min.y);
	min.z = min(_max.z, _min.z);

	if (max.x < point.x)return false;
	if (max.y < point.y)return false;
	if (max.z < point.z)return false;
	if (min.x > point.x)return false;
	if (min.y > point.y)return false;
	if (min.z > point.z)return false;

	return true;
}

#include "Game/Component.h"
class ModelBufferTexture : public ModelBuffer{

	HRESULT Create(const char* FileName, Model* mpModel, shared_ptr<MaterialComponent> resultMaterial) override{
		HRESULT hr = S_OK;
		// Create vertex buffer
		PMDVertex vertices[] =
		{
			{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
			{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
			{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
			{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f), { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
		};

		hr = createVertex(vertices, sizeof(PMDVertex), 4);
		if (FAILED(hr))
			return hr;

		// Create index buffer
		WORD indices[] =
		{
			3, 1, 0,
			2, 1, 3,
		};
		hr = createIndex(indices, 6);
		if (FAILED(hr))
			return hr;

		//mMaterialNum = 1;
		//mMaterials = new Material[mMaterialNum];
		mpModel->mMeshs.resize(1);
		mpModel->mMeshs[0].mpModelBuffer = this;
		mpModel->mMeshs[0].mFace_vert_start_count = 0;
		mpModel->mMeshs[0].mFace_vert_count = 6;

		Material mat;
		mat.SetTexture(FileName, 0);
		auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);
		hr = mat.Create(cbm, cbt);
		if (FAILED(hr))
			return hr;

		if (resultMaterial)
			resultMaterial->SetMaterial(0, mat);

		mStride = sizeof(PMDVertex);

		return S_OK;
	}
};


ModelTexture::ModelTexture(){

}
ModelTexture::~ModelTexture(){

}
HRESULT ModelTexture::Create(const char* FileName, shared_ptr<MaterialComponent> resultMaterial){
	HRESULT hr = S_OK;

	mModelBuffer = new ModelBufferTexture();

	hr = mModelBuffer->Create(FileName, this, resultMaterial);
	if (FAILED(hr))
		return hr;

	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	if (!mCBuffer.mBuffer)
		return S_FALSE;

	return S_OK;
}