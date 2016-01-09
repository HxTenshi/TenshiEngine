#pragma once

class Actor;
class PhysX3Main;
class EditorCamera;

#include "Graphic/Material/Material.h"

class SelectActor{
public:
	SelectActor();
	void Finish();

	void Initialize();

	void SetSelect(Actor* select);
	Actor* GetSelect();

	void UpdateInspector();
	void Update(float deltaTime);

	void SelectActorDraw();
	bool ChackHitRay(PhysX3Main* physx, EditorCamera* camera);
private:

	Actor* mSelect;
	Actor* mVectorBox;

	int mDragBox;
	XMVECTOR mDragPos;

	Material mSelectWireMaterial;
};