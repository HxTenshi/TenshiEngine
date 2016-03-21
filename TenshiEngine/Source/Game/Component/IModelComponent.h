#pragma once


#include "IComponent.h"

class Model;

class IModelComponent :public Component{
public:
	virtual void SetMatrix() = 0;
	Model* mModel;
};
