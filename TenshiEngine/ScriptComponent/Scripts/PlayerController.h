
#pragma once
#include "main.h"

class CharacterControllerComponent;
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

	void move();
	void moveUpdate();
	void rotate();
	void doge();
	void guard();
	void attack();

	//ÉÅÉìÉoïœêî

	SERIALIZE
	float m_JumpPower;
	SERIALIZE
	float m_MoveSpeed;
	SERIALIZE
	GameObject m_WeaponHand;

	XMVECTOR mJump;
	XMVECTOR mGravity;

	XMVECTOR mRotate;

	bool m_IsDoge;
	bool m_IsGround;

	weak_ptr<CharacterControllerComponent> m_CharacterControllerComponent;
};