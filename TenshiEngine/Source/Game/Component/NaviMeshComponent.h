#pragma once

#include "Component.h"
#include "Engine/AssetFile/NaviMesh/NaviMeshFileData.h"

class INaviMeshComponent {
public:
	INaviMeshComponent() {}
	virtual ~INaviMeshComponent(){}

	virtual void SetNaviMesh(MeshAsset mesh) = 0;
	virtual void RootCreate(GameObject start, GameObject end) = 0;
	virtual void Move(float speed) = 0;
	virtual bool IsMoveEnd() = 0;
	virtual XMVECTOR GetPosition() = 0;
	virtual XMVECTOR GetRouteVector() = 0;
	virtual float GetRemainderRouteDistance() = 0;
	virtual float GetRouteDistance() = 0;
};

class NaviMeshComponent : public INaviMeshComponent, public Component {
public:
	NaviMeshComponent();
	~NaviMeshComponent();
	void Initialize() override;
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif
	void IO_Data(I_ioHelper* io) override;

	void SetNaviMesh(MeshAsset mesh) override;
	void RootCreate(GameObject start, GameObject goal) override;
	//現在の位置を進める
	void Move(float speed) override;
	bool IsMoveEnd() override;
	//現在の位置
	XMVECTOR GetPosition()  override;
	//現在の位置から道の方向
	XMVECTOR GetRouteVector() override;
	//現在の位置から残りの距離
	float GetRemainderRouteDistance() override;
	//総ルート距離
	float GetRouteDistance() override;

private:

#ifdef _ENGINE_MODE
	bool m_EngineView;
#endif

	MeshAsset m_Mesh;
	NaviMesh mNaviMesh;
	shared_ptr<NavigateCreator> mNaviMeshCreatorPtr;
	shared_ptr<Navigate> mNavigatePtr;
	NaviMeshPolygon* mStart;
	NaviMeshPolygon* mEnd;

	GameObject mStartTarget;
	GameObject mEndTarget;
};