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
	void SaveParam();
	void LoadParam();
	void Initialize() override;
	void Start() override;

	void Initialize_Script();
	void Start_Script();

	void Update() override;
	void Finish() override;

	void OnCollide(Actor* target);
	void LostCollide(Actor* target);



#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;
	IDllScriptComponent* pDllClass;
	std::string mClassName;

	std::unordered_map<int, Actor*> mCollideMap;
private:
	bool mEndInitialize;
	bool mEndStart;

	picojson::value* mSaveParam;
};


class ScriptManager{
public:

#ifdef _ENGINE_MODE
	static void ReCompile();
	static void CreateScriptFile(const std::string& className);
#endif
};