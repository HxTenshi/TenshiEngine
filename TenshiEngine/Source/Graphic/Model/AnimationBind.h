#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "AnimeData.h"
#include "ModelStructs.h"


class AnimationBind{
public:
	struct BoneFrame
	{
		XMVECTOR Location;
		XMVECTOR Quaternion;
	};

	AnimationBind()
		:mIsLoop(true)
		, mKeyFraneTime(0)
		, mAnimeData(NULL)
		, mWeight(1.0f)
	{}
	~AnimationBind()
	{}

	void SetLoopFlag(bool flag){
		mIsLoop = flag;
	}
	void SetWeight(float weight){
		mWeight = min(max(weight,0.0f),1.0f);
	}
	float GetWeight(){
		return mWeight;
	}

	void PlayAnimetionAddTime(float add){
		if (!mAnimeData)return;
		float last = mAnimeData->GetLastFrameTime();
		mKeyFraneTime += add;
		//ループ有り
		if (mIsLoop){
			if (last!=0)mKeyFraneTime = fmod(mKeyFraneTime, last);
			if (mKeyFraneTime < 0){
				mKeyFraneTime = last + mKeyFraneTime;
			}

		}
		//ループ無し
		else{
			mKeyFraneTime = max(min(mKeyFraneTime, last), 0);
		}

		AnimationUpdate();
	}
	void PlayAnimetionSetTime(float frame){
		if (!mAnimeData)return;
		float last = mAnimeData->GetLastFrameTime();
		mKeyFraneTime = max(min(frame, last), 0);

		AnimationUpdate();
	}

	void Create(std::vector<Bone>& boneData, AnimeData* animeData){

		mAnimeData = animeData;
		DWORD mBoneNum = boneData.size();

		mBoneAnimations.resize(mBoneNum);

		int id = 0;
		//ボーン分回す
		for (auto& boneAnime : mBoneAnimations){

			auto& bone = boneData[id];
			//なければNULLがかえってくる
			auto boneMotion = mAnimeData->GetBoneMotion(bone.mStrName);
			boneAnime.BaseAnime = boneMotion;
			boneAnime.FramePoint = 0;


			//初期化
			auto& frame = boneAnime.CurrentKeyFrame;
			frame.Location = XMVectorSet(0.0f,0.0f,0.0f, 0.0f);
			frame.Quaternion = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

			id++;

		}
	}

	BoneFrame& GetBoneFrame(int id){
		return mBoneAnimations[id].CurrentKeyFrame;
	}

	float GetTime(){
		return mKeyFraneTime;
	}

private:

	//ボーン毎のアニメーション再生データ
	struct BoneAnimation{
		AnimeData::Motion *BaseAnime; //ボーンアニメーションデータ
		int FramePoint; //現在のアニメーション位置
		BoneFrame CurrentKeyFrame;
	};

	int FindCurrentKeyPosition(BoneAnimation& boneAnime, float time){

		auto size = (int)boneAnime.BaseAnime->size();

		int framePoint = boneAnime.FramePoint;
		framePoint = min(max(framePoint, 0), (size - 1));

		auto& Anime = *boneAnime.BaseAnime;
		auto curMotion = &Anime[framePoint];
		auto curTime = curMotion->mKeyFrame.FrameNo;

		//現在のキーより先か前か
		if (curTime <= time){
			//先
			int nextPoint = framePoint + 1;
			nextPoint = min(max(nextPoint, 0), (size - 1));

			auto nexMotion = &Anime[nextPoint];
			auto nexTime = nexMotion->mKeyFrame.FrameNo;

			//同じポイントなら処理する必要なし
			while (nextPoint != framePoint) {

				//現在のキー位置から次のキー位置の間なら抜ける
				if (curTime <= time &&
					nexTime > time){
					break;
				}

				//進める
				framePoint++;
				nextPoint++;

				//ネクストが最後までいったら抜ける
				if (nextPoint >= size){
					break;
				}

				++curMotion;
				++nexMotion;
				curTime = curMotion->mKeyFrame.FrameNo;
				nexTime = nexMotion->mKeyFrame.FrameNo;
			}


		}
		else{
			//前
			int nextPoint = framePoint;
			framePoint = min(max(nextPoint - 1, 0), (size - 1));

			curMotion = &Anime[framePoint];
			curTime = curMotion->mKeyFrame.FrameNo;
			auto nexMotion = &Anime[nextPoint];
			auto nexTime = nexMotion->mKeyFrame.FrameNo;

			//同じポイントなら処理する必要なし
			while (nextPoint != framePoint) {

				//現在のキー位置から次のキー位置の間なら抜ける
				if (curTime <= time &&
					nexTime > time){
					break;
				}

				//進める
				framePoint--;
				nextPoint--;

				//最初までいったら抜ける
				if (framePoint <= 0){
					break;
				}

				--curMotion;
				--nexMotion;
				curTime = curMotion->mKeyFrame.FrameNo;
				nexTime = nexMotion->mKeyFrame.FrameNo;
			}

		}


		return min(max(framePoint, 0), (size - 1));
	}

	void AnimationUpdate(){

		//ボーン分回す
		for (auto& boneAnime : mBoneAnimations){

			//アニメーション無し
			if (!boneAnime.BaseAnime){
				continue;
			}

			//現在のポイントを探す
			boneAnime.FramePoint = FindCurrentKeyPosition(boneAnime, mKeyFraneTime);

			//現在と次のフレームを取得
			auto size = boneAnime.BaseAnime->size();
			int curID = boneAnime.FramePoint;
			int nexID = min(curID + 1, size - 1);
			auto& Anime = *boneAnime.BaseAnime;
			vmd::VMDKeyFrame* key[2] = { &Anime[curID].mKeyFrame, &Anime[nexID].mKeyFrame };

			//キーフレーム位置の線形補完
			float t = 1.0f;
			if (curID != nexID){
				t = float(mKeyFraneTime - key[0]->FrameNo) / float(key[1]->FrameNo - key[0]->FrameNo);
			}
			if (t < 0.0f)t = 0.0f;
			if (t > 1.0f)t = 1.0f;

			//線形補完をカーブさせる
			float tx = Bezier(key[0]->Interpolation[0], key[0]->Interpolation[8], key[0]->Interpolation[4], key[0]->Interpolation[12], t);
			float ty = Bezier(key[0]->Interpolation[1], key[0]->Interpolation[9], key[0]->Interpolation[5], key[0]->Interpolation[13], t);
			float tz = Bezier(key[0]->Interpolation[2], key[0]->Interpolation[10], key[0]->Interpolation[6], key[0]->Interpolation[14], t);
			float tr = Bezier(key[0]->Interpolation[3], key[0]->Interpolation[11], key[0]->Interpolation[7], key[0]->Interpolation[15], t);

			XMVECTOR tv = XMVectorSet(tx, ty, tz, 0.0f);

			XMVECTOR pos[2];
			XMVECTOR rot[2];
			for (int i = 0; i < 2; i++)
			{
				pos[i] = XMVectorSet(key[i]->Location[0], key[i]->Location[1], key[i]->Location[2], 0.0f);
				rot[i] = XMVectorSet(key[i]->Rotatation[0], key[i]->Rotatation[1], key[i]->Rotatation[2], key[i]->Rotatation[3]);
			}

			auto& curFrame = boneAnime.CurrentKeyFrame;

			//ラープを適用
			curFrame.Location = XMVectorLerpV(pos[0], pos[1], tv);
			curFrame.Quaternion = XMQuaternionSlerp(rot[0], rot[1], tr);


		}
	}

	// http://d.hatena.ne.jp/edvakf/20111016/1318716097
	float Bezier(float x1, float x2, float y1, float y2, float x)
	{
		x1 = x1 / 127.0f;
		y1 = y1 / 127.0f;
		x2 = x2 / 127.0f;
		y2 = y2 / 127.0f;
		float t = 0.5f, s = 0.5f;
		for (int i = 0; i < 15; i++) {
			float ft = (3 * s*s *t *x1) + (3 * s *t*t *x2) + (t*t*t) - x;
			if (ft == 0) break;
			if (ft > 0){
				t -= 1.0f / float(4 << i);
			}
			else{ // ft < 0
				t += 1.0f / float(4 << i);
			}
			s = 1 - t;
		}
		return (3 * s*s *t *y1) + (3 * s *t*t *y2) + (t*t*t);
	}

	AnimeData* mAnimeData; //再生アニメーション
	std::vector<BoneAnimation> mBoneAnimations; //ボーンのアニメーション再生データ

	float mWeight;//適用率
	float mKeyFraneTime; //再生位置
	bool mIsLoop;

};
