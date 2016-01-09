#pragma once

#include "IComponent.h"

class IDllScriptComponent;
class ScriptComponent : public Component{
public:
	ScriptComponent();
	~ScriptComponent();

	void Load();
	void Unload();
	void ReCompile();
	void Initialize() override;
	void Start() override;
	void Update() override;
	void Finish() override;

	void OnCollide(Actor* target);

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override{
#define _KEY(x) io->func( x , #x)
		_KEY(mClassName);

#undef _KEY
	}
	IDllScriptComponent* pDllClass;
	std::string mClassName;
};


class ScriptManager{
public:
	static void ReCompile();
};