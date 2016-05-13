#pragma once


#include "IComponent.h"

class Model;
class MeshComponent;
#include <vector>
#include "MySTL/ptr.h"

class IModelComponent :public Component{
public:
	virtual void SetMatrix() = 0;
	Model* mModel;

	std::vector<weak_ptr<MeshComponent>>& GetMeshComVector(){ return m_MeshComVector; }

protected:
	std::vector<weak_ptr<MeshComponent>> m_MeshComVector;
};
