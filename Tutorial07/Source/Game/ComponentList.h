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
class DrawComponent;

class ComponentList{
public:
	ComponentList(Actor* obj){
		gameObject = obj;
	}
	template<class T>
	void AddComponent(const shared_ptr<T>& spComponent){
		_AddComponent(typeid(*spComponent.Get()).hash_code(), spComponent, spComponent.Get());
		//mComponent.insert(std::pair<size_t, shared_ptr<Component>>(typeid(T).hash_code(), spComponent));
	}
	void _AddComponent(size_t hash, shared_ptr<Component> spComponent, Component* ptr){
		mComponent.insert(std::pair<size_t, shared_ptr<Component>>(hash, spComponent));
		ptr->_Initialize(gameObject);
	}
	void _AddComponent(size_t hash, shared_ptr<DrawComponent> spComponent, DrawComponent* ptr){
		mDrawComponent.insert(std::pair<size_t, shared_ptr<Component>>(hash, spComponent));
		ptr->_Initialize(gameObject);
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
			p = mDrawComponent.find(typeid(T).hash_code());
			if (p == mDrawComponent.end()){
				return shared_ptr<Component>();
			}
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
		p = mDrawComponent.find(typeid(T).hash_code());
		if (p != mDrawComponent.end()){
			mDrawComponent.erase(typeid(T).hash_code());
			return;
		}

	}
#ifdef COMPONENTLIST_TYPE_MAP
	std::map<size_t, shared_ptr<Component>> mComponent;
	std::map<size_t, shared_ptr<Component>> mDrawComponent;
#endif
#ifdef COMPONENTLIST_TYPE_UNORDERED_MAP
	std::unordered_map<size_t, shared_ptr<Component>> mComponent;
#endif

private:
	Actor* gameObject;

};