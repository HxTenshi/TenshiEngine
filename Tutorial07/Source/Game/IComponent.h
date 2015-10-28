#pragma once

#include <String>
#include "MySTL/File.h"

class Actor;

class Component{
public:
	Component(){

	}
	void _Initialize(Actor* obj){
		gameObject = obj;
	}

	virtual void Initialize(){

	}
	virtual ~Component(){

	}
	virtual void Update(){

	}

	virtual void CreateInspector(){

	}

	void ExportClassName(File& f){
		std::string name = typeid(*this).name();
		name.erase(0, 6);//"class "ÇçÌèú
		f.Out(name);
	}
	virtual void ExportData(File& f) = 0;
	virtual void ImportData(File& f) = 0;

	Actor* gameObject;
};