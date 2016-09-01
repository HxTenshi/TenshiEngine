
#pragma once
#include "main.h"


class TPSCamera :public IDllScriptComponent{
public:
	TPSCamera();
	void Initialize()override;
	void Start()override;
	void Update()override;
	void Finish()override;
	void OnCollideBegin(Actor* target)override;
	void OnCollideEnter(Actor* target)override;
	void OnCollideExit(Actor* target)override;

private:
	//ƒƒ“ƒo•Ï”
	SERIALIZE
	std::string mTargetName;
	SERIALIZE
	float mDistance;
	SERIALIZE
	float mUp;
	Actor* mTarget;
};