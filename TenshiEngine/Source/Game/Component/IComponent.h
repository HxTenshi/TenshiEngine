#pragma once

#include <String>
#include "Game/Script/GameObject.h"
#include "Game/Parts/Enabled.h"
#include "MySTL/ioHelper.h"


class Actor;
class I_ioHelper;

class Component
	:public Enabled
{
public:
	Component()
		:gameObject(NULL){

	}
	virtual ~Component(){

	}
	//コンポーネント共通初期化
	void _Initialize(GameObject obj){
		gameObject = obj;
	}
	//AddComponent後の初期化
	virtual void Initialize(){

	}
	//GameStart時の初期化
	virtual void Start(){

	}
	//デストロイ時の処理
	virtual void Finish(){

	}
	virtual void Update(){

	}

	virtual void ChangeParentCallback(){

	}

#ifdef _ENGINE_MODE
	virtual void EngineUpdate(){}
	virtual void CreateInspector(){}
#endif
	std::string ClassName(){
		std::string name = typeid(*this).name();
		name.erase(0, 6);//"class "を削除
		return name;
	}
	virtual void IO_Data(I_ioHelper* io) = 0;

	GameObject gameObject;

};