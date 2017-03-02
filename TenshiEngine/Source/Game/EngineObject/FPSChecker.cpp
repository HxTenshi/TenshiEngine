
#include "FPSChecker.h"
#include <vector>

#include "Game/Actor.h"
#include "Game/Component/TextComponent.h"
#include "MySTL/ptr.h"


#include "Window/Window.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Game/Component/TextureModelComponent.h"

#include "Game/SettingObject/Canvas.h"
class Text : public Actor{
public:
	Text()
	{
		Name("fps");

		mTransform = mComponents.AddComponent<TransformComponent>();
		mComponents.AddComponent<TextComponent>();

		mTransform->Position(XMVectorSet(0, Canvas::GetHeight(),9999999.0f, 1));
		mTransform->Scale(XMVectorSet(512, 512,0,1));
		weak_ptr<TextComponent> mFPSText = GetComponent<TextComponent>();
		mFPSText->SetTextureCenter(XMFLOAT2(0.0f,0.0f));
	}
	~Text(){
		Finish();
	}
};

FPSChecker::FPSChecker()
	: mFPSObject(make_shared<Text>())
	, mFrameNum(60)
{
}
FPSChecker::~FPSChecker(){
	//delete mFPSObject;
}

void FPSChecker::Initialize()
{
	mFPSObject->Initialize();
	mFPSObject->Start();
}

void FPSChecker::Update(){

	mFPSObject->mTransform->Position(XMVectorSet(0, Canvas::GetHeight(), 0, 1));

	static int mCorrentVectorPos = 0;
	weak_ptr<TextComponent> mFPSText = mFPSObject->GetComponent<TextComponent>();
	static std::vector<unsigned long> mStepFrame_times(mFrameNum, 0);

	auto old_time = mStepFrame_times[mCorrentVectorPos];
	auto current_time = timeGetTime();
	mStepFrame_times[mCorrentVectorPos] = current_time;
	mCorrentVectorPos = ++mCorrentVectorPos%mFrameNum;

	auto sec_time = (current_time - old_time);
	if (sec_time == 0.0f){
		sec_time = 1;
	}
	float fps = (float)mFrameNum / sec_time * 1000;
	auto fpsstr = std::to_string(fps);
	std::string title = "FPS:" + fpsstr.substr(0, 4);

	if (mFPSText)mFPSText->ChangeText(title);
	mFPSObject->UpdateComponent();
}