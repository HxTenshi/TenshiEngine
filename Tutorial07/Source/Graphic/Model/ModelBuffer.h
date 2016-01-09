#pragma once

#include "Engine/AssetLoader.h"

class Material;
class BoneModel;
class Model;

#include "MySTL/ptr.h"
class MaterialComponent;
class ModelBuffer{
public:
	ModelBuffer();
	virtual ~ModelBuffer();
	virtual HRESULT Create(const char* FileName, Model* mpModel);
	void IASet() const;
	void Draw(UINT IndexNum, UINT StartIndex) const;
	void Release();

protected:

	HRESULT createVertex(void* Vertices, UINT TypeSize, UINT VertexNum);
	HRESULT createIndex(WORD* Indices, UINT IndexNum);
protected:
	ID3D11Buffer*	mpVertexBuffer;
	ID3D11Buffer*	mpIndexBuffer;

	UINT mIndexNum;

	UINT mStride;

	XMVECTOR mMaxVertex;
	XMVECTOR mMinVertex;

	friend Model;
};

class BoneBuffer{
public:
	BoneBuffer();

	void Release();

	void VMDMotionCreate(vmd& anime);

	// とりあえずアニメーション
	void VMDAnimation(float key_time);
	// とりあえずIK
	void VMDIkAnimation();

	void UpdatePose();

	void createBone();
	void createIk(DWORD ikCount, UINT bidx);

	UINT GetMaxAnimeTime(){
		return mLastKeyNo;
	}

	BoneDataPtr mBoneDataPtr;

private:

	std::vector<Motion> mMotion;

	std::vector<Bone> mBone;
	std::vector<Ik> mIk;

	UINT mLastKeyNo;

	friend BoneModel;
};


class AssetModelBuffer : public ModelBuffer {
public:
	HRESULT Create(const char* FileName, Model* mpModel) override;
private:
	friend Model;
};
