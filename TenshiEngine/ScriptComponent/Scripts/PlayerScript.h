#pragma once
#include "main.h"


class PlayerScript :public IDllScriptComponent{
public:
	PlayerScript();
	void Update()override;
	void OnCollideBegin(GameObject target) override;
	float mRotateY;

};