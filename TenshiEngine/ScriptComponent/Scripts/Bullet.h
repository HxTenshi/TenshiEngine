
#pragma once
#include "main.h"

class Bullet :public IDllScriptComponent{
public:
	void Initialize()override;
	void Start()override;
	void Update()override;
	void Finish()override;
	void OnCollideBegin(GameObject target)override;
	void OnCollideEnter(GameObject target)override;
	void OnCollideExit(GameObject target)override;

private:
	//メンバ変数
	SERIALIZE
	float mDeadTime;
	SERIALIZE
	float mSpeed;
};