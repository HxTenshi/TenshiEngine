#pragma once


#include <unordered_map>
#include <functional>
#include "MySTL/ptr.h"

class Component;


class ComponentFactory{

public:
	//クラス名からコンポーネント作成
	static
		shared_ptr<Component> Create(const std::string& ClassName);

	//登録されているコンポーネント一覧を取得
	static
		std::unordered_map<std::string, std::function<shared_ptr<Component>()>>& GetComponents();
private:
	template<class T>
	static
		shared_ptr<Component> _Make(){
		return make_shared<T>();
	}

	template<class T>
	static
		void _NewFunc(){
		mFactoryComponents[typeid(T).name()] = &ComponentFactory::_Make<T>;
	}

	static
		shared_ptr<Component> _Create(std::string ClassName);

	//登録コンポーネント一覧
	static
		std::unordered_map<std::string, std::function<shared_ptr<Component>()>> mFactoryComponents;

protected:
	ComponentFactory();
};