#pragma once

#include "GameEngineObject.h"
#include "Game/Script/GameObject.h"

class Actor;

class FPSChecker : public GameEngineObject{
public:
	FPSChecker();
	~FPSChecker();
	void Update() override;

private:

	FPSChecker& operator = (const FPSChecker&);

	shared_ptr<Actor> mFPSObject;

	const int mFrameNum;

};