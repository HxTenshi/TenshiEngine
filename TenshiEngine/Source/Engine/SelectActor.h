#pragma once

class Actor;
class PhysX3Main;
class EditorCamera;

#include "Graphic/Material/Material.h"
#include "Library/picojson.h"
#include <list>

class Selects{
public:
	Selects();
	~Selects();
	//àÍÇ¬ÇÃÇ›ÇÃëIë
	void OneSelect(Actor* act);
	//í«â¡Ç∆çÌèúÇÃêÿÇËë÷Ç¶
	void TriggerSelect(Actor* act);

	void MoveStart(XMVECTOR& vect);
	void MovePos(XMVECTOR& vect);
	void MoveEnd(XMVECTOR& vect);


	void Copy();
	void CopyDelete();
	void Paste();

	int SelectNum();
	XMVECTOR GetPosition();

	Actor* GetSelectOne();
	std::list<Actor*>& GetSelects();
private:
	std::list<Actor*> mSelects;
	std::list<picojson::value*> mCopy;
	XMVECTOR mLastVect;
};

class SelectActor{
public:
	SelectActor();
	void Finish();

	void Initialize();

	void SetSelect(Actor* select);
	Actor* GetSelectOne();

	void UpdateInspector();
	void Update(float deltaTime);

	void ReCreateInspector();

	void SelectActorDraw();
	bool ChackHitRay(PhysX3Main* physx, EditorCamera* camera);
private:

	Selects mSelects;
	Actor* mVectorBox;

	int mDragBox;
	XMVECTOR mDragPos;

	Material mSelectWireMaterial;
};