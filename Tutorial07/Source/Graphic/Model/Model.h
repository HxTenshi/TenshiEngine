#pragma once

#include "ConstantBuffer/ConstantBuffer.h"
#include "Graphic/Shader/VertexShader.h"
#include "Graphic/Shader/PixelShader.h"

#include "ConstantBuffer/ConstantBufferDif.h"

#include "../Loader/Animation/VMD.h"
#include <vector>

class ModelBuffer;
class BoneBuffer;
class AssetModelBuffer;
class ModelBufferPMD;
class ModelBufferPMX;
class ModelBufferTexture;

class PointLightComponent;

class Mesh{
public:
	void Draw()const;
	ModelBuffer* mpModelBuffer;
	UINT mFace_vert_count;//	面頂点リストのデータ数
	UINT mFace_vert_start_count;//	面頂点リストのデータ開始位置
};
#include "../Material/Material.h"

class MaterialComponent;

class BoneModel{
public:
	BoneModel();
	HRESULT Create(const char* FileName);
	void CreateAnime(vmd& VMD);

	void Release();
	void SetConstantBuffer() const;
	void PlayVMD(float time);

	bool IsCreateAnime(){ return mIsCreateAnime; }

	UINT GetMaxAnimeTime();

private:
	ConstantBufferArray<cbBoneMatrix> mCBBoneMatrix;

	bool mIsCreateAnime;

	BoneBuffer* mBoneBuffer;
};

class Model{
public:
	Model();
	virtual ~Model();
	virtual HRESULT Create(const char* FileName);
	void Release();
	void SetConstantBuffer() const;
	void Update();
	void IASet() const;
	void Draw(Material material) const;
	void Draw(shared_ptr<MaterialComponent> material) const;
	ModelBuffer* GetModelBuffer(){ return mModelBuffer; }

	//クリエイトされているか
	bool IsCreate(){
		return mModelBuffer != NULL;
	}

public:
	XMMATRIX		mWorld;
	//std::vector<Material> mMaterials;

	BoneModel* mBoneModel;
protected:

	ModelBuffer* mModelBuffer;

	std::vector<Mesh> mMeshs;

	ConstantBuffer<CBChangesEveryFrame> mCBuffer;


	friend ModelBuffer;
	friend AssetModelBuffer;
	friend ModelBufferPMD;
	friend ModelBufferPMX;
	friend ModelBufferTexture;

	friend PointLightComponent;
};

class ModelTexture : public Model{
public:
	ModelTexture();
	~ModelTexture();
	HRESULT Create(const char* FileName) override;
};