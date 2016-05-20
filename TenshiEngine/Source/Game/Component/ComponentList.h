#pragma once



#define COMPONENTLIST_TYPE_MAP
//#define COMPONENTLIST_TYPE_UNORDERED_MAP

#ifdef COMPONENTLIST_TYPE_MAP
#include <list>
#endif
#ifdef COMPONENTLIST_TYPE_UNORDERED_MAP
#include <unordered_map>
#endif

#include <typeinfo.h>

#include "MySTL/Ptr.h"
class Actor;
#include "TransformComponent.h"
#include "IComponent.h"

#include <memory>
class ComponentList{
public:
	ComponentList(Actor* obj)
	{
		gameObject = obj;
		mInitializeEnd = false;
		mStartEnd = false;
	}
	void RunInitialize(){
		auto t = GetComponent<TransformComponent>();
		t->Initialize();
		for (auto& p : mComponent){
			if (p.second.Get() == t.Get())continue;
			p.second->Initialize();
		}
		mInitializeEnd = true;
	}
	void RunStart(){
		auto t = GetComponent<TransformComponent>();
		t->Start();
		for (auto& p : mComponent){
			if (p.second.Get() == t.Get())continue;
			p.second->Start();
		}
		mStartEnd = true;
	}
	void RunFinish(){
		auto t = GetComponent<TransformComponent>();
		for (auto& p : mComponent){
			if (p.second.Get() == t.Get())continue;
			p.second->Finish();
		}
		t->Finish();
		mInitializeEnd = false;
		mStartEnd = false;
	}


	template<class T>
	shared_ptr<T> AddComponent(const shared_ptr<T>& spComponent){
		//親クラスでアドしても平気
		_AddComponent(typeid(*spComponent.Get()).hash_code(), spComponent);
		spComponent->_Initialize(gameObject);
		if (mInitializeEnd)spComponent->Initialize();
		if (mStartEnd)spComponent->Start();
		return spComponent;
	}
	template<class T>
	shared_ptr<T> AddComponent(){
		auto spComponent = make_shared<T>();
		_AddComponent(typeid(T).hash_code(), spComponent);
		spComponent->_Initialize(gameObject);
		if (mInitializeEnd)spComponent->Initialize();
		if (mStartEnd)spComponent->Start();
		return spComponent;
	}
	template<class T>
	void _AddComponent(const size_t& hash,const shared_ptr<T>& spComponent){
		mComponent.push_back(std::make_pair(hash, spComponent));
	}

	template<class T>
	shared_ptr<T> GetComponent() const{
		const auto& v = typeid(T).hash_code();
		for (auto& p : mComponent){
			if (p.first == v)
				return p.second;
		}
		return NULL;
	}
	shared_ptr<Component> GetComponent(const size_t& hash) const{
		for (auto& p : mComponent){
			if (p.first == hash)
				return p.second;
		}
		return NULL;
	}

	shared_ptr<Component> GetComponent(const std::string& name) const{
		for (auto& p : mComponent){
			if (p.second->ClassName() == name)
				return p.second;
		}
		return NULL;
	}


	template<class T>
	void RemoveComponent(){
		const auto& v = typeid(T).hash_code();
		mComponent.remove_if([v](const item_type& i){
			if (i.first == v){
				i.second->Finish();
				return true;
			}
			return false;
		});
	}
	void RemoveComponent(Component* com){
		mComponent.remove_if([com](const item_type& i){
			if (i.second.Get() == com){
				i.second->Finish();
				return true;
			}
			return false;
		});
	}
#ifdef COMPONENTLIST_TYPE_MAP
	using item_type = std::pair<size_t, shared_ptr<Component>>;
	std::list<item_type> mComponent;
#endif
#ifdef COMPONENTLIST_TYPE_UNORDERED_MAP
	using item_type = std::pair<size_t, shared_ptr<Component>>;
	std::unordered_map<item_type> mComponent;
#endif

private:
	Actor* gameObject;

	bool mInitializeEnd;
	bool mStartEnd;

	friend Actor;

	template<class T>
	void AddComponent_NotInitialize(const shared_ptr<T>& spComponent){
		//親クラスでアドしても平気
		_AddComponent(typeid(*spComponent.Get()).hash_code(), spComponent);
		spComponent->_Initialize(gameObject);
	}

};