#include "PhysxLayer.h"
#include "Window/Window.h"
#include "Physx/Physx3.h"

PhysxLayer::PhysxLayer(){
	mPhysX3Main = NULL;
	_0x0 = true;
	_0x1 = true;
	_1x1 = true;
	i = 0;
	mSelects.push_back("None");
	mSelects.push_back("Layer1");
	mSelects.push_back("Layer2");
}

PhysxLayer::~PhysxLayer(){
	mPhysX3Main = NULL;
}

#ifdef _ENGINE_MODE
void PhysxLayer::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("0", &mSelects[0], [&](std::string f){
		mSelects[0] = f;
	}), data);
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("1", &mSelects[1], [&](std::string f){
		mSelects[1] = f;
	}), data);
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("2", &mSelects[2], [&](std::string f){
		mSelects[2] = f;
	}), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("0x0", &_0x0, [&](bool f){
		_0x0 = f;
		SetLayerFlag(0, 0, f);
	}), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("0x1", &_0x1, [&](bool f){
		_0x1 = f;
		SetLayerFlag(0, 1, f);
	}), data);
	Window::AddInspector(new TemplateInspectorDataSet<bool>("1x1", &_1x1, [&](bool f){
		_1x1 = f;
		SetLayerFlag(1,1,f);
	}), data);

	Window::AddInspector(new InspectorSelectDataSet("0x0", mSelects, &i, [&](int f){
		i=f;
	}), data);

	Window::AddInspector(new TemplateInspectorDataSet<int>("1x1", &i, [&](int f){
		i = f;
	}), data);
	Window::ViewInspector("PhysxLayer", NULL, data);


}
#endif


void PhysxLayer::SetLayerFlag(int l1, int l2, bool f){
	Layer::Enum layer1 = (Layer::Enum)(1 << l1);
	Layer::Enum layer2 = (Layer::Enum)(1 << l2);

	mPhysX3Main->SetLayerCollideFlag(layer1, layer2, f);
}