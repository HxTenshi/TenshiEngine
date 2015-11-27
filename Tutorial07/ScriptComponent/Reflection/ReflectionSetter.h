#pragma once


#include <string>
#include <map>
#include <memory>
#include "MySTL/Reflection/MemberInfo_Data.h"


struct ReflectionSetter{
private:

	static std::map<std::string, std::map<std::string, MemberInfo_Data>> map;

public:
	ReflectionSetter();

	decltype(&map) GetReflectionDatas(){
		return &map;
	}

private:

	//ƒŠƒtƒŒƒNƒVƒ‡ƒ“î•ñ’Ç‰Á
	void create(const std::string& className, const std::string& memberName, const std::string& typeName, const MemberInfo_Data::GetType get, const MemberInfo_Data::SetType set){
		MemberInfo_Data mf;
		mf.className = className;
		mf.memberName = memberName;
		mf.typeName = typeName;
		mf.get = get;
		mf.set = set;

		auto mem = std::make_pair(mf.memberName, mf);

		auto& memmap = map[mf.className];
		memmap.insert(mem);
	}
};
