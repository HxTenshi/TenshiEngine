#pragma once


#ifdef _ENGINE_MODE

class Actor;
class PhysX3Main;
class EditorCamera;

struct SelectObjectDrawMode {
	enum Enum{
		None,
		Wire,
		ChildrenWire,
		Count
	};
};

#include "Graphic/Material/Material.h"
#include "Library/picojson.h"
#include <list>

class EditGuide{
public:
	EditGuide()
		:mSelectGuide(-1)
		, m_TransformMode(TransformMode::Local)
	{}
	virtual ~EditGuide(){}


	void SetGuideTransform(const XMVECTOR& pos, const XMVECTOR& quat);
	void SetGuideTransform(const XMVECTOR& pos);
	void UpdateGuideTransform(const XMVECTOR& pos, const XMVECTOR& quat);
	int SetGuideHit(Actor* act);
	void Update();

	void Enable();
	void Disable();
	bool IsTransformModeLocal();
	void ChangeTransformMode();

	virtual void GuideDrag(float pow) = 0;
	virtual void UpdateTransform(std::list<Actor*>& actors) = 0;
protected:
	shared_ptr<Actor> mGuide[3];
	int mSelectGuide;

	XMVECTOR mGuidePosition;
	XMVECTOR mGuideRotate;

	enum TransformMode {
		Local,
		World,
		Count,
	} m_TransformMode;
};

class Selects{
public:
	Selects();
	~Selects();
	//一つのみの選択
	void OneSelect(Actor* act);
	//追加と削除の切り替え
	bool TriggerSelect(Actor* act);

	void Copy();
	void CopyDelete();
	void Paste();

	void SetUndo();

	int SelectNum();
	XMVECTOR GetPosition();
	XMVECTOR GetQuaternion();

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
	void Update();

	void ReCreateInspector();
	void Draw();
	void DrawMeshWire(weak_ptr<Actor> object);
	void DrawPhysxWire(weak_ptr<Actor> object);
	bool ChackHitRay();

	void PushUndo();
private:

	Selects mSelects;
	bool mSelectAsset;
	bool mDontTreeViewSelect;

	int mCurrentGuide;
	EditGuide* mEditGuide[3];
	//shared_ptr<Actor> mVectorBox[3];
	bool mIsDragMode;
	//int mDragBox;
	//XMVECTOR mDragPos;

	Material mSelectWireMaterial;
	Material mSelectPhysxWireMaterial;
	bool mCreateInspector;
	std::string mAssetFileName;
	SelectUndo mSelectUndo;

	SelectObjectDrawMode::Enum m_DrawMode;
};

#endif