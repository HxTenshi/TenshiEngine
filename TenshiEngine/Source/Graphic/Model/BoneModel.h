#pragma once


#include <vector>


#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

#include "../Loader/Animation/VMD.h"
#include "Engine/AssetDataBase.h"

#include "Engine/AssetFile/Bone/BoneFileData.h"

class AnimeData;
struct AnimeSet;
class AnimationBind;

class BoneModel{
public:
	BoneModel();
	~BoneModel();
	HRESULT Create(const char* FileName);
	HRESULT Create(BoneAsset& asset);
	//void CreateAnime(vmd& VMD);


	shared_ptr<AnimationBind> BindAnimation(const AnimeData* data);

	void SetConstantBuffer(ID3D11DeviceContext* context) const;

	void UpdateAnimation(const std::vector<AnimeSet>& anime);
	//void PlayVMD(float time);
	//bool IsCreateAnime(){ return mIsCreateAnime; }

	//UINT GetMaxAnimeTime();

	std::vector<Bone>& GetBones() { return mBone; }

private:

	//void VMDAnimation(float key_time);
	//void VMDIkAnimation();
	//void UpdatePose();

	void createBone();
	void createIk(DWORD ikCount, UINT bidx);

	//std::vector<Motion> mMotion;

	std::vector<XMVECTOR> mBoneInitPos;
	std::vector<Bone> mBone;
	//std::vector<Ik> mIk;

	//UINT mLastKeyNo;


	ConstantBufferArray<cbBoneMatrix> mCBBoneMatrix;

	//bool mIsCreateAnime;
	mutable bool mIsChangeMatrix;

	BoneAssetDataPtr mBoneAssetDataPtr;
};