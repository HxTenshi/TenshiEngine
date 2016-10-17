
#include "ComponentList.h"
#include "TransformComponent.h"
#include "ScriptComponent.h"

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
	auto s = GetComponent<ScriptComponent>();
	t->_Initialize(gameObject);
	t->Initialize();
	for (auto& p : mComponent) {
		if (p.second.Get() == t.Get())continue;
		if (p.second.Get() == s.Get())continue;
		p.second->_Initialize(gameObject);
		p.second->Initialize();
	}
	if (s) {
		s->_Initialize(gameObject);
		s->Initialize();
	}
	mInitializeEnd = true;
}
void ComponentList::RunStart() {
	auto t = GetComponent<TransformComponent>();
	auto s = GetComponent<ScriptComponent>();
	t->Start();
	for (auto& p : mComponent) {
		if (p.second.Get() == t.Get())continue;
		if (p.second.Get() == s.Get())continue;
		p.second->Start();
	}
	if (s) {
		s->Start();
	}
	mStartEnd = true;
}
void ComponentList::RunFinish() {
	auto t = GetComponent<TransformComponent>();
	auto s = GetComponent<ScriptComponent>();
	if (s) {
		s->Finish();
	}
	for (auto& p : mComponent) {
		if (p.second.Get() == t.Get())continue;
		if (p.second.Get() == s.Get())continue;
		p.second->Finish();
	}
	t->Finish();
	mInitializeEnd = false;
	mStartEnd = false;
}