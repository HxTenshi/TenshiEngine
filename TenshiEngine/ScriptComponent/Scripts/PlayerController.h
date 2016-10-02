
#pragma once
#include "main.h"


class PlayerController :public IDllScriptComponent{
public:
	void Initialize()override;
	void Start()override;
	void Update()override;
	void Finish()override;
	void OnCollideBegin(GameObject target)override;
	void OnCollideEnter(GameObject target)override;
	void OnCollideExit(GameObject target)override;

private:
	//ƒƒ“ƒo•Ï”

	XMVECTOR mJump;
	XMVECTOR mGravity;
	XMVECTOR mToQuaternion;

	XMVECTOR mRotate;
};