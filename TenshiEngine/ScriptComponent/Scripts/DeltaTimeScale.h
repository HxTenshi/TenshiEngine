
#pragma once
#include "main.h"


class DeltaTimeScale :public IDllScriptComponent{
public:
	DeltaTimeScale();
	void Initialize()override;
	void Start()override;
	void Update()override;
	void Finish()override;
	void OnCollideBegin(GameObject target)override;
	void OnCollideEnter(GameObject target)override;
	void OnCollideExit(GameObject target)override;

private:
	//ƒƒ“ƒo•Ï”
	SERIALIZE
	float mDeltaTimeScale;
};