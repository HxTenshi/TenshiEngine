
#pragma once
#include "main.h"

class DllScriptComponent :public IDllScriptComponent{
public:
	DllScriptComponent();
	virtual void Update(Actor* This)override;

private:
	int timer;
	int timer2;
};
class PlayerScript :public IDllScriptComponent{
public:
	PlayerScript();
	virtual void Update(Actor* This)override;
	float mRotateY;

};
class CameraScript :public IDllScriptComponent{
public:
	CameraScript();
	virtual void Update(Actor* This)override;

private:
	float mRotateY;
};