#pragma once

#include "IComponent.h"
#include<unordered_map>

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
	void LostCollide(Actor* target);

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;
	IDllScriptComponent* pDllClass;
	std::string mClassName;

	std::unordered_map<int, Actor*> mCollideMap;
private:
	bool mEndInitialize;
	bool mEndStart;
};


class ScriptManager{
public:
	static void ReCompile();
	static void CreateScriptFile(const std::string& className);
};