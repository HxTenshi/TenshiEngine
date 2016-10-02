
#pragma once
#include "main.h"


class TPSCamera :public IDllScriptComponent{
public:
	TPSCamera();
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
	float mDistance;
	SERIALIZE
	float mUp;
	SERIALIZE
	GameObject mTarget;
};