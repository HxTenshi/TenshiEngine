#pragma once

#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

#include "Engine/AssetDataBase.h"

class PointLightComponent;

#include "../Material/Material.h"

class MaterialComponent;

class BoneModel;

class Model{
public:
	Model();
	~Model();
	HRESULT Create(const char* FileName);
	HRESULT CreateBoneModel(const char* FileName);
	void Release();
	void SetConstantBuffer() const;
	void Update();
	void IASet() const;
	void Draw(const Material& material) const;
	void Draw(const shared_ptr<MaterialComponent> material) const;

	//クリエイトされているか
	bool IsCreate(){
		return m_MeshAssetDataPtr != NULL;
	}

public:

	//強制的にこのマテリアルを使用
	static Material* mForcedMaterial;
	static Material* mForcedMaterialUseTexture;

	XMMATRIX		mWorld;

	BoneModel* mBoneModel;
protected:

	MeshAssetDataPtr m_MeshAssetDataPtr;

	ConstantBuffer<CBChangesEveryFrame> mCBuffer;

	friend PointLightComponent;
};

//class ModelTexture : public Model{
//public:
//	ModelTexture();
//	~ModelTexture();
//	HRESULT Create(const char* FileName) override;
//};