#pragma once

#include "../DebugEngineScriptComponent.h"

#include "Engine/AssetFile/NaviMesh/NaviMeshFileData.h"


class DebugNaviMesh : public IDebugEngineScript {
public:
	DebugNaviMesh();
	~DebugNaviMesh();
	void Initialize(GameObject gameObject) override;
	void EngineUpdate() override;
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector(Inspector* ins) override;
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
	GameObject mGameObject;
};