#pragma once

#include "../DebugEngineScriptComponent.h"

#include "Engine/AssetFile/NaviMesh/NaviMeshFileData.h"


class DebugNaviMesh : public IDebugEngineScript {
public:
	DebugNaviMesh();
	~DebugNaviMesh();
	void Initialize(Actor* gameObject) override;
	void EngineUpdate() override;
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector(std::vector<InspectorDataSet>& data) override;
#endif
	void IO_Data(I_ioHelper* io) override;

	void Find();

private:
	std::string mFileName;
	NaviMesh mNaviMesh;
	shared_ptr<NavigateCreator> mNaviMeshCreatorPtr;
	shared_ptr<Navigate> mNavigatePtr;
	NaviMeshPolygon* mStart;
	NaviMeshPolygon* mEnd;

	float mSpeed;
	Actor* mGameObject;
};