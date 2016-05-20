#pragma once


#include <vector>


#include "ConstantBuffer/ConstantBuffer.h"
#include "ConstantBuffer/ConstantBufferDif.h"

#include "../Loader/Animation/VMD.h"
#include "Engine/AssetDataBase.h"

#include "Engine/AssetFile/Bone/BoneFileData.h"

class AnimeData;
class AnimationBind;

class BoneModel{
public:
	BoneModel();
	~BoneModel();
	HRESULT Create(const char* FileName);
	//void CreateAnime(vmd& VMD);


	shared_ptr<AnimationBind> BindAnimation(AnimeData* data);

	void SetConstantBuffer(ID3D11DeviceContext* context) const;

	void UpdateAnimation(std::vector<shared_ptr<AnimationBind>>& anime);
	//void PlayVMD(float time);
	//bool IsCreateAnime(){ return mIsCreateAnime; }

	//UINT GetMaxAnimeTime();

private:

	//void VMDAnimation(float key_time);
	//void VMDIkAnimation();
	//void UpdatePose();

	void createBone();
	void createIk(DWORD ikCount, UINT bidx);

	//std::vector<Motion> mMotion;

	std::vector<XMVECTOR> mBoneInitPos;
	std::vector<Bone> mBone;
	std::vector<Ik> mIk;

	//UINT mLastKeyNo;


	ConstantBufferArray<cbBoneMatrix> mCBBoneMatrix;

	//bool mIsCreateAnime;
	mutable bool mIsChangeMatrix;

	BoneAssetDataPtr mBoneAssetDataPtr;
};