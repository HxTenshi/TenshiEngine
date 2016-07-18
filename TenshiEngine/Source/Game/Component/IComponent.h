#pragma once

#include <String>
#include "MySTL/File.h"
#include "MySTL/ioHelper.h"

class Actor;

class Component{
public:
	Component()
		:gameObject(NULL){

	}
	virtual ~Component(){

	}
	//コンポーネント共通初期化
	void _Initialize(Actor* obj){
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
	virtual void EngineUpdate(){

	}
	virtual void Update(){

	}

	virtual void ChangeParentCallback(){

	}

#ifdef _ENGINE_MODE
	virtual void CreateInspector(){

	}
#endif

	void ExportClassName(File& f){
		std::string name = ClassName();
		f.Out(name);
	}
	std::string ClassName(){
		std::string name = typeid(*this).name();
		name.erase(0, 6);//"class "を削除
		return name;
	}
	virtual void IO_Data(I_ioHelper* io) = 0;

	Actor* gameObject;
};