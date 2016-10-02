#pragma once

#include "IComponent.h"

#include "MySTL/ptr.h"

class IModelComponent;
class MaterialComponent;

class MeshDrawComponent :public Component{
public:
	MeshDrawComponent();
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;

private:

	weak_ptr<IModelComponent> mModel;
	weak_ptr<MaterialComponent> mMaterial;
};