
#include <functional>
#include "AnimationComponent.h"

#include "ModelComponent.h"

#include "Component.h"
#include "Graphic/Loader/Animation/VMD.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Model/BoneModel.h"
#include "Engine/AssetLoad.h"

#include "Engine/Inspector.h"

#include "Engine/AssetFile/Animation/AnimationFileData.h"


AnimationComponent::AnimationComponent()
	:mCurrentSet(0)
	, mAnimetionCapacity(10)
	, mAnimationSets(10)
{}
void AnimationComponent::Initialize(){
	for (auto& set : mAnimationSets){
		set.Create();
	}
	changeAnimetion(0);
}

void AnimationComponent::Start() {
	if (!mModel) {
		mModel = gameObject->GetComponent<ModelComponent>();
		if (!mModel)return;
	}
	if (mModel && mModel->mModel && mModel->mModel->mBoneModel) {

		auto bone = mModel->mModel->mBoneModel;
		for (auto& set : mAnimationSets) {
			set.Bind(bone);
		}
	}
}

void AnimationComponent::Finish(){
	mAnimationSets.clear();
}
void AnimationComponent::Update(){


	if (!mModel){
		mModel = gameObject->GetComponent<ModelComponent>();
		if (!mModel)return;
	}
	if (mModel && mModel->mModel && mModel->mModel->mBoneModel){

		auto bone = mModel->mModel->mBoneModel;

		for (auto& set : mAnimationSets){
			if (!set.mAnimationBind) {
				set.Bind(bone);
			}
			set.Update();
		}

		auto& set = mAnimationSets[mCurrentSet];
		mView.Param = set.Param;

		//std::vector<shared_ptr<AnimationBind>> binds(mAnimetionCapacity);
		//
		//for (int i = 0; i < mAnimetionCapacity; i++){
		//	binds[i] = mAnimationSets[i].mAnimationBind;
		//}

		bone->UpdateAnimation(mAnimationSets);
	}
}
#ifdef _ENGINE_MODE
void AnimationComponent::CreateInspector(){

	Inspector ins("Animetion",this);
	ins.AddEnableButton(this);

	std::function<void(int)> collbackset = [&](int f){
		changeAnimetion(f);
	};
	ins.Add("Capacity", &mAnimetionCapacity, [&](int f) {
		mAnimetionCapacity = f;
		mAnimationSets.resize(mAnimetionCapacity);
	});

	ins.Add("ID", &mCurrentSet, collbackset);

	std::function<void(float)> collback = [&](float f){
		mView.Param.mTime = f;
		mAnimationSets[mCurrentSet].Param.mTime = f;

		mAnimationSets[mCurrentSet].mAnimationBind->PlayAnimetionSetTime(mAnimationSets[mCurrentSet].Param.mTime);
	};
	ins.Add("Time", &mView.Param.mTime, collback);


	std::function<void(float)> collbackscale = [&](float f){
		mView.Param.mTimeScale = f;
		mAnimationSets[mCurrentSet].Param.mTimeScale = f;
	};
	ins.Add("TimeScale", &mView.Param.mTimeScale, collbackscale);

	std::function<void(float)> collbackw = [&](float f){
		mView.Param.mWeight = f;
		mAnimationSets[mCurrentSet].Param.mWeight = f;
		if (mAnimationSets[mCurrentSet].mAnimationBind){
			mAnimationSets[mCurrentSet].mAnimationBind->SetWeight(f);
		}
	};
	ins.Add("Weight", &mView.Param.mWeight, collbackw);

	std::function<void(bool)> collbackloop= [&](bool f){
		mView.Param.mLoop = f;
		mAnimationSets[mCurrentSet].Param.mLoop = f;
		if (mAnimationSets[mCurrentSet].mAnimationBind){
			 mAnimationSets[mCurrentSet].mAnimationBind->SetLoopFlag(f);
		}
	};
	ins.Add("Loop", &mView.Param.mLoop, collbackloop);


	ins.Add("VMD", &mView.Param.mAnimationAsset, [&]() {
		mAnimationSets[mCurrentSet].Param.mAnimationAsset = mView.Param.mAnimationAsset;
		mAnimationSets[mCurrentSet].Create();

	});

	ins.Complete();
}
#endif

void AnimationComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY_I(i_,x) io->func( x , (#x + std::to_string(##i_)).c_str() )
#define _KEY(x) io->func( x , #x)

	_KEY(mAnimetionCapacity);
	if (io->isInput()) {
		mAnimationSets.resize(mAnimetionCapacity);
	}

	for (int i = 0; i < mAnimetionCapacity; i++){
		_KEY_I(i, mAnimationSets[i].Param.mAnimationAsset);
		_KEY_I(i, mAnimationSets[i].Param.mTime);
		_KEY_I(i, mAnimationSets[i].Param.mTimeScale);
		_KEY_I(i, mAnimationSets[i].Param.mLoop);
		_KEY_I(i, mAnimationSets[i].Param.mWeight);
	}

#undef _KEY
}

void AnimationComponent::changeAnimetion(int id){
	mCurrentSet = min(max(id, 0), (mAnimetionCapacity - 1));
	auto& set = mAnimationSets[mCurrentSet];
	mView.Param = set.Param;
}

AnimeParam AnimationComponent::GetAnimetionParam(int id){
	id = min(max(id, 0), (mAnimetionCapacity - 1));
	return mAnimationSets[id].Param;
}
void AnimationComponent::SetAnimetionParam(int id,const AnimeParam& param){
	id = min(max(id, 0), (mAnimetionCapacity - 1));
	//bool createflag = false;
	//if (mAnimationSets[id].Param.mFileName != param.mFileName){
	//	createflag = true;
	//}
	bool create = mAnimationSets[id].Param.mAnimationAsset.m_Hash == param.mAnimationAsset.m_Hash;
	mAnimationSets[id].Param = param;
	if (!create) {
		mAnimationSets[id].Create();
		if (mModel && mModel->mModel && mModel->mModel->mBoneModel) {
			auto bone = mModel->mModel->mBoneModel;
			mAnimationSets[id].Bind(bone);
		}
	}
	if (!mAnimationSets[id].mAnimationBind)return;
	mAnimationSets[id].mAnimationBind->SetWeight(mAnimationSets[id].Param.mWeight);
	mAnimationSets[id].mAnimationBind->SetLoopFlag(mAnimationSets[id].Param.mLoop);
	mAnimationSets[id].mAnimationBind->PlayAnimetionSetTime(mAnimationSets[id].Param.mTime);
}

float AnimationComponent::GetTotalTime(int id)
{
	id = min(max(id, 0), (mAnimetionCapacity - 1));
	if (!mAnimationSets[id].mAnimationBind)return 0.0f;

	auto& asset = mAnimationSets[id].Param.mAnimationAsset;
	if (!asset.IsLoad())return 0.0f;
	auto animedata = asset.Get();
	if (!animedata)return 0.0f;
	return animedata->GetAnimeData().GetLastFrameTime();
}

bool AnimationComponent::IsAnimationEnd(int id)
{
	id = min(max(id, 0), (mAnimetionCapacity - 1));
	if (!mAnimationSets[id].mAnimationBind)return false;

	auto& asset = mAnimationSets[id].Param.mAnimationAsset;
	if (!asset.IsLoad())return false;
	auto animedata = asset.Get();
	if (!animedata)return false;
	return animedata->GetAnimeData().GetLastFrameTime() == mAnimationSets[id].mAnimationBind->GetTime();
}



#include "Game/Game.h"

AnimeSet::AnimeSet()
{
	mAnimationBind = NULL;
}

AnimeSet::~AnimeSet()
{
	mAnimationBind = NULL;
}

void AnimeSet::Create()
{
	if (!Param.mAnimationAsset.IsLoad()) {
		AssetLoad::Instance(Param.mAnimationAsset.m_Hash, Param.mAnimationAsset);
	}
	mAnimationBind = NULL;
}

void AnimeSet::Bind(BoneModel * bone)
{
	if (!Param.mAnimationAsset.IsLoad())return;
	auto animedata = Param.mAnimationAsset.Get();
	if (!animedata)return;
	mAnimationBind = bone->BindAnimation(&animedata->GetAnimeData());

	mAnimationBind->SetLoopFlag(Param.mLoop);
	mAnimationBind->SetWeight(Param.mWeight);
	mAnimationBind->PlayAnimetionSetTime(Param.mTime);
}

void AnimeSet::Update()
{
	if (!mAnimationBind)return;
	auto time = Game::GetDeltaTime()->GetDeltaTime();
	//‚P•b‚Å‚R‚OƒtƒŒ[ƒ€
	mAnimationBind->PlayAnimetionAddTime((30)*time * Param.mTimeScale);
	Param.mTime = mAnimationBind->GetTime();
}
