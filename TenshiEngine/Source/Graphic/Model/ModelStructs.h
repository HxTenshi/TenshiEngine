#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include "XNAMath/xnamath.h"

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};
struct SimpleVertexNormal
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT3 Binormal;
	XMFLOAT3 Tangent;
	XMFLOAT2 Tex;
};
struct SimpleBoneVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
	UINT BoneIndex[4];
	UINT BoneWeight[4];
};
struct SimpleBoneVertexNormal
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT3 Binormal;
	XMFLOAT3 Tangent;
	XMFLOAT2 Tex;
	UINT BoneIndex[4];
	float BoneWeight[4];
};


struct IkLink
{
	int idxBone;
	unsigned char bLimit;
	float Min[3];//bLimit=1
	float Max[3];//bLimit=1
};
struct OutputSimpleBone{
	float bone_head_pos[3]; // x, y, z // ボーンのヘッドの位置
	int parent_bidx;	//ボーンIndexサイズ 親ボーン番号(ない場合は0xFFFF)
};
struct OutputIKBone{
	float bone_head_pos[3]; // x, y, z // ボーンのヘッドの位置
	int parent_bidx;	//ボーンIndexサイズ 親ボーン番号(ない場合は0xFFFF)
	int Hierarchy;//変形階層
	unsigned short bone_flag;		// ボーンの種類

	float JointPos[3];	//BIT_JOINT=0
	int idxJoint;		//BIT_JOINT=1
	int idxGive;		//BIT_GIVE_ROT=1 || BIT_GIVE_MOVE=1
	float fGive;		//BIT_GIVE_ROT=1 || BIT_GIVE_MOVE=1
	float FixAxis[3];	//BIT_FIX_AXIS=1
	float AxisX[3];	//BIT_LOCAL_AXIS=1
	float AxisZ[3];	//BIT_LOCAL_AXIS=1
	int iExtParent;	//BIT_EXT_PARENT=1

	int t_ik_data_idx;// ボーンIndexサイズ | IKターゲットボーンのボーンIndex;
	int t_ik_data_Loop;
	float t_ik_data_LimitAng;
};
struct SimpleBone{
	float bone_head_pos[3]; // x, y, z // ボーンのヘッドの位置
	int parent_bidx;	//ボーンIndexサイズ 親ボーン番号(ない場合は0xFFFF)
	int Hierarchy;//変形階層
	unsigned short bone_flag;		// ボーンの種類

	float JointPos[3];	//BIT_JOINT=0
	int idxJoint;		//BIT_JOINT=1
	int idxGive;		//BIT_GIVE_ROT=1 || BIT_GIVE_MOVE=1
	float fGive;		//BIT_GIVE_ROT=1 || BIT_GIVE_MOVE=1
	float FixAxis[3];	//BIT_FIX_AXIS=1
	float AxisX[3];	//BIT_LOCAL_AXIS=1
	float AxisZ[3];	//BIT_LOCAL_AXIS=1
	int iExtParent;	//BIT_EXT_PARENT=1

	int t_ik_data_idx;// ボーンIndexサイズ | IKターゲットボーンのボーンIndex;
	int t_ik_data_Loop;
	float t_ik_data_LimitAng;
	std::vector<IkLink> IKLink;
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
	XMMATRIX mMtxPoseInv;	//
	//WORD mIkBoneIdx;
	Hierarchy mHierarchy;
private:
};

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



#include "../Loader/Animation/VMD.h"

class KeyFrame{
public:
	KeyFrame(const vmd::VMDKeyFrame& key) :mKeyFrame(key){}
	vmd::VMDKeyFrame mKeyFrame;
	bool operator<(const KeyFrame& o){ return mKeyFrame.FrameNo < o.mKeyFrame.FrameNo; }
private:
};

//class Motion{
//public:
//	std::vector<KeyFrame> mKeyFrame;
//private:
//};