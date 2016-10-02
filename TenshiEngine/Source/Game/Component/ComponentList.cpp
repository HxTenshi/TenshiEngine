
#include "ComponentList.h"
#include "TransformComponent.h"

ComponentList::ComponentList()
{
	gameObject = NULL;
	mInitializeEnd = false;
	mStartEnd = false;
}
void ComponentList::Initialize(GameObject obj) {

	gameObject = obj;
}
void ComponentList::RunInitialize() {
	auto t = GetComponent<TransformComponent>();
	t->_Initialize(gameObject);
	t->Initialize();
	for (auto& p : mComponent) {
		if (p.second.Get() == t.Get())continue;
		p.second->_Initialize(gameObject);
		p.second->Initialize();
	}
	mInitializeEnd = true;
}
void ComponentList::RunStart() {
	auto t = GetComponent<TransformComponent>();
	t->Start();
	for (auto& p : mComponent) {
		if (p.second.Get() == t.Get())continue;
		p.second->Start();
	}
	mStartEnd = true;
}
void ComponentList::RunFinish() {
	auto t = GetComponent<TransformComponent>();
	for (auto& p : mComponent) {
		if (p.second.Get() == t.Get())continue;
		p.second->Finish();
	}
	t->Finish();
	mInitializeEnd = false;
	mStartEnd = false;
}