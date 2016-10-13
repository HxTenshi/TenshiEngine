
#pragma once
#include "main.h"


class GetWeapon :public IDllScriptComponent{
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

	GameObject mMinWeapon;
	SERIALIZE
	GameObject mWeaponHand;
	SERIALIZE
	PrefabAsset mGetParticle1;
	SERIALIZE
	PrefabAsset mGetParticle2;
};