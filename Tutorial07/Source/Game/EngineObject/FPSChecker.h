#pragma once

#include "GameEngineObject.h"

class Actor;

class FPSChecker : public GameEngineObject{
public:
	FPSChecker();
	~FPSChecker();
	void Update(float deltaTime) override;

private:

	FPSChecker& operator = (const FPSChecker&);

	Actor* mFPSObject;

	const int mFrameNum;

};