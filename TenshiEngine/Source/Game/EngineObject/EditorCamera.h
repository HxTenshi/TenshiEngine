#pragma once

#include "GameEngineObject.h"

#include "Game/Actor.h"
class CameraComponent;

class EditorCamera : public GameEngineObject{
public:
	EditorCamera();
	~EditorCamera();
	void Initialize();
	void Update(float deltaTime) override;

	XMVECTOR ScreenToWorldPoint(const XMVECTOR& point);

	XMVECTOR PointRayVector(const XMVECTOR& point);
	XMVECTOR GetPosition();

private:
	XMVECTOR	mRClickMousePos;
	Actor		mCamera;
	CameraComponent* mCameraComponent;
};