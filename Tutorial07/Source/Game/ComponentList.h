#pragma once

#define COMPONENTLIST_TYPE_MAP
//#define COMPONENTLIST_TYPE_UNORDERED_MAP

#ifdef COMPONENTLIST_TYPE_MAP
#include <map>
#endif
#ifdef COMPONENTLIST_TYPE_UNORDERED_MAP
#include <unordered_map>
#endif

#include <typeinfo.h>

#include "MySTL/Ptr.h"
class Actor;
#include "IComponent.h"

class ComponentList{
public:
	ComponentList(Actor* obj){
		gameObject = obj;
	}
	template<class T>
	void AddComponent(const shared_ptr<T>& spComponent){
		_AddComponent(typeid(*spComponent.Get()).hash_code(), spComponent);
		//mComponent.insert(std::pair<size_t, shared_ptr<Component>>(typeid(T).hash_code(), spComponent));
	}
	void _AddComponent(size_t hash, shared_ptr<Component> spComponent){
		mComponent.insert(std::pair<size_t, shared_ptr<Component>>(hash, spComponent));
		spComponent->_Initialize(gameObject);
	}

	//template<class T>
	//shared_ptr<T> GetComponent(){
	//	if (mComponent.find(typeid(T).hash_code()) == mComponent.end()){
	//		return shared_ptr<Component>();
	//	}
	//	return mComponent.at(typeid(T).hash_code());
	//}
	template<class T>
	shared_ptr<T> GetComponent() const{
		auto p = mComponent.find(typeid(T).hash_code());
		if (p == mComponent.end()){
			return shared_ptr<Component>();
		}
		return p->second;
	}

	template<class T>
	void RemoveComponent(){
		auto p = mComponent.find(typeid(T).hash_code());
		if (p != mComponent.end()){
			mComponent.erase(typeid(T).hash_code());
			return;
		}

	}
#ifdef COMPONENTLIST_TYPE_MAP
	std::map<size_t, shared_ptr<Component>> mComponent;
#endif
#ifdef COMPONENTLIST_TYPE_UNORDERED_MAP
	std::unordered_map<size_t, shared_ptr<Component>> mComponent;
#endif

private:
	Actor* gameObject;

};