#pragma once


#include <string>
#include <functional>
#include <map>

#include "MemberInfo_Data.h"


//前方宣言
struct Reflection;

//メンバ変数情報
struct MemberInfo{
public:
	MemberInfo(void* post, MemberInfo_Data& data)
		:Post(post)
		, Data(data){}
	std::string GetName(){
		return Data.memberName;
	}
	std::string GetTypeName(){
		return Data.typeName;
	}

private:

	MemberInfo& operator =(const MemberInfo&);
	void* Post;
	MemberInfo_Data& Data;

	friend Reflection;
};
//メンバ変数情報マップ
struct MemberInfos{
public:
	typedef std::map<std::string, MemberInfo_Data>::iterator iterator;

	MemberInfos(void* post, std::map<std::string, MemberInfo_Data>& data)
		:Post(post)
		, Data(data){}

	iterator begin(){
		return Data.begin();
	}
	iterator end(){
		return Data.end();
	}

private:
	MemberInfos& operator =(const MemberInfos&);

	void* Post;
	std::map<std::string, MemberInfo_Data>& Data;

	friend Reflection;
};



struct Reflection{

	//クラス名と変数名を指定してクラスのメンバ変数情報を取得
	static MemberInfo GetMemberInfo(void* post, const std::string& className, const std::string& memberName){
		return MemberInfo(post, (*map)[className][memberName]);
	}
	//クラス名とを指定してクラスのメンバ変数情報マップを取得
	static MemberInfos GetMemberInfos(void* post, const std::string& className){
		return MemberInfos(post, (*map)[className]);
	}
	//メンバ変数情報マップとマップ要素を指定してクラスのメンバ変数情報を取得
	template<class Pair>
	static MemberInfo GetMemberInfo(MemberInfos& infos, Pair& info){
		return MemberInfo(infos.Post, info.second);
	}

	//メンバ変数情報から変数を取得
	template<class T>
	static T* Get(MemberInfo& minfo){
		return (T*)minfo.Data.get(minfo.Post);
	}

	//メンバ変数情報から変数を設定
	template<class ValueType>
	static void Set(MemberInfo& minfo, ValueType&& Value){
		minfo.Data.set(minfo.Post, &Value);
	}

	static std::map<std::string, std::map<std::string, MemberInfo_Data>>* map;
private:
	Reflection();
};