
#pragma once
#include "main.h"


class DeltaTimeScale :public IDllScriptComponent{
public:
	DeltaTimeScale();
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
	float mDeltaTimeScale;
};