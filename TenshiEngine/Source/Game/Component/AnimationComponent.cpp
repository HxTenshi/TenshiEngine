
#include <functional>
#include "AnimationComponent.h"

#include "ModelComponent.h"

#include "Component.h"
#include "Graphic/Loader/Animation/VMD.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Model/BoneModel.h"

AnimationComponent::AnimationComponent()
		:mTime(0.0f)
		, mVMD(NULL)
	{}
void AnimationComponent::Initialize(){

		mTime = 0.0f;

		if (!mFileName.empty()){
			mVMD = new vmd(mFileName.c_str());
			if (!mVMD->mLoadResult){
				delete mVMD;
				mVMD = NULL;
			}
		}
	}

void AnimationComponent::Finish(){
		if (mVMD){
			delete mVMD;
			mVMD = NULL;
		}
	}
void AnimationComponent::Update(){


		mTime += 1.0f / 2.0f;

		if (!mVMD)return;

		if (!mModel){
			mModel = gameObject->GetComponent<ModelComponent>();
			if (!mModel)return;
		}
		if (mModel && mModel->mModel && mModel->mModel->mBoneModel){


			if (!mModel->mModel->mBoneModel->IsCreateAnime())
				mModel->mModel->mBoneModel->CreateAnime(*mVMD);

			auto max = mModel->mModel->mBoneModel->GetMaxAnimeTime();
			if (mTime > max){
				mTime -= max;
			}

			mModel->mModel->mBoneModel->PlayVMD(mTime);
		}
	}
void AnimationComponent::CreateInspector(){
	auto data = Window::CreateInspector();

	std::function<void(float)> collback = [&](float f){
		mTime = f;
		mTime -= 1.0f / 2.0f;
		Update();
	};
	Window::AddInspector(new TemplateInspectorDataSet<float>("Time", &mTime, collback), data);

	std::function<void(std::string)> collbackpath = [&](std::string name){
		mFileName = name;

		if (mVMD){
			delete mVMD;
			mVMD = NULL;
		}
		if (!mFileName.empty()){
			mVMD = new vmd(mFileName.c_str());
			if (mVMD->mLoadResult){
				if (mModel && mModel->mModel && mModel->mModel->mBoneModel){
					mModel->mModel->mBoneModel->CreateAnime(*mVMD);
				}
			}
			else{
				delete mVMD;
				mVMD = NULL;
			}
		}

	};
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("VMD", &mFileName, collbackpath), data);

	Window::ViewInspector("Animetion", this, data);
}