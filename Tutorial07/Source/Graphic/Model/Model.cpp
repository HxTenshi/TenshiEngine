

#include "Model.h"

#include "ModelBuffer.h"
#include "Graphic/Material/Material.h"
#include <string>
#include "ModelBufferPMD.h"

void Mesh::Draw()const{
	mpModelBuffer->Draw(mFace_vert_count, mFace_vert_start_count);
}


Model::Model()
	: mModelBuffer(NULL)
	, mBoneModel(NULL)
	//: mVMD("anim/test2.vmd")
{
	// Initialize the world matrices
	mWorld = XMMatrixIdentity();
}
Model::~Model()
{

}
#include "Game/Component/Component.h"
HRESULT Model::Create(const char* FileName){
	HRESULT hr = S_OK;

	std::string f = FileName;
	if (f.find(".tesmesh\0") != std::string::npos){
		mModelBuffer = new AssetModelBuffer();
	}
	else if (f.find(".tedmesh\0") != std::string::npos){
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

	hr = mModelBuffer->Create(FileName, this);
	if (FAILED(hr))
		return hr;

	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	if (!mCBuffer.mBuffer)
		return E_FAIL;


	return S_OK;
}


void Model::SetConstantBuffer() const
{
	if (mCBuffer.mBuffer)mCBuffer.VSSetConstantBuffers();
}

void Model::Release(){

	if (mModelBuffer){
		mModelBuffer->Release();
		delete mModelBuffer;
		mModelBuffer = NULL;
	}
	if (mBoneModel){
		mBoneModel->Release();
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
	if(mModelBuffer)mModelBuffer->IASet();
}
void Model::Draw(Material material) const{
	material.SetShader((bool)mBoneModel);
	IASet();
	SetConstantBuffer();
	if (mBoneModel){
		mBoneModel->SetConstantBuffer();
	}

	material.PSSetShaderResources();
	for (UINT i = 0; i < mMeshs.size(); i++){
		mMeshs[i].Draw();
	}
}

void Model::Draw(shared_ptr<MaterialComponent> material) const{
	if (mBoneModel){
		mBoneModel->SetConstantBuffer();
	}
	for (UINT i = 0; i < mMeshs.size(); i++){
		material->GetMaterial(i).SetShader((bool)mBoneModel);
		IASet();
		SetConstantBuffer();
		material->GetMaterial(i).PSSetShaderResources();
		mMeshs[i].Draw();
	}
}

//#include "../../Input/Input.h"

BoneModel::BoneModel()
	: mBoneBuffer(NULL)
	, mIsCreateAnime(false)
{
}

HRESULT BoneModel::Create(const char* FileName){

	AssetLoader loader;
	auto bonedata = loader.LoadAssetBone(FileName);
	if (!bonedata)return E_FAIL;

	mBoneBuffer = new BoneBuffer();

	mBoneBuffer->mBoneDataPtr = bonedata;
	mBoneBuffer->createBone();

	if (mBoneBuffer->mBoneDataPtr->mBoneBuffer.size()){
		mCBBoneMatrix = ConstantBufferArray<cbBoneMatrix>::create(7, mBoneBuffer->mBoneDataPtr->mBoneBuffer.size());
		if (!mCBBoneMatrix.mBuffer)
			return E_FAIL;
	}
	return S_OK;
}
void BoneModel::CreateAnime(vmd& VMD){
	if (mBoneBuffer->mBoneDataPtr->mBoneBuffer.size()){
		mBoneBuffer->VMDMotionCreate(VMD);
		mIsCreateAnime = true;
	}
}



void BoneModel::PlayVMD(float time){
	if (!mBoneBuffer)return;
	if (mBoneBuffer->mMotion.empty())return;
	mBoneBuffer->VMDAnimation(time);
		
	// ボーン差分行列の作成、定数バッファに転送
	for (UINT ib = 0; ib<mBoneBuffer->mBone.size(); ++ib){
		XMVECTOR Determinant;
		XMMATRIX invmtx = XMMatrixInverse(&Determinant, mBoneBuffer->mBone[ib].mMtxPoseInit);
		XMMATRIX mtx = XMMatrixMultiplyTranspose(invmtx, mBoneBuffer->mBone[ib].mMtxPose);
		mCBBoneMatrix.mParam[ib].BoneMatrix[0] = XMFLOAT4(mtx.r[0].x, mtx.r[0].y, mtx.r[0].z, mtx.r[0].w);//mtx.r[0];
		mCBBoneMatrix.mParam[ib].BoneMatrix[1] = XMFLOAT4(mtx.r[1].x, mtx.r[1].y, mtx.r[1].z, mtx.r[1].w);//mtx.r[1];
		mCBBoneMatrix.mParam[ib].BoneMatrix[2] = XMFLOAT4(mtx.r[2].x, mtx.r[2].y, mtx.r[2].z, mtx.r[2].w);//mtx.r[2];
	}

	mCBBoneMatrix.UpdateSubresource();
}


void BoneModel::Release(){

	mBoneBuffer->Release();
	delete mBoneBuffer;

}


UINT BoneModel::GetMaxAnimeTime(){
	return mBoneBuffer ? mBoneBuffer->GetMaxAnimeTime() : 0;
}

void BoneModel::SetConstantBuffer() const{
	if (mCBBoneMatrix.mBuffer)mCBBoneMatrix.VSSetConstantBuffers();
}

#include "Game/Component/Component.h"
class ModelBufferTexture : public ModelBuffer{

	HRESULT Create(const char* FileName, Model* mpModel) override{
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
		hr = mat.Create();
		if (FAILED(hr))
			return hr;

		mStride = sizeof(PMDVertex);

		return S_OK;
	}
};


ModelTexture::ModelTexture(){

}
ModelTexture::~ModelTexture(){

}
HRESULT ModelTexture::Create(const char* FileName){
	HRESULT hr = S_OK;

	mModelBuffer = new ModelBufferTexture();

	hr = mModelBuffer->Create(FileName, this);
	if (FAILED(hr))
		return hr;

	mCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
	if (!mCBuffer.mBuffer)
		return S_FALSE;

	return S_OK;
}