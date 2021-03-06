#pragma once

struct InspectorDataSet;

#include "IComponent.h"
#include <unordered_map>

class Inspector;

class IDebugEngineScript{
public:
	virtual ~IDebugEngineScript(){}
	virtual void Initialize(GameObject gameObject){ (void)gameObject; }
	virtual void EngineUpdate(){}
	virtual void Update(){}
	virtual void Finish(){}
#ifdef _ENGINE_MODE
	virtual void CreateInspector(Inspector* data){
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

#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
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
