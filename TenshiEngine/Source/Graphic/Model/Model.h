#pragma once

#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

#include "Engine/AssetDataBase.h"

class PointLightComponent;

#include "../Material/Material.h"
#include "MySTL/Math.h"

class MaterialComponent;

class BoneModel;

#include <vector>
class MeshComponent;

class Model{
public:
	Model();
	~Model();
	HRESULT Create(const char* FileName);
	HRESULT CreateBoneModel(const char* FileName);
	HRESULT Create(MeshAsset& asset);
	HRESULT CreateBoneModel(BoneAsset& asset);
	void Release();
	void SetConstantBuffer(ID3D11DeviceContext* context) const;
	void Update();
	void IASet(ID3D11DeviceContext* context) const;
	void Draw(ID3D11DeviceContext* context, const Material& material) const;
	void Draw(ID3D11DeviceContext* context, const shared_ptr<MaterialComponent> material) const;
	void Draw(ID3D11DeviceContext* context, const std::vector<weak_ptr<MeshComponent>>& mesh) const;

	BoundingSphere GetBoundingSphere();

	int GetMeshNum();
	//クリエイトされているか
	bool IsCreate(){
		return m_MeshAssetDataPtr != NULL;
	}

public:

	//強制的にこのマテリアルを使用
	static Material* mForcedMaterial;
	//static Material* mForcedMaterialUseTexture;
	static ForcedMaterialFilter::Enum mForcedMaterialFilter;

	XMMATRIX		mWorld;

	BoneModel* mBoneModel;
protected:

	MeshAssetDataPtr m_MeshAssetDataPtr;

	ConstantBuffer<CBChangesEveryFrame> mCBuffer;

	friend PointLightComponent;

	mutable bool mIsChangeMatrix;
};

#include "Engine/AssetFile/Mesh/MeshBufferData.h"
class ModelMesh{
public:
	ModelMesh(){}
	~ModelMesh(){}

	void SetMaterialComponent(const weak_ptr<MaterialComponent>& mate){
		m_MaterialComponent = mate;
	}

	void Create(MeshAsset& asset, int id);

	void Draw(ID3D11DeviceContext* context, bool UseAnimetion) const;
private:

	weak_ptr<MaterialComponent> m_MaterialComponent;
	MeshAssetDataPtr m_MeshAssetDataPtr;
	int m_ID;
};

//class ModelTexture : public Model{
//public:
//	ModelTexture();
//	~ModelTexture();
//	HRESULT Create(const char* FileName) override;
//};