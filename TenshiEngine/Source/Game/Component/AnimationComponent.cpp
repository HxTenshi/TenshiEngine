
#include <functional>
#include "AnimationComponent.h"

#include "ModelComponent.h"

#include "Component.h"
#include "Graphic/Loader/Animation/VMD.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Model/BoneModel.h"

AnimationComponent::AnimationComponent()
	:mCurrentSet(0)
{}
void AnimationComponent::Initialize(){

	mAnimationSets.resize(10);

	for (auto& set : mAnimationSets){
		set.Create();
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
			set.Update(bone);
		}

		auto& set = mAnimationSets[mCurrentSet];
		mView.mTime = set.mTime;
		mView.mTimeScale = set.mTimeScale;
		mView.mWeight = set.mWeight;
		mView.mLoop = set.mLoop;
		mView.mFileName = set.mFileName;

		std::vector<shared_ptr<AnimationBind>> binds(10);
		
		for (int i = 0; i < 10;i++){
			binds[i] = mAnimationSets[i].mAnimationBind;
		}

		bone->UpdateAnimation(binds);
	}
}
void AnimationComponent::CreateInspector(){
	auto data = Window::CreateInspector();

	std::function<void(int)> collbackset = [&](int f){
		mCurrentSet = min(max(f, 0), 9);
		auto& set = mAnimationSets[mCurrentSet];
		mView.mTime = set.mTime;
		mView.mTimeScale = set.mTimeScale;
		mView.mWeight = set.mWeight;
		mView.mLoop = set.mLoop;
		mView.mFileName = set.mFileName;
	};
	Window::AddInspector(new TemplateInspectorDataSet<int>("ID", &mCurrentSet, collbackset), data);

	std::function<void(float)> collback = [&](float f){
		mView.mTime = f;
		mAnimationSets[mCurrentSet].mTime = f;
	};
	Window::AddInspector(new TemplateInspectorDataSet<float>("Time", &mView.mTime, collback), data);


	std::function<void(float)> collbackscale = [&](float f){
		 mView.mTimeScale = f;
		 mAnimationSets[mCurrentSet].mTimeScale = f;
	};
	Window::AddInspector(new TemplateInspectorDataSet<float>("TimeScale", &mView.mTimeScale, collbackscale), data);

	std::function<void(float)> collbackw = [&](float f){
		mView.mWeight = f;
		mAnimationSets[mCurrentSet].mWeight = f;
		if (mAnimationSets[mCurrentSet].mAnimationBind){
			mAnimationSets[mCurrentSet].mAnimationBind->SetWeight(f);
		}
	};
	Window::AddInspector(new TemplateInspectorDataSet<float>("Weight", &mView.mWeight, collbackw), data);

	std::function<void(bool)> collbackloop= [&](bool f){
		mView.mLoop = f;
		mAnimationSets[mCurrentSet].mLoop = f;
		if (mAnimationSets[mCurrentSet].mAnimationBind){
			 mAnimationSets[mCurrentSet].mAnimationBind->SetLoopFlag(f);
		}
	};
	Window::AddInspector(new TemplateInspectorDataSet<bool>("Loop", &mView.mLoop, collbackloop), data);

	std::function<void(std::string)> collbackpath = [&](std::string name){
		mView.mFileName = name;
		mAnimationSets[mCurrentSet].mFileName = name;
		mAnimationSets[mCurrentSet].Create();

	};
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("VMD", &mView.mFileName, collbackpath), data);

	Window::ViewInspector("Animetion", this, data);
}

void AnimationComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	//_KEY(mFileName);
	//_KEY(mTime);
	//_KEY(mTimeScale);
	//_KEY(mLoop);
#undef _KEY
}








void AnimeSet::Update(BoneModel* bone){
	//‚È‚¯‚ê‚Îì¬
	if (!mAnimationBind){
		mAnimationBind = bone->BindAnimation(&mAnimeData);

		mAnimationBind->SetLoopFlag(mLoop);
		mAnimationBind->SetWeight(mWeight);
		mAnimationBind->PlayAnimetionSetTime(mTime);
	}

	if (!mAnimationBind)return;

	mTime = mAnimationBind->GetTime();
	mAnimationBind->PlayAnimetionAddTime(0.5f * mTimeScale);
}