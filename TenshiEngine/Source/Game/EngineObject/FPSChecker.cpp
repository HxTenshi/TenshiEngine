
#include "FPSChecker.h"
#include <vector>

#include "Game/Actor.h"
#include "Game/Component/TextComponent.h"
#include "MySTL/ptr.h"


#include "Window/Window.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Game/Component/TextureModelComponent.h"
class Text : public Actor{
public:
	Text(const XMFLOAT2& TopLeft, const XMFLOAT2& DownRight)
	{
		Name("fps");

		mTransform = mComponents.AddComponent<TransformComponent>();
		
		//mComponents.AddComponent<MaterialComponent>();

		//mComponents.AddComponent<TextureModelComponent>();

		mComponents.AddComponent<TextComponent>();


		//auto mPixelTopLeft = TopLeft;
		//auto mPixelDownRight = DownRight;
		//XMFLOAT2 mTopLeft;
		//XMFLOAT2 mDownRight;

		float x = DownRight.x - TopLeft.x;
		float y = DownRight.y - TopLeft.y;


		x = TopLeft.x + x / 2;
		y = TopLeft.y + y / 2;
		x = x / WindowState::mWidth;
		y = y / WindowState::mHeight;
		y = y - 1;
		//mTopLeft.x = TopLeft.x / WindowState::mWidth;
		//mTopLeft.y = 1 - DownRight.y / WindowState::mHeight;
		//
		//mDownRight.x = DownRight.x / WindowState::mWidth;
		//mDownRight.y = 1 - TopLeft.y / WindowState::mHeight;


		//mTransform->Position(XMVectorSet(-0.8f,0.5f, 0, 1));
		//mTransform->Scale(XMVectorSet(0.5f, 0.5f,1,1));
		mTransform->Position(XMVectorSet(200,200, 0, 1));
		mTransform->Scale(XMVectorSet(400, 400,0,1));
	}
	~Text(){
		Finish();
	}
};

FPSChecker::FPSChecker()
	: mFPSObject(new Text({ 0, 0 }, { 500, 800 }))
	, mFrameNum(60)
{
	mFPSObject->Initialize();
	mFPSObject->Start();
}
FPSChecker::~FPSChecker(){
	delete mFPSObject;
}

void FPSChecker::Update(float deltaTime){

	static int mCorrentVectorPos = 0;
	static weak_ptr<TextComponent> mFPSText = mFPSObject->GetComponent<TextComponent>();
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
	std::string title = "FPS:" + fpsstr.substr(0, 4) + "‚¾‚æ`š";

	if (mFPSText)mFPSText->ChangeText(title);
	mFPSObject->UpdateComponent(deltaTime);
}