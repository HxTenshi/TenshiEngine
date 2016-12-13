#pragma once

#include <d3d11.h>
#include "XNAMath/xnamath.h"
#include <functional>
#include <queue>
#include "Game/Component/ComponentList.h"
#include "Game/Parts/Enabled.h"
#include "Engine/AssetDataBase.h"

#include "Types.h"

#include "IActor.h"

class ITransformComponent;
class File;
class PrefabAssetData;
//PhysXテスト用
namespace physx{
	class PxTransform;
}

class Actor 
	: public IActor
	, public Enabled{
public:
	Actor();
	virtual ~Actor();
	void PlayInitializeStageColl();
	virtual void Initialize();
	virtual void Start();
	virtual void Finish();

#ifdef _ENGINE_MODE
	virtual void Initialize_Script();
	virtual void Start_Script();
	virtual void EngineUpdateComponent();
#endif
	virtual void UpdateComponent();
	virtual void Update();
	void SetInitializeStageCollQueue(const std::function<void()>& coll);
	void SetUpdateStageCollQueue(const std::function<void()>& coll);

	template<class T>
	weak_ptr<T> GetComponent(){
		return mComponents.GetComponent<T>();
	}
	weak_ptr<Component> GetComponent(const size_t& hash){
		return mComponents.GetComponent(hash);
	}

	template<class T>
	T* GetScript(){
		return (T*)_GetScript(typeid(T).name());
	}

	void* _GetScript(const char* name) override;

	template<class T>
	shared_ptr<T> AddComponent(shared_ptr<T> component){
		return mComponents.AddComponent<T>(component);
	}
	template<class T>
	shared_ptr<T> AddComponent(){
		return mComponents.AddComponent<T>();
	}
	template<class T>
	void RemoveComponent(){
		mComponents.RemoveComponent<T>();
	}
	void RemoveComponent(Component* comptr){
		mComponents.RemoveComponent(comptr);
	}

#ifdef _ENGINE_MODE
	virtual void CreateInspector();
#endif

	std::string Name() override{return mName;}
	void Name(const std::string& name) override{mName = name;}


	std::string Prefab(){ return mPrefab; }

	UniqueID GetUniqueID(){
		return mUniqueHash;
	}
	UniqueID GetBeforeUniqueID() {
		return mBeforeUniqueHash;
	}


	//ペアレント変更コールバックを実行
	void RunChangeParentCallback();

	//void PastePrefabParam(picojson::value& json);

	//void ExportSceneDataStart(const std::string& pass, File& sceneFile);
	//void ExportSceneData(const std::string& pass, File& sceneFile);
	void ExportData(const std::string& fileName, bool childExport = false, bool worldTransform = false);
	//void ExportData(const std::string& pass);
	void ImportData(const std::string& fileName, const std::function<void(shared_ptr<Actor>)>& childstackfunc = [](auto){}, bool newID = false);
	void ImportData(picojson::value& json, const std::function<void(shared_ptr<Actor>)>& childstackfunc = [](auto) {}, bool newID = false);
	bool ImportDataAndNewID(const std::string& fileName, const std::function<void(shared_ptr<Actor>)>& childstackfunc = [](auto) {});

	void ExportData(picojson::value& json, bool childExport=false, bool worldTransform = false);
	void ImportDataAndNewID(picojson::value& json, const std::function<void(shared_ptr<Actor>)>& childstackfunc = [](auto) {});

	void CreateNewID();

	shared_ptr<ITransformComponent> mTransform;

	void* mTreeViewPtr;
	void SetLayer(int layer)override;

	int GetLayer() override {
		return mPhysxLayer;
	}

	bool EndFinish() {
		return mEndFinish;
	}

	void SetInspectorFindGameObjectFunc(const std::function<weak_ptr<Actor>(const UniqueID&) >& func) {
		m_InspectorFindGameObjectFunc = func;
	}
	weak_ptr<Actor> InspectorFindGameObject(const UniqueID& id) {
		return m_InspectorFindGameObjectFunc(id);
	}
protected:
	ComponentList mComponents;

protected:


	virtual void _ExportData(I_ioHelper* io, bool childExport=false, bool worldTransform = false);
	virtual void _ImportData(I_ioHelper* io, const std::function<void(shared_ptr<Actor>)>& childstackfunc = [](auto) {}, bool newID = false);

	std::queue<std::function<void()>> mInitializeStageCollQueue;
	std::queue<std::function<void()>> mUpdateStageCollQueue;
	std::string mName;

	std::string mPrefab;
	PrefabAssetDataPtr mPrefabAsset;

	UniqueID mUniqueHash;
	UniqueID mBeforeUniqueHash;
	bool mEndInitialize;
	bool mEndStart;
	bool mEndFinish;

	int mPhysxLayer;

private:
	std::function<weak_ptr<Actor>(const UniqueID&)> m_InspectorFindGameObjectFunc;

	virtual void OnEnabled()override;
	virtual void OnDisabled()override;
};

namespace GameObjectFindHelper {
	//weak_ptr<Actor> GameSceneFind(const UniqueID& id);
	weak_ptr<Actor> ChildrenFind(GameObject obj, const UniqueID& id);
}