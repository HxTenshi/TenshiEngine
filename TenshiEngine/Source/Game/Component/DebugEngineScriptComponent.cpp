#include "DebugEngineScriptComponent.h"
#include <map>
#include <functional>
#include "Window/Window.h"

#include "DebugScript/DebugNaviMesh.h"

#include "Engine/Inspector.h"

class DebugEngineFactory{
public:
	DebugEngineFactory(){
		mFactory.insert(std::make_pair("DebugNaviMesh", [](){return new DebugNaviMesh(); }));
	}

	IDebugEngineScript* Get(const std::string name){
		auto func = mFactory.find(name);
		if (mFactory.end() == func)return NULL;

		return func->second();
	}
private:
	std::map<const std::string, std::function<IDebugEngineScript*(void)>> mFactory;
};
DebugEngineFactory gFactory;

DebugEngineScriptComponent::DebugEngineScriptComponent(){
	pClass = NULL;
}
DebugEngineScriptComponent::~DebugEngineScriptComponent(){
	if (pClass){
		delete pClass;
		pClass = NULL;
	}
}

void DebugEngineScriptComponent::Initialize(){
	if (!pClass){
		pClass = gFactory.Get(mClassName);
	}
	if (pClass){
		pClass->Initialize(gameObject);
	}
}
void DebugEngineScriptComponent::Start(){
}

#ifdef _ENGINE_MODE
void DebugEngineScriptComponent::EngineUpdate(){

	if (pClass){
		pClass->EngineUpdate();
	}
}
#endif
void DebugEngineScriptComponent::Update(){

	if (pClass){
		pClass->Update();
	}
}
void DebugEngineScriptComponent::Finish(){
	if (pClass){
		pClass->Finish();
	}

	
}

#ifdef _ENGINE_MODE
void DebugEngineScriptComponent::CreateInspector(){

	Inspector ins("DebugEngineScript",this);
	ins.AddEnableButton(this);

	ins.Add("Class", &mClassName, [&](std::string f){mClassName = f;
		if (pClass){
			delete pClass;
			pClass = NULL;

		}
		pClass = gFactory.Get(mClassName);
	});


	if (pClass){
		pClass->CreateInspector(&ins);
	}
	ins.Complete();
}
#endif

void DebugEngineScriptComponent::IO_Data(I_ioHelper* io){

	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)

	auto back = mClassName;
	_KEY(mClassName);

	if (back != mClassName){
		if (pClass){
			delete pClass;
			pClass = NULL;
		}
	}
	if (!pClass){
		pClass = gFactory.Get(mClassName);
	}

	if (pClass){
		pClass->IO_Data(io);
	}
#undef _KEY
}