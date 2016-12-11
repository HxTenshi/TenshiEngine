
#pragma once
#include "main.h"


class testhit :public IDllScriptComponent{
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
	SERIALIZE GameObject m_Obj1;
	SERIALIZE GameObject m_Obj2;
	SERIALIZE GameObject m_Obj3;
};