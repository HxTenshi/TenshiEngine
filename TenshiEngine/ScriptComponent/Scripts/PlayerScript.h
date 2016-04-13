#pragma once
#include "main.h"


class PlayerScript :public IDllScriptComponent{
public:
	PlayerScript();
	void Update()override;
	void OnCollide(Actor* target) override;
	float mRotateY;

};