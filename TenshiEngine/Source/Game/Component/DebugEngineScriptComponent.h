#pragma once

struct InspectorDataSet;

#include "IComponent.h"
#include <unordered_map>


class IDebugEngineScript{
public:
	virtual ~IDebugEngineScript(){}
	virtual void Initialize(Actor* gameObject){ (void)gameObject; }
	virtual void EngineUpdate(){}
	virtual void Update(){}
	virtual void Finish(){}
#ifdef _ENGINE_MODE
	virtual void CreateInspector(std::vector<InspectorDataSet>& data){
		(void)data;
	}
#endif
	virtual void IO_Data(I_ioHelper* io){
		(void)io;
	}
};

class DebugEngineScriptComponent : public Component{
public:
	DebugEngineScriptComponent();
	~DebugEngineScriptComponent();

	void Initialize() override;
	void Start() override;

	void EngineUpdate() override;
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;
	IDebugEngineScript* pClass;
	std::string mClassName;

private:
	bool mEndInitialize;
};
