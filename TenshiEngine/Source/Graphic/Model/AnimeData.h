#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "ModelStructs.h"

class AnimeData{
public:


	typedef std::vector<KeyFrame> Motion;
	typedef Motion::iterator KeyFrameIte;

	AnimeData()
		:mUseBoneNum(0)
		, mLastKeyNo(0)
	{
	}
	~AnimeData()
	{}

	//IDの取得
	int GetBoneID(const std::string& boneName){
		if (mBoneName2ID.count(boneName) == 0)return -1;
		return  mBoneName2ID[boneName].id;
	}
	//ボーンモーションの取得
	Motion* GetBoneMotion(const std::string& boneName){
		int id = GetBoneID(boneName);
		if (id == -1)return NULL;
		return &mBoneMoution[id];
	}

	float GetLastFrameTime(){
		return mLastKeyNo;
	}

	void Clear(){
		mBoneMoution.clear();
		mBoneName2ID.clear();
		mUseBoneNum = 0;
		mLastKeyNo = 0;
	}

	void Create(const std::string& fileName){

		mBoneMoution.clear();
		mBoneName2ID.clear();
		mUseBoneNum = 0;
		mLastKeyNo = 0;

		vmd Anime(fileName.c_str());
		if (!Anime.mLoadResult)return;

		//使用するボーンの洗い出し
		for (unsigned int i = 0; i < Anime.mKeyNum; i++){
			auto& key = Anime.mKeyFrame[i];
			//ボーン名とIDの割り振り
			SettingBoneID(key.BoneName);


			//最後のキーフレームがアニメーションの終わり
			if (key.FrameNo > (unsigned int)mLastKeyNo){
				mLastKeyNo = key.FrameNo;
			}
		}

		//使用するボーンだけ用意
		mBoneMoution.resize(mUseBoneNum);

		//ボーンごとにモーション登録
		for (unsigned int i = 0; i < Anime.mKeyNum; i++){
			auto& key = Anime.mKeyFrame[i];
			PushBoneMotion(key);
		}

		//ボーンモーションのソート
		for (auto& motion : mBoneMoution){
			std::sort(motion.begin(), motion.end());
		}
	}

private:
	struct BoneId{
		int id = -1;
	};

	//ボーン名とIDの割り振り
	void SettingBoneID(const std::string& boneName){

		auto& boneid = mBoneName2ID[boneName];
		//登録済み
		if (boneid.id != -1){
			return;
		}

		boneid.id = mUseBoneNum;
		mUseBoneNum++;
	}


	void PushBoneMotion(const vmd::VMDKeyFrame& key){

		auto moution = GetBoneMotion(key.BoneName);

		moution->push_back(KeyFrame(key));

	}

	std::vector<Motion> mBoneMoution;	//ボーン毎のキーフレーム
	std::unordered_map<std::string, BoneId>  mBoneName2ID;	//ボーンとIDの関連付け
	int mUseBoneNum;	//使用ボーン数
	int mLastKeyNo;		//最後のキーフレーム

};
