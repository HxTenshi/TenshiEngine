
#pragma once
#include "main.h"


class CameraScript :public IDllScriptComponent{
public:
	void Start()override;
	void Update()override;

	void GameClear();

private:
	float mRotateY;
	float mLength;
	bool mClear;
};

class CoinScript : public IDllScriptComponent{
public:
	void Start()override;
	void Update()override;
private:
	float mRotateY;
};

class CoinManagerScript : public IDllScriptComponent{
public:
	void Start()override;
	void Update()override;
	void GetCoin();
private:
	int mGetCoinCount;
	int mMaxCoin;
	bool isClear;
};
