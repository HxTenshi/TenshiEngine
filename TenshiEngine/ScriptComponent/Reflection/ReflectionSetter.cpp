
#include "ReflectionSetter.h"

#define private public
#include "System\include"
#undef private

#include<type_traits>

//static
decltype(ReflectionSetter::map) ReflectionSetter::map;

ReflectionSetter reflection;

//修飾削除
template<class T>
struct _cc{
	typedef typename std::remove_cv<T>::type type;
};

//代入
template<class T>
struct _refhelper{
	typedef typename _cc<T>::type type;
	inline static void in(type* post, void* send){
		*post = *(type*)send;
	}
	inline static void in(const type* post, void* send){
		*const_cast<type*>(post) = *(type*)send;
	}
};

//コネクタ
struct refhelper{
	template<class T>
	inline static void in(T* post, void* send){
		_refhelper<T>::in(post, send);
	}
};

ReflectionSetter::ReflectionSetter(){
#define _REF(PostType,menber) \
create(\
	#PostType, #menber,\
	typeid(decltype(PostType::menber)).name(),\
	[](void* post)\
		{\
			return (void*)&((PostType*)post)->menber;\
		},\
	[](void* post, void* Value)\
		{\
			refhelper::in(&((PostType*)post)->menber, Value);\
		}\
	)

	//((PostType*)post)->menber = *(decltype(PostType::menber)*)Value;

	//REF(TextureScroll, x);
	//REF(TextureScroll, y);
	//ここにコンポーネントで表示したい変数を追加します

#include "System\reflection.h"


	
#undef REF
}



std::map<std::string, std::map<std::string, MemberInfo_Data>>* GetReflectionData(){
	return reflection.GetReflectionDatas();
	
}