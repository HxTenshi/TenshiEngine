#pragma once

#include "IComponent.h"

#include "MySTL/ptr.h"

class IModelComponent;
class MaterialComponent;

class MeshDrawComponent :public Component{
public:
	MeshDrawComponent();

	void EngineUpdate() override;
	void Update() override;

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;

private:

	weak_ptr<IModelComponent> mModel;
	weak_ptr<MaterialComponent> mMaterial;
};