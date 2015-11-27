#pragma once

#include <functional>
#include "MySTL/File.h"
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

	void ExportData(File& f) override{
		ExportClassName(f);
		f.Out(mClassName);
	}
	void ImportData(File& f) override{
		(void)f;
		f.In(&mClassName);
	}

	IDllScriptComponent* pDllClass;
	std::string mClassName;
};


class ScriptManager{
public:
	static void ReCompile();
};