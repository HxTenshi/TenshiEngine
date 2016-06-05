#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"

#include "Graphic/Model/AnimeData.h"
#include "Graphic/Model/AnimationBind.h"

struct AnimeParam{
	//アニメーションの再生位置
	float mTime = 0.0f;
	//アニメーションの再生速度
	float mTimeScale = 1.0f;
	//アニメーションのループ
	bool mLoop = true;
	//アニメーションのループ
	float mWeight = 0.0f;

	//アニメーションVMDファイルのパス
	std::string mFileName = "";
};

class BoneModel;
struct AnimeSet{

	~AnimeSet(){
		mAnimationBind = NULL;
		mAnimeData.Clear();
	}
	void Create(){
		mAnimationBind = NULL;
		if (!Param.mFileName.empty()){
			mAnimeData.Create(Param.mFileName);
		}
	}

	void Update(BoneModel* bone);

	AnimeParam Param;

	AnimeData mAnimeData;
	shared_ptr<AnimationBind> mAnimationBind = NULL;
};

class IAnimationComponent{
public:
	virtual ~IAnimationComponent(){}
	virtual void ChangeAnimetion(int id) = 0;
	virtual AnimeParam GetAnimetionParam(int id) = 0;
	virtual void SetAnimetionParam(int id, const AnimeParam& param) = 0;
};

class ModelComponent;
//class vmd;
class AnimationComponent :public IAnimationComponent, public Component{
public:
	AnimationComponent();
	void Initialize() override;
	void Finish() override;
	void Update() override;
#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void ChangeAnimetion(int id)override;
	AnimeParam GetAnimetionParam(int id) override;
	void SetAnimetionParam(int id,const AnimeParam& param) override;

	void IO_Data(I_ioHelper* io) override;

	weak_ptr<ModelComponent> mModel;

	int mCurrentSet;
	AnimeSet mView;
	std::vector<AnimeSet> mAnimationSets;
	int mAnimetionCapacity;
};