#pragma once

#include "GameEngineObject.h"

#include "Game/Actor.h"
class CameraComponent;

class EditorCamera : public GameEngineObject{
public:
	EditorCamera();
	~EditorCamera();
	void Initialize();
	void Update() override;

	XMVECTOR ScreenToWorldPoint(const XMVECTOR& point);

	XMVECTOR PointRayVector(const XMVECTOR& point);
	XMVECTOR GetPosition();

	void GoActorPosition(Actor* actor);

private:
	XMVECTOR	mRClickMousePos;
	shared_ptr<Actor>		mCamera;
	CameraComponent* mCameraComponent;

	std::function<void(float)> mUpdateFunc;
};