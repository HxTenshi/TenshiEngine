#pragma once

#include "IComponent.h"
#include<unordered_map>

void GenerateScriptProjectFile();

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

	enum ColliderState{
		Begin,
		Enter,
		//Exit,
	};
	struct ColliderStateData{
		ColliderStateData(
			GameObject target = NULL,
			ColliderState state = Begin,
			int count = 0)
			:Target(target), State(state), HitCount(count)
		{}
		GameObject Target;
		ColliderState State;
		int HitCount;
	};

	std::unordered_map<int, ColliderStateData> mCollideMap;
private:

	void InitParam();
	bool mEndInitialize;
	bool mEndStart;

	picojson::value* mSaveParam;
};
