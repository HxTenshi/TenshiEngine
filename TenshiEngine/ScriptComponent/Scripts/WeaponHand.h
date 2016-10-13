
#pragma once
#include "main.h"


class WeaponHand :public IDllScriptComponent{
public:
	void Initialize()override;
	void Start()override;
	void Update()override;
	void Finish()override;
	void OnCollideBegin(GameObject target)override;
	void OnCollideEnter(GameObject target)override;
	void OnCollideExit(GameObject target)override;


	void SetWeapon(GameObject weapon);

	void Atack();
	void Guard();
private:
	//ƒƒ“ƒo•Ï”
	GameObject mWeapon;

	SERIALIZE
	GameObject m_GuardPos;
	float m_AtackTime;
	bool m_IsGuard;
};