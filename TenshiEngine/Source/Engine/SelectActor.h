#pragma once


#ifdef _ENGINE_MODE

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
	bool TriggerSelect(Actor* act);

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

#include <map>
class ICommand;
class SelectUndo{
public:
	SelectUndo();
	~SelectUndo();
	void Clear();
	void Set(Actor* act);
	void Remove(Actor* act);
	void PushUndo();
private:
	std::map<int, ICommand*> mCommands;
};

class SelectActor{
public:
	SelectActor();
	void Finish();

	void Initialize();

	void ReleaseSelect(Actor* actor);
	void SetSelect(Actor* select,bool dontTreeViewSelect=false);
	void SetSelectAsset(Actor* select, const char* filename);
	Actor* GetSelectOne();

	void UpdateInspector();
	void Update(float deltaTime);

	void ReCreateInspector();

	void SelectActorDraw();
	bool ChackHitRay(PhysX3Main* physx, EditorCamera* camera);

	void PushUndo();
private:

	Selects mSelects;
	bool mSelectAsset;
	bool mDontTreeViewSelect;
	Actor* mVectorBox;

	int mDragBox;
	XMVECTOR mDragPos;

	Material mSelectWireMaterial;
	Material mSelectPhysxWireMaterial;
	bool mCreateInspector;
	std::string mAssetFileName;
	SelectUndo mSelectUndo;
};

#endif