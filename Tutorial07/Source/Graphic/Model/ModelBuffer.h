#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"

class Material;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};


struct PMDVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
	UINT BoneIndex[4];
	UINT BoneWeight[4];
};

// 階層構造
class Hierarchy
{
public:
	Hierarchy() : mIdxSelf((UINT)-1), mIdxParent((UINT)-1){}
	UINT mIdxSelf;		// ボーンidx
	UINT mIdxParent;	// 親階層idx
};

#include <string>
// ボーン
class Bone
{
public:
	Bone() : mScale(1, 1, 1), mRot(0, 0, 0, 1), mPos(1, 1, 1){}
	std::string mStrName;
	XMFLOAT3 mScale;	// スケールxyz
	XMFLOAT4 mRot;		// 回転（クォータニオン）
	XMFLOAT3 mPos;		// 位置
	XMMATRIX mMtxPose;	//
	XMMATRIX mMtxPoseInit;	//
	WORD mIkBoneIdx;
	Hierarchy mHierarchy;
private:
};

#include <vector>

//IK
struct Ik
{
	UINT bone_index; // IKボーン番号
	UINT target_bone_index; // IKターゲットボーン番号 // IKボーンが最初に接続するボーン
	UINT chain_length; // IKチェーンの長さ(子の数)
	UINT iterations; // 再帰演算回数 // IK値1
	float control_weight; // IKの影響度 // IK値2 // 演算1回あたりの制限角度
	std::vector<UINT> child_bone_index;//[ik_chain_length] IK影響下のボーン番号
};



class Model;
#include "../Loader/Animation/VMD.h"

class KeyFrame{
public:
	KeyFrame(const vmd::VMDKeyFrame& key):mKeyFrame(key){}
	vmd::VMDKeyFrame mKeyFrame;
	bool operator<(const KeyFrame& o){ return mKeyFrame.FrameNo < o.mKeyFrame.FrameNo; }
private:
};

class Motion{
public:
	std::vector<KeyFrame> mKeyFrame;
private:
};


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

	void VMDMotionCreate(vmd& anime);

	// とりあえずアニメーション
	void VMDAnimation(float key_time);
	// とりあえずIK
	void VMDIkAnimation();

	void UpdatePose();

protected:

	HRESULT createVertex(void* Vertices, UINT TypeSize, UINT VertexNum);
	HRESULT createIndex(WORD* Indices, UINT IndexNum);
protected:
	ID3D11Buffer*	mpVertexBuffer;
	ID3D11Buffer*	mpIndexBuffer;

	UINT mIndexNum;

	DWORD mBoneNum;
	Bone* mBone;
	DWORD mIkNum;
	Ik* mIk;

	Motion* mMotion;

	UINT mStride;

	XMVECTOR mMaxVertex;
	XMVECTOR mMinVertex;

	friend Model;
};


#include "Engine/AssetLoader.h"
class AssetModelBuffer : public ModelBuffer {
public:
	HRESULT Create(const char* FileName, Model* mpModel) override;
private:
	friend Model;
};
